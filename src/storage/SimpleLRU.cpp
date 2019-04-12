#include "SimpleLRU.h"
#include <assert.h>

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Put(const std::string &key, const std::string &value) {
    return SimpleLRU::UpdateNode(key, value, _lru_index.find(key));
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::PutIfAbsent(const std::string &key, const std::string &value) {
    if (_lru_index.find(key) != _lru_index.end())
        return false;
    return SimpleLRU::UpdateNode(key, value, _lru_index.end());
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Set(const std::string &key, const std::string &value) {
    const lru_map::iterator elem_it = _lru_index.find(key);
    if (elem_it == _lru_index.end())
        return false;
    return SimpleLRU::UpdateNode(key, value, elem_it);
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Delete(const std::string &key) {
    const lru_map::iterator elem_it = _lru_index.find(key);
    if (elem_it == _lru_index.end())
        return false;
    else {
        lru_node *found_node = &(elem_it->second.get());
        SimpleLRU::RecountCurrentSize((-1) * (found_node->key.size() + found_node->value.size()));
        if (found_node->next != nullptr)
            found_node->next->prev = found_node->prev;
        _lru_index.erase(key);
        found_node->prev->next.swap(found_node->next);
        found_node->next.reset();
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value) {
    const lru_map::iterator elem_it = _lru_index.find(key);

    if (elem_it == _lru_index.end())
        return false;
    else {
        lru_node *found_node = &(elem_it->second.get());
        value = found_node->value;
        SimpleLRU::MoveToTail(found_node);
        return true;
    }
}

bool SimpleLRU::ClearMemory(const std::size_t needed_size) {
    if (needed_size <= _current_size) {
        lru_node *deleted_node = _lru_head->next.get();
        std::size_t cleared_size = deleted_node->key.size() + deleted_node->value.size();

        SimpleLRU::RecountCurrentSize((-1) * cleared_size);
        _lru_index.erase(_lru_index.find(deleted_node->key));
        _lru_head->next.swap(deleted_node->next);
        _lru_head->next->prev = _lru_head.get();
        deleted_node->next.reset();

        if (cleared_size >= needed_size)
            return true;
        else
            return SimpleLRU::ClearMemory(needed_size - cleared_size);
    } else
        return false;
}

void SimpleLRU::RecountCurrentSize(const std::size_t increment) {
    std::size_t new_size = _current_size + increment;
    assert((new_size >= 0) & (new_size <= _max_size));
    _current_size = new_size;
    _empty_size -= increment;
}

bool SimpleLRU::UpdateNode(const std::string &key, const std::string &value, const lru_map::iterator elem_it) {
    int increment;
    if (elem_it != _lru_index.end())
        increment = value.size() - elem_it->second.get().value.size();
    else
        increment = key.size() + value.size();

    if (increment <= _max_size) // to not destroy elements if we don't have enough memory after freeing
    {
        if (_empty_size < increment)
            if (!SimpleLRU::ClearMemory(increment))
                return false;
        SimpleLRU::RecountCurrentSize(increment);

        if (elem_it != _lru_index.end()) {
            lru_node *found_node = &(elem_it->second.get());
            found_node->value = value;
            SimpleLRU::MoveToTail(found_node);
        } else {
            std::unique_ptr<lru_node> new_lru_node{new lru_node(key, value, _lru_tail)};
            _lru_index.insert(make_pair(std::reference_wrapper<const std::string>(new_lru_node->key),
                                        std::reference_wrapper<lru_node>(*new_lru_node)));

            _lru_tail->next.swap(new_lru_node);
            _lru_tail = _lru_tail->next.get();
        }
        return true;
    } else
        return false;
}

void SimpleLRU::MoveToTail(lru_node *found_node) {
    if (found_node != _lru_tail) {
        found_node->next->prev = found_node->prev;
        found_node->prev->next.swap(found_node->next);
        found_node->prev = _lru_tail;
        _lru_tail->next.swap(found_node->next);
        _lru_tail = found_node;
    }
}

} // namespace Backend
} // namespace Afina
