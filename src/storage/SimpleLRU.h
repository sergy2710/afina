#ifndef AFINA_STORAGE_SIMPLE_LRU_H
#define AFINA_STORAGE_SIMPLE_LRU_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <afina/Storage.h>

namespace Afina {
namespace Backend {

/**
 * # Map based implementation
 * That is NOT thread safe implementaiton!!
 */

class SimpleLRU : public Afina::Storage {
public:
    SimpleLRU(std::size_t max_size = 1024) : _max_size(max_size) {}

    ~SimpleLRU() {
        _lru_index.clear();
        
        while (_lru_head->next != nullptr)
        {
            std::unique_ptr<lru_node> tmp_node{new lru_node()};
            tmp_node.swap(_lru_head->next);
            _lru_head->next.swap(tmp_node->next);
            tmp_node.reset();
        }
        _lru_head.reset();
    }

    // Implements Afina::Storage interface
    bool Put(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool PutIfAbsent(const std::string &key, const std::string &value) override; 

    // Implements Afina::Storage interface
    bool Set(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Delete(const std::string &key) override;

    // Implements Afina::Storage interface
    bool Get(const std::string &key, std::string &value) override;

private:
    // LRU cache node
    using lru_node = struct lru_node {
        const std::string key;
        std::string value;
        lru_node* prev;
        std::unique_ptr<lru_node> next;

        lru_node(const std::string key = "", std::string value = "",
                lru_node* prev = nullptr, lru_node* next = nullptr)
                : key(key), value(value), prev(prev), next(next){};
    };

    // Maximum number of bytes could be stored in this cache.
    // i.e all (keys+values) must be less the _max_size
    std::size_t _max_size;
    std::size_t _current_size = 0;
    std::size_t _empty_size = _max_size; // extra but usefull variable

    // Main storage of lru_nodes, elements in this list ordered descending by "freshness": in the head
    // element that wasn't used for longest time.
    //
    // List owns all nodes
    std::unique_ptr<lru_node> _lru_head{new lru_node()};
    lru_node* _lru_tail = _lru_head.get(); // quick access to tail;

    typedef std::map<std::reference_wrapper<const std::string>, std::reference_wrapper<lru_node>,
                                 std::less<std::string>> lru_map;

    // Index of nodes from list above, allows fast random access to elements by lru_node#key
    lru_map _lru_index;

    // clear memory for some data sizeof needed_size
    bool ClearMemory(const std::size_t needed_size);

    // accurately recount size of _current_size and _empty_size
    bool RecountCurrentSize(const std::size_t increment);

    // delete node by key and insert new
    bool UpdateNode(const std::string &key, const std::string &value, const lru_map::iterator elem_it);

    // move node to tail
    void MoveToTail(lru_node* found_node);
};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STORAGE_SIMPLE_LRU_H
