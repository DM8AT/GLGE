/**
 * @file OrderedMap.h
 * @author DM8AT
 * @brief Similar to unordered_map, but with a stable layout. In contrast to std::map the layout is user-defined. 
 * 
 * This file defines a class that holds named data in a fully user defined order. The RAM consumption is approximal double that of a normal
 * hash map (unordered map). Indexing cost is the indexing cost of an unordered map index and a vector indexing cost for a named index or
 * just the indexing cost for indexing using an index. 
 * 
 * In contrast to most other classes I followed a style similar to the C++ std library to not get confused. That's why I am using exceptions and 
 * camel case in user facing code. Internally, the `m_` for member is still used as the user does not need to interface with it and it is constant over
 * the project. 
 * 
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_ORDERED_MAP_
#define _GLGE_CORE_ORDERED_MAP_

//add common stuff
#include "../Common.h"

//unordered maps are required
#include <unordered_map>

//use the libraries namespace
namespace GLGE {

    /**
     * @brief define a class that holds ordered and named data
     * 
     * @tparam Key the key type. Must be hashable. 
     * @tparam Value the value type to store
     */
    template <typename Key, typename Value>
    class OrderedMap {
    public:

        /**
         * @brief store the type for an iterator
         */
        using iterator = typename std::vector<std::pair<Key, Value>>::iterator;
        /**
         * @brief store the type for a constant iterator
         */
        using const_iterator = typename std::vector<std::pair<Key, Value>>::const_iterator;

        /**
         * @brief Construct a new Ordered Map
         * 
         * @param entries a list of entries to create the ordered map from
         */
        inline OrderedMap(const std::initializer_list<std::pair<Key, Value>>& entries = {}) {
            //make enough storage space
            m_nameMap.reserve(entries.size());
            m_pairs.reserve(entries.size());
            //store all the entries
            for (auto& entry : entries) {
                //store the new entry
                push_back(entry);
            }
        }

        /**
         * @brief reserve space for a specific amount of entries
         * 
         * @param space the amount of entries to make space for
         */
        inline void reserve(size_t space) {
            //just reserve the space in both lists
            m_nameMap.reserve(space);
            m_pairs.reserve(space);
        }

        /**
         * @brief Get a value using the name
         * 
         * @param key the name to access the value
         * @return `Value&` a reference to the value
         */
        inline Value& getNamed(const Key& key) {
            //use the value from the name map to return the element from the value map
            return m_pairs[m_nameMap.find(key)->second].second;
        }

        /**
         * @brief Get a value using the name
         * 
         * @param key the name to access the value
         * @return `const Value&` a constant reference to the value
         */
        inline const Value& getNamed(const Key& key) const {
            //use the named map to index into the value and return a constant reference to the value
            return m_pairs[m_nameMap.find(key)->second].second;
        }

        /**
         * @brief Get a value using an index
         * 
         * @param idx the index to use to access the element
         * @return `Value&` the value of that element
         */
        inline Value& getIndexed(size_t idx) {
            //just return the value directly
            return m_pairs[idx].second;
        }

        /**
         * @brief Get a value using an index
         * 
         * @param idx the index to use to access the element
         * @return `const Value&` the value of that element
         */
        inline const Value& getIndexed(size_t idx) const {
            //just return the value directly
            return m_pairs[idx].second;
        }

        /**
         * @brief a wrapper function for `getNamed` to simplify quarrying
         * 
         * @param key the key to quarry the value for
         * @return `Value&` a reference to the quarried value
         */
        inline Value& operator[](const Key& key)
        {return getNamed(key);}

        /**
         * @brief a wrapper function for `getNamed` to simplify quarrying
         * 
         * @param key the key to quarry the value for
         * @return `const Value&` a constant reference to the quarried value
         */
        inline const Value& operator[](const Key& key) const
        {return getNamed(key);}

        /**
         * @brief a wrapper function for `getIndexed` to simplify quarrying
         * 
         * @param idx the index to quarry the value for
         * @return `Value&` a reference to the quarried value
         */
        inline Value& operator[](size_t idx)
        {return getIndexed(idx);}

        /**
         * @brief a wrapper function for `getIndexed` to simplify quarrying
         * 
         * @param idx the index to quarry the value for
         * @return `const Value&` a constant reference to the quarried value
         */
        inline const Value& operator[](size_t idx) const
        {return getIndexed(idx);}

        /**
         * @brief add a new entry at the end of the map
         * 
         * @param pair the pair to add to the end
         */
        inline void push_back(const std::pair<Key, Value>& pair) {
            //add the new entry mapping
            m_nameMap.insert_or_assign(pair.first, m_pairs.size());
            //store the new value entry
            m_pairs.emplace_back(pair.first, pair.second);
        }

        /**
         * @brief remove the last element from the map
         */
        inline void pop_back() {
            //remove the name of the last element from the name map
            m_nameMap.erase(m_pairs.back().first);
            //pop the pair list back
            m_pairs.pop_back();
        }

        /**
         * @brief get an iterator that points to the begin of the data
         * 
         * @return `iterator` an iterator that points to the beginning of the data
         */
        iterator begin() noexcept 
        {return m_pairs.begin();}

        /**
         * @brief get an iterator that points to the end of the data
         * 
         * @return `iterator` an iterator that points to the end of the data
         */
        iterator end() noexcept 
        {return m_pairs.end();}

        /**
         * @brief get a constant iterator that points to the begin of the data
         * 
         * @return `const_iterator` a constant iterator that points to the begin of the data
         */
        const_iterator begin() const noexcept 
        {return m_pairs.begin();}

        /**
         * @brief get a constant iterator that points to the end of the data
         * 
         * @return `const_iterator` a constant iterator that points to the end of the data
         */
        const_iterator end() const noexcept
        {return m_pairs.end();}

        /**
         * @brief get a constant iterator that points to the begin of the data
         * 
         * @return `const_iterator` a constant iterator that points to the begin of the data
         */
        const_iterator cbegin() const noexcept 
        {return m_pairs.cbegin();}

        /**
         * @brief get a constant iterator that points to the end of the data
         * 
         * @return `const_iterator` a constant iterator that points to the end of the data
         */
        const_iterator cend() const noexcept
        {return m_pairs.cend();}

        /**
         * @brief erase an element using its key
         * 
         * @param key the key of the element to erase
         */
        inline void erase(const Key& key) {
            //find the index of the element
            size_t idx = m_nameMap.find(key)->second;
            //erase using the index
            erase(idx);
        }

        /**
         * @brief erase an element using its key
         * 
         * @param key the key of the element to erase
         */
        inline void eraseNamed(const Key& key) {
            //find the index of the element
            size_t idx = m_nameMap.find(key)->second;
            //erase using the index
            erase(idx);
        }

        /**
         * @brief erase an element using an iterator
         * 
         * @param it the iterator pointing to the element to erase
         * @return `iterator` an iterator to the next element
         */
        inline iterator erase(iterator it) {
            //convert the iterator to an index
            size_t idx = static_cast<size_t>(it - m_pairs.begin());
            //erase using the index and return the resulting iterator
            erase(idx);
            return m_pairs.begin() + idx;
        }

        /**
         * @brief erase an element using an index
         * 
         * @param idx the index of the element to erase
         */
        inline void erase(size_t idx) {
            //remove the key from the name map
            m_nameMap.erase(m_pairs[idx].first);
            //erase the element from the ordered list
            m_pairs.erase(m_pairs.begin() + idx);
            //update all indices after the erased element
            for (size_t i = idx; i < m_pairs.size(); ++i) {
                m_nameMap[m_pairs[i].first] = i;
            }
        }

        /**
         * @brief add a new entry to the front of the map
         * 
         * @param pair the pair to add to the front
         */
        inline void push_front(const std::pair<Key, Value>& pair) {
            //insert the element at the front
            m_pairs.emplace(m_pairs.begin(), pair.first, pair.second);
            //update all existing indices
            for (size_t i = 0; i < m_pairs.size(); ++i) {
                m_nameMap[m_pairs[i].first] = i;
            }
        }

        /**
         * @brief remove the first element from the map
         */
        inline void pop_front() {
            //remove the key from the name map
            m_nameMap.erase(m_pairs.front().first);
            //remove the first element
            m_pairs.erase(m_pairs.begin());
            //update all remaining indices
            for (size_t i = 0; i < m_pairs.size(); ++i) {
                m_nameMap[m_pairs[i].first] = i;
            }
        }

        /**
         * @brief insert a new entry at a specific index
         * 
         * @param idx the index to insert the element at
         * @param pair the pair to insert
         */
        inline void insert(size_t idx, const std::pair<Key, Value>& pair) {
            //insert the element at the given position
            m_pairs.emplace(m_pairs.begin() + idx, pair.first, pair.second);
            //update all indices starting from the insertion point
            for (size_t i = idx; i < m_pairs.size(); ++i) {
                m_nameMap[m_pairs[i].first] = i;
            }
        }

        /**
         * @brief insert a new entry at a specific iterator position
         * 
         * @param it the iterator to insert the element at
         * @param pair the pair to insert
         * @return `iterator` an iterator to the inserted element
         */
        inline iterator insert(iterator it, const std::pair<Key, Value>& pair) {
            //insert the element and get the iterator
            iterator result = m_pairs.emplace(it, pair.first, pair.second);
            //update all indices starting from the insertion point
            for (size_t i = static_cast<size_t>(result - m_pairs.begin());
                i < m_pairs.size(); ++i) {
                m_nameMap[m_pairs[i].first] = i;
            }
            return result;
        }

        /**
         * @brief get a value using the key with bounds checking
         * 
         * @param key the key to access
         * @return `Value&` a reference to the value
         * @throws std::out_of_range if the key does not exist
         */
        inline Value& at(const Key& key) {
            auto it = m_nameMap.find(key);
            if (it == m_nameMap.end()) {
                throw std::out_of_range("OrderedMap::at(): key not found");
            }
            return m_pairs[it->second].second;
        }

        /**
         * @brief get a value using the key with bounds checking (const)
         * 
         * @param key the key to access
         * @return `const Value&` a constant reference to the value
         * @throws std::out_of_range if the key does not exist
         */
        inline const Value& at(const Key& key) const {
            auto it = m_nameMap.find(key);
            if (it == m_nameMap.end()) {
                throw std::out_of_range("OrderedMap::at(): key not found");
            }
            return m_pairs[it->second].second;
        }

        /**
         * @brief get a value using an index with bounds checking
         * 
         * @param idx the index to access
         * @return `Value&` a reference to the value
         * @throws std::out_of_range if the index is out of bounds
         */
        inline Value& at(size_t idx) {
            if (idx >= m_pairs.size()) {
                throw std::out_of_range("OrderedMap::at(): index out of range");
            }
            return m_pairs[idx].second;
        }

        /**
         * @brief get a value using an index with bounds checking (const)
         * 
         * @param idx the index to access
         * @return `const Value&` a constant reference to the value
         * @throws std::out_of_range if the index is out of bounds
         */
        inline const Value& at(size_t idx) const {
            if (idx >= m_pairs.size()) {
                throw std::out_of_range("OrderedMap::at(): index out of range");
            }
            return m_pairs[idx].second;
        }

        /**
         * @brief check if a key exists in the map
         * 
         * @param key the key to check
         * @return `true` if the key exists, `false` otherwise
         */
        inline bool contains(const Key& key) const {
            return m_nameMap.find(key) != m_nameMap.end();
        }

        /**
         * @brief get the number of elements in the map
         * 
         * @return `size_t` the number of elements
         */
        inline size_t size() const noexcept {
            return m_pairs.size();
        }

        /**
         * @brief check if the map is empty
         * 
         * @return `true` if the map has no elements, `false` otherwise
         */
        inline bool empty() const noexcept {
            return m_pairs.empty();
        }

        /**
         * @brief remove all elements from the map
         */
        inline void clear() noexcept {
            m_pairs.clear();
            m_nameMap.clear();
        }

        /**
         * @brief get the index of a given key
         * 
         * @param key the key to query
         * @return `size_t` the index of the key
         * @throws std::out_of_range if the key does not exist
         */
        inline size_t index_of(const Key& key) const {
            auto it = m_nameMap.find(key);
            if (it == m_nameMap.end()) {
                throw std::out_of_range("OrderedMap::index_of(): key not found");
            }
            return it->second;
        }

        /**
         * @brief get the key stored at a specific index
         * 
         * @param idx the index to query
         * @return `const Key&` a constant reference to the key
         * @throws std::out_of_range if the index is out of bounds
         */
        inline const Key& key_at(size_t idx) const {
            if (idx >= m_pairs.size()) {
                throw std::out_of_range("OrderedMap::key_at(): index out of range");
            }
            return m_pairs[idx].first;
        }

        /**
         * @brief get an iterator to the element at a specific index
         * 
         * @param idx the index to access
         * @return `iterator` an iterator pointing to the element
         * @throws std::out_of_range if the index is out of bounds
         */
        inline iterator iterator_at(size_t idx) {
            if (idx >= m_pairs.size()) {
                throw std::out_of_range("OrderedMap::iterator_at(): index out of range");
            }
            return m_pairs.begin() + idx;
        }

        /**
         * @brief get a constant iterator to the element at a specific index
         * 
         * @param idx the index to access
         * @return `const_iterator` a constant iterator pointing to the element
         * @throws std::out_of_range if the index is out of bounds
         */
        inline const_iterator iterator_at(size_t idx) const {
            if (idx >= m_pairs.size()) {
                throw std::out_of_range("OrderedMap::iterator_at(): index out of range");
            }
            return m_pairs.begin() + idx;
        }

        /**
         * @brief get an iterator to the element with a specific key
         * 
         * @param key the key to access
         * @return `iterator` an iterator pointing to the element
         * @throws std::out_of_range if the key does not exist
         */
        inline iterator iterator_of(const Key& key) {
            auto it = m_nameMap.find(key);
            if (it == m_nameMap.end()) {
                throw std::out_of_range("OrderedMap::iterator_of(): key not found");
            }
            return m_pairs.begin() + it->second;
        }

        /**
         * @brief get a constant iterator to the element with a specific key
         * 
         * @param key the key to access
         * @return `const_iterator` a constant iterator pointing to the element
         * @throws std::out_of_range if the key does not exist
         */
        inline const_iterator iterator_of(const Key& key) const {
            auto it = m_nameMap.find(key);
            if (it == m_nameMap.end()) {
                throw std::out_of_range("OrderedMap::iterator_of(): key not found");
            }
            return m_pairs.begin() + it->second;
        }

    protected:

        /**
         * @brief store a mapping from names to the actual indices in the ordered maps
         */
        std::unordered_map<Key, size_t> m_nameMap;
        /**
         * @brief store the values for the ordered map
         */
        std::vector<std::pair<Key, Value>> m_pairs;

    };

}

#endif