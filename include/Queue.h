#pragma once

#include "Node.h"

template <typename T>
class Queue
{
private:
    LinkedList<T> _items;

public:
    bool empty() const noexcept
    {
        return _items.empty();
    }

    std::size_t size() const noexcept
    {
        return _items.size();
    }

    T& front()
    {
        return _items.front();
    }

    const T& front() const
    {
        return _items.front();
    }

    void push(const T& value)
    {
        _items.pushBack(value);
    }

    void pop()
    {
        if (!_items.popFront())
        {
            throw std::out_of_range("Hang doi rong.");
        }
    }

    void swap(Queue& other) noexcept
    {
        _items.swap(other._items);
    }
};
