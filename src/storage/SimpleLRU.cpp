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
    auto lru_find = _lru_index.find(key);  
    lru_node* found_node = &(lru_find->second.get());
    if (lru_find == _lru_index.end()) return false;
    else
    {
        if (!SimpleLRU::RecountCurrentSize((-1) * (found_node->key.size() + found_node->value.size()))) return false;
        if (found_node->prev != NULL)
        {
            found_node->prev->next = found_node->next;        
        }
        if (found_node->next != NULL)
        {
            found_node->next->prev = found_node->prev;        
        }  
        
        _lru_index.erase(key);
    }
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value)
{
    auto lru_find = _lru_index.find(key);
    if (lru_find == _lru_index.end()) return false;
    else value = (lru_find->second.get()).value;
    if (!SimpleLRU::MoveToTail(&(lru_find->second.get()))) return false;
    return true;
}

bool SimpleLRU::ClearMemory(const std::size_t needed_size) // want to clear memory for some data sizeof needed_size
{
    if (_lru_head != NULL)
    {
        lru_node* temp_node = _lru_head->next;
        auto deleted_node = _lru_index.find(_lru_head->key);
        lru_node* deleted_node_pointer = &(deleted_node->second.get()); 
        std::size_t cleared_size = deleted_node_pointer->key.size() + deleted_node_pointer->value.size();
        if (!SimpleLRU::RecountCurrentSize((-1) * cleared_size)) return false;

        _lru_index.erase(deleted_node);
        _lru_head = temp_node;

        if (cleared_size >= needed_size)
        {
            return true;
        }
        else
        {
            return SimpleLRU::ClearMemory(needed_size - cleared_size);
        }
    }
    else
    {
        return false;
    }    
}

bool SimpleLRU::RecountCurrentSize(const std::size_t increment)
{
    std::size_t new_size = _current_size + increment;
    if ((new_size < 0) || (new_size > _max_size)) // if new_size < 0 something went wrong previously
    {
        return false;
    }
    else
    {
        _current_size = new_size;
        _empty_size -= increment;
        return true;
    }
}

bool SimpleLRU::UpdateNode(const std::string &key, const std::string &value, const std::size_t increment)
{
    if (_lru_index.find(key) != _lru_index.end())
    {
        if (!SimpleLRU::Delete(key)) return false;   
    }

    if (_empty_size < increment)
    {
        if (!SimpleLRU::ClearMemory(increment)) return false;
    }

    lru_node* new_lru_node = new lru_node(key, value, NULL, NULL);
    _lru_index.insert(make_pair(std::reference_wrapper<const std::string>(new_lru_node->key), \
                                std::reference_wrapper<lru_node>(*new_lru_node)));
    if (!SimpleLRU::RecountCurrentSize(increment)) return false;
    if (!SimpleLRU::MoveToTail(new_lru_node)) return false;

    return true;
}

bool SimpleLRU::MoveToTail(lru_node* node)
{
    if (_lru_head != NULL)
    {
        if (_lru_tail != NULL)
        {
            if (node != _lru_tail)
            {
                if (node != _lru_head)
                {
                    if ((node->prev != NULL) && (node->next != NULL))
                    { 
                        node->prev->next = node->next;
                        node->next->prev = node->prev;
                    }
                    node->prev = _lru_tail;
                    node->next = NULL;
                    node->prev->next = node;
                    _lru_tail = node;
                    return true;
                }
                else
                {
                    _lru_head->next->prev = NULL;
                    _lru_head = _lru_head->next;
                    node->prev = _lru_tail;
                    node->next = NULL;
                    _lru_tail->next = node;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        else
        {
            _lru_head->next = node;
            node->prev = _lru_head;
            _lru_tail = node;
            return true;
        }  
    }      
    else
    {
        _lru_head = node;
        return true;
    }    
}

} // namespace Backend
} // namespace Afina
