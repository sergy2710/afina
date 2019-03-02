#include "SimpleLRU.h"

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Put(const std::string &key, const std::string &value)
{
    return SimpleLRU::UpdateNode(key, value, key.size() + value.size());
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::PutIfAbsent(const std::string &key, const std::string &value)
{
    if (_lru_index.find(key) != _lru_index.end()) return false;
    return SimpleLRU::UpdateNode(key, value, key.size() + value.size());
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Set(const std::string &key, const std::string &value)
{
    if (_lru_index.find(key) == _lru_index.end()) return false;
    return SimpleLRU::UpdateNode(key, value, key.size() + value.size());
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Delete(const std::string &key)
{
    auto found_node_iterator = _lru_index.find(key);  
    if (found_node_iterator == _lru_index.end()) return false;
    else
    {
        lru_node* found_node = &(found_node_iterator->second.get()); 
        if (!SimpleLRU::RecountCurrentSize((-1) * (found_node->key.size() + found_node->value.size()))) return false;
        if (found_node->prev != nullptr) found_node->prev->next.swap(found_node->next);
        if (found_node->next != nullptr) found_node->next->prev = found_node->prev; 
        _lru_index.erase(key);
        found_node->next.reset();     
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value)
{
    auto found_node_iterator = _lru_index.find(key);
    if (found_node_iterator == _lru_index.end()) return false;
    else
    {
        value = (found_node_iterator->second.get()).value;
        lru_node* found_node = &(found_node_iterator->second.get());
        if (found_node != _lru_tail)
        {
            if (found_node != _lru_head.get())
            {
                found_node->next->prev = found_node->prev;
                found_node->next.swap(found_node->prev->next); 
                found_node->prev = _lru_tail;
                found_node->next.swap(_lru_tail->next);
            }
            else
            {
                _lru_head->next->prev = nullptr;
                _lru_tail->next.swap(_lru_head);
                _lru_head.swap(_lru_tail->next->next);
            }
            _lru_tail = _lru_tail->next.get();
        }
        return true;
    }
}

bool SimpleLRU::ClearMemory(const std::size_t needed_size) 
{
    if (_lru_head != nullptr)
    {
        std::size_t cleared_size = _lru_head->key.size() + _lru_head->value.size();
        if (!SimpleLRU::RecountCurrentSize((-1) * cleared_size)) return false;
        _lru_index.erase(_lru_index.find(_lru_head->key));

        std::unique_ptr<lru_node> prev_head{new lru_node()};
        prev_head.swap(_lru_head);
        _lru_head.swap(prev_head->next);
        if (cleared_size >= needed_size) return true;
        else return SimpleLRU::ClearMemory(needed_size - cleared_size);
    }
    else return false;  
}

bool SimpleLRU::RecountCurrentSize(const std::size_t increment)
{
    std::size_t new_size = _current_size + increment;
    if ((new_size < 0) | (new_size > _max_size)) return false;
    else
    {
        _current_size = new_size;
        _empty_size -= increment;
        return true;
    }
}

bool SimpleLRU::UpdateNode(const std::string &key, const std::string &value, const std::size_t increment)
{
    if (_lru_index.find(key) != _lru_index.end()) if (!SimpleLRU::Delete(key)) return false;
    if (_empty_size < increment) if (!SimpleLRU::ClearMemory(increment)) return false;

    std::unique_ptr<lru_node> new_lru_node{new lru_node(key, value, _lru_tail)};
    _lru_index.insert(make_pair(std::reference_wrapper<const std::string>(new_lru_node->key),
                                std::reference_wrapper<lru_node>(*new_lru_node)));
    if (!SimpleLRU::RecountCurrentSize(increment)) return false;
    if (_lru_head != nullptr)
    {
        _lru_tail->next.swap(new_lru_node);
        _lru_tail = _lru_tail->next.get();
    }
    else
    {
        _lru_head.swap(new_lru_node);
        _lru_tail = _lru_head.get();
    }
    return true;
}

} // namespace Backend
} // namespace Afina
