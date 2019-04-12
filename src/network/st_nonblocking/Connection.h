#ifndef AFINA_NETWORK_ST_NONBLOCKING_CONNECTION_H
#define AFINA_NETWORK_ST_NONBLOCKING_CONNECTION_H

#include <cstring>
#include <iostream>
#include <list>

#include <sys/epoll.h>

#include <spdlog/logger.h>

#include "protocol/Parser.h"
#include <afina/Storage.h>
#include <afina/execute/Command.h>

namespace Afina {
namespace Network {
namespace STnonblock {

class Connection {
public:
    Connection(int s, std::shared_ptr<spdlog::logger> log, std::shared_ptr<Afina::Storage> ps)
        : _socket(s), _logger(log), pStorage(ps) {
        std::memset(&_event, 0, sizeof(struct epoll_event));
        _event.data.ptr = this;
    }

    inline bool isAlive() const { return _live; }

    void Start();

protected:
    void OnError();
    void OnClose();
    void DoRead();
    void DoWrite();

private:
    friend class ServerImpl;

    static constexpr int EVENT_READ = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP | EPOLLET;
    static constexpr int EVENT_READ_WRITE = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLERR | EPOLLHUP | EPOLLET;

    int _socket;
    struct epoll_event _event;

    std::shared_ptr<spdlog::logger> _logger;
    std::shared_ptr<Afina::Storage> pStorage;

    bool _live;

    std::list<std::string> _write_buffers;
    std::size_t _write_offset;
    char _read_buffer[4096];
    int readed_bytes;

    std::size_t arg_remains;
    Protocol::Parser parser;
    std::string argument_for_command;
    std::unique_ptr<Execute::Command> command_to_execute;
};

} // namespace STnonblock
} // namespace Network
} // namespace Afina

#endif // AFINA_NETWORK_ST_NONBLOCKING_CONNECTION_H
