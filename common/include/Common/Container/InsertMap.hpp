#pragma once

#include <concepts>
#include <mutex>
#include <vector>

#include "Common/Core/meta.hpp"

template<IsKey T, IsEmptyCtor _T>
class InsertMap
{
    public:
        using Pair = std::pair<T, _T>;
        using Iterator = std::vector<Pair>::iterator;
        using ConstIterator = std::vector<Pair>::const_iterator;

        InsertMap() = default;
        InsertMap(const InsertMap<T, _T> &_map);
        ~InsertMap() = default;

        InsertMap &operator=(const InsertMap<T, _T> &_map);

        [[nodiscard]] bool contains(const T &_key);
        [[nodiscard]] Iterator find(const T &_key);
        void append(const Pair &&_pair);
        template<class ...Ts>
        void emplace(const Ts &&..._data);

        [[nodiscard]] _T &at(const T &_key);
        [[nodiscard]] const _T &at(const T &_key) const;

        [[nodiscard]] _T &operator[](const T &_key);

        [[nodiscard]] constexpr size_t size() const;
        void clear();

        Iterator begin();
        Iterator end();

        ConstIterator begin() const;
        ConstIterator end() const;

    private:
        std::mutex m_mutex;

        // check for reimplementation of a hash map
        std::vector<Pair> m_map;
};

#include "Common/Container/InsertMap.inl"