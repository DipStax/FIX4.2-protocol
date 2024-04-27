#include <algorithm>

#include "Common/Container/InsertMap.hpp"

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::InsertMap(const InsertMap<T, _T> &_map)
    : m_mutex(), m_map(_map.m_map)
{
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T> &InsertMap<T, _T>::operator=(const InsertMap<T, _T> &_map)
{
    m_map = _map.m_map;
    return *this;
}

template<IsKey T, IsEmptyCtor _T>
bool InsertMap<T, _T>::contains(const T &_key)
{
    return find(_key) != m_map.end();
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::Iterator InsertMap<T, _T>::find(const T &_key)
{
    // std::lock_guard<std::mutex> guard(m_mutex);

    return std::find_if(m_map.begin(), m_map.end(), [_key] (const Pair &_mkey) {
        return _key == _mkey.first;
    });
}

template<IsKey T, IsEmptyCtor _T>
void InsertMap<T, _T>::append(const Pair &&_pair)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    m_map.emplace_back(std::move(_pair));
}

template<IsKey T, IsEmptyCtor _T>
template<class ...Ts>
void InsertMap<T, _T>::emplace(const Ts &&..._pair)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    m_map.emplace_back(std::move(_pair)...);
}

template<IsKey T, IsEmptyCtor _T>
_T &InsertMap<T, _T>::at(const T &_key)
{
    return find(_key)->second;
}

template<IsKey T, IsEmptyCtor _T>
const _T &InsertMap<T, _T>::at(const T &_key) const
{
    return find(_key)->second;
}

template<IsKey T, IsEmptyCtor _T>
_T &InsertMap<T, _T>::operator[](const T &_key)
{
    Iterator it = find(_key);

    if (it == m_map.end()) {
        emplace(Pair{ _key, _T{} });
        return find(_key)->second;
    }
    return it->second;
}

template<IsKey T, IsEmptyCtor _T>
constexpr size_t InsertMap<T, _T>::size() const
{
    return m_map.size();
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::Iterator InsertMap<T, _T>::begin()
{
    return m_map.begin();
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::Iterator InsertMap<T, _T>::end()
{
    return m_map.end();
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::ConstIterator InsertMap<T, _T>::begin() const
{
    return m_map.begin();
}

template<IsKey T, IsEmptyCtor _T>
InsertMap<T, _T>::ConstIterator InsertMap<T, _T>::end() const
{
    return m_map.end();
}

template<IsKey T, IsEmptyCtor _T>
void InsertMap<T, _T>::clear()
{
    std::lock_guard<std::mutex> guard(m_mutex);

    m_map.clear();
}