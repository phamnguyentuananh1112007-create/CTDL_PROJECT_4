#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T>
struct Node
{
    T Data;
    Node<T>* Next;

    explicit Node(const T& value)
        : Data(value), Next(nullptr)
    {
    }
};

template <typename T>
class LinkedList
{
private:
    Node<T>* _pHead;
    Node<T>* _pTail;
    std::size_t _size;

    void clear() noexcept
    {
        while (_pHead != nullptr)
        {
            Node<T>* pDelete = _pHead;
            _pHead = _pHead->Next;
            delete pDelete;
        }
        _pTail = nullptr;
        _size = 0;
    }

public:
    LinkedList() noexcept
        : _pHead(nullptr), _pTail(nullptr), _size(0)
    {
    }

    LinkedList(const LinkedList& other)
        : LinkedList()
    {
        try
        {
            for (const Node<T>* pCurrent = other._pHead;
                 pCurrent != nullptr;
                 pCurrent = pCurrent->Next)
            {
                pushBack(pCurrent->Data);
            }
        }
        catch (...)
        {
            clear();
            throw;
        }
    }

    LinkedList& operator=(const LinkedList& other)
    {
        if (this != &other)
        {
            LinkedList copy(other);
            swap(copy);
        }
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept
        : _pHead(other._pHead), _pTail(other._pTail), _size(other._size)
    {
        other._pHead = nullptr;
        other._pTail = nullptr;
        other._size = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            _pHead = other._pHead;
            _pTail = other._pTail;
            _size = other._size;
            other._pHead = nullptr;
            other._pTail = nullptr;
            other._size = 0;
        }
        return *this;
    }

    ~LinkedList()
    {
        clear();
    }

    Node<T>* head() noexcept
    {
        return _pHead;
    }

    const Node<T>* head() const noexcept
    {
        return _pHead;
    }

    bool empty() const noexcept
    {
        return _pHead == nullptr;
    }

    std::size_t size() const noexcept
    {
        return _size;
    }

    T& front()
    {
        if (empty())
        {
            throw std::out_of_range("Danh sach rong.");
        }
        return _pHead->Data;
    }

    const T& front() const
    {
        if (empty())
        {
            throw std::out_of_range("Danh sach rong.");
        }
        return _pHead->Data;
    }

    void pushBack(const T& value)
    {
        std::unique_ptr<Node<T> > pNewNode(new Node<T>(value));
        if (_pTail == nullptr)
        {
            _pHead = pNewNode.get();
        }
        else
        {
            _pTail->Next = pNewNode.get();
        }
        _pTail = pNewNode.release();
        ++_size;
    }

    bool popFront()
    {
        if (empty())
        {
            return false;
        }

        Node<T>* pDelete = _pHead;
        _pHead = _pHead->Next;
        if (_pHead == nullptr)
        {
            _pTail = nullptr;
        }
        delete pDelete;
        --_size;
        return true;
    }

    template <typename Predicate>
    bool removeIf(Predicate predicate)
    {
        Node<T>* pPrevious = nullptr;
        Node<T>* pCurrent = _pHead;
        while (pCurrent != nullptr)
        {
            if (predicate(pCurrent->Data))
            {
                if (pPrevious == nullptr)
                {
                    _pHead = pCurrent->Next;
                }
                else
                {
                    pPrevious->Next = pCurrent->Next;
                }
                if (_pTail == pCurrent)
                {
                    _pTail = pPrevious;
                }
                delete pCurrent;
                --_size;
                return true;
            }
            pPrevious = pCurrent;
            pCurrent = pCurrent->Next;
        }
        return false;
    }

    void swap(LinkedList& other) noexcept
    {
        using std::swap;
        swap(_pHead, other._pHead);
        swap(_pTail, other._pTail);
        swap(_size, other._size);
    }
};
