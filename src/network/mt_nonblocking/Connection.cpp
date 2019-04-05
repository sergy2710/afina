#include "Connection.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <chrono>
#include <thread>

#include <list>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <spdlog/logger.h>

#include <afina/Storage.h>
#include <afina/execute/Command.h>
#include <afina/logging/Service.h>

#include "protocol/Parser.h"

#include <sys/ioctl.h>

namespace Afina {
namespace Network {
namespace MTnonblock {

// See Connection.h
void Connection::Start() {
    _logger->info("Start on descriptor {}", _socket);
    std::lock_guard<std::mutex> lg{_mutex};
    _live = true;
    _event.data.fd = _socket;
    _event.data.ptr = this;
    _event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;

    readed_bytes = 0;
    _write_offset = 0;
    arg_remains = 0;
    command_to_execute = nullptr;
    parser = Protocol::Parser{};
    argument_for_command.clear();
    _write_buffers.clear();
}

// See Connection.h
void Connection::OnError() {
    _logger->info("OnError on descriptor {}", _socket);
    std::lock_guard<std::mutex> lg{_mutex};
    _live = false;
    shutdown(_socket, SHUT_RDWR);
    _logger->error("Error connection on descriptor {}", _socket);
    // std::string err_message = "Error has happened\r\n"; // TODO send some info about error ?
    // send(_socket, err_message.data(), err_message.size(), 0)
}

// See Connection.h
void Connection::OnClose() {
    _logger->info("OnClose on descriptor {}", _socket);
    std::lock_guard<std::mutex> lg{_mutex};
    _live = false;
    shutdown(_socket, SHUT_RDWR);
    _logger->debug("Closed connection on descriptor {}", _socket);
    // std::string close_message = "Connection is closed\r\n"; // TODO send some info why connection closed ?
    // send(_socket, close_message.data(), close_message.size(), 0);
}

// See Connection.h
void Connection::DoRead() {
    _logger->info("DoRead on descriptor {}", _socket);
    std::lock_guard<std::mutex> lg{_mutex};

    _event.events |= EPOLLOUT;
    _event.data.ptr = this;

    try {
        int readed_bytes_ = -1;

        while ((readed_bytes_ = read(_socket, _read_buffer + readed_bytes, sizeof(_read_buffer) - readed_bytes)) > 0) {

            _logger->debug("Got {} bytes from socket", readed_bytes_);

            readed_bytes += readed_bytes_;

            // Single block of data readed from the socket could trigger inside actions a multiple times,
            // for example:
            // - read#0: [<command1 start>]
            // - read#1: [<command1 end> <argument> <command2> <argument for command 2> <command3> ... ]
            while (readed_bytes > 0) {
                _logger->debug("Process {} bytes", readed_bytes);

                // There is no command yet
                if (!command_to_execute) {
                    std::size_t parsed = 0;
                    if (parser.Parse(_read_buffer, readed_bytes, parsed)) {
                        // There is no command to be launched, continue to parse input stream
                        // Here we are, current chunk finished some command, process it
                        _logger->debug("Found new command: {} in {} bytes", parser.Name(), parsed);
                        command_to_execute = parser.Build(arg_remains);
                        if (arg_remains > 0) {
                            arg_remains += 2;
                        }
                    }

                    // Parsed might fails to consume any bytes from input stream. In real life that could happens,
                    // for example, because we are working with UTF-16 chars and only 1 byte left in stream
                    if (parsed == 0) {
                        break;
                    } else {
                        std::memmove(_read_buffer, _read_buffer + parsed, readed_bytes - parsed);
                        readed_bytes -= parsed;
                    }
                }

                // There is command, but we still wait for argument to arrive...
                if (command_to_execute && arg_remains > 0) {
                    _logger->debug("Fill argument: {} bytes of {}", readed_bytes, arg_remains);
                    // There is some parsed command, and now we are reading argument
                    std::size_t to_read = std::min(arg_remains, std::size_t(readed_bytes));
                    argument_for_command.append(_read_buffer, to_read);

                    std::memmove(_read_buffer, _read_buffer + to_read, readed_bytes - to_read);
                    arg_remains -= to_read;
                    readed_bytes -= to_read;
                }

                // Thre is command & argument - RUN!
                if (command_to_execute && arg_remains == 0) {
                    _logger->debug("Start command execution");

                    _logger->debug("arguments {}", argument_for_command);

                    std::string result;
                    try {
                        command_to_execute->Execute(*pStorage, argument_for_command, result);
                        result += "\r\n";
                        _write_buffers.push_back(result);
                    } catch (std::runtime_error &ex) {
                        _logger->error("Failed to Execute {}", ex.what());
                        result = "SERVER_ERROR " + std::string(ex.what()) + "\r\n";
                        if (send(_socket, result.data(), result.size(), 0) <= 0) {
                            throw std::runtime_error("Failed to send response about server error");
                        }
                    }

                    // Prepare for the next command
                    command_to_execute.reset();
                    argument_for_command.resize(0);
                    parser.Reset();
                }
            } // while (readed_bytes)
        }

        if (readed_bytes == 0) {
            _logger->debug("Readed 0 bytes in DoRead");
        } else {
            throw std::runtime_error(std::string(strerror(errno)));
        }

    } catch (std::runtime_error &ex) {
        _logger->error("Failed to process connection on descriptor {}: {}", _socket, ex.what());
    }
}

// See Connection.h
void Connection::DoWrite() {
    _logger->info("DoWrite on descriptor {}\n", _socket);
    std::lock_guard<std::mutex> lg{_mutex};

    int available_space;
    ioctl(_socket, FIONREAD, &available_space);

    std::string result = _write_buffers.front();
    _write_buffers.pop_front();
    result.erase(0, _write_offset);
    if (available_space > result.size()) {
        if (send(_socket, result.data(), result.size(), 0) <= 0) {
            throw std::runtime_error("Failed to send response");
        }
    } else {
        _write_offset = available_space;
        _write_buffers.push_front(result.substr(available_space, result.size()));
        result.erase(result.size() - available_space, result.size());
        if (send(_socket, result.data(), result.size(), 0) <= 0) {
            throw std::runtime_error("Failed to send response");
        }
    }

    while ((available_space > 0) && (!_write_buffers.empty())) {
        result = _write_buffers.front();
        _write_buffers.pop_front();
        if (available_space > result.size()) {
            if (send(_socket, result.data(), result.size(), 0) <= 0) {
                throw std::runtime_error("Failed to send response");
            }
        } else {
            _write_offset = available_space;
            _write_buffers.push_front(result.substr(available_space, result.size()));
            result.erase(result.size() - available_space, result.size());
            if (send(_socket, result.data(), result.size(), 0) <= 0) {
                throw std::runtime_error("Failed to send response");
            }
        }
    }
    _event.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
}

} // namespace MTnonblock
} // namespace Network
} // namespace Afina
