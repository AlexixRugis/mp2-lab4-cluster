#pragma once
#include <initializer_list>
#include <stdexcept>

template<typename T>
class linked_list
{
private:
    struct node
    {
        T data{};
        node* prev;
        node* next;
    };

    node* mFirst;
    node* mLast;
    size_t mSize;

public:
    struct iterator
    {
    private:
        node* mCurrent;
        node* mPrevious;

        iterator() noexcept : mCurrent(nullptr) {}
        iterator(node* prev, node* cur) noexcept : mCurrent(cur), mPrevious(prev) {}
    public:
        iterator& operator++()
        {
            if (mCurrent == nullptr)
            {
                throw std::out_of_range(__FUNCTION__ ": can't increment end() iterator.");
            }

            mPrevious = mCurrent;
            mCurrent = mCurrent->next;
            return *this;
        }

        iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--()
        {
            if (mPrevious == nullptr)
            {
                throw std::out_of_range(__FUNCTION__ ": can't decrement iterator.");
            }

            mCurrent = mPrevious;
            mPrevious = mCurrent->prev;
            return *this;
        }

        iterator operator--(int)
        {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator& other) const noexcept
        {
            return mCurrent == other.mCurrent;
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        const T& operator*() const
        {
            if (mCurrent == nullptr)
            {
                throw std::out_of_range(__FUNCTION__ ": can't dereference end() iterator.");
            }

            return mCurrent->data;
        }

        friend class linked_list;
    };

    linked_list() : mSize(0), mFirst(nullptr), mLast(nullptr) {}
    linked_list(const std::initializer_list<T>& elems) : mSize(0), mFirst(nullptr), mLast(nullptr)
    {
        for (const auto& elem : elems)
        {
            push_back(elem);
        }
    }

    linked_list(const linked_list& other) : mSize(0), mFirst(nullptr), mLast(nullptr)
    {
        for (const auto& elem : other)
        {
            push_back(elem);
        }
    }

    linked_list(linked_list&& other) noexcept : mSize(other.mSize), mFirst(other.mFirst), mLast(other.mLast)
    {
        other.mSize = 0;
        other.mFirst = nullptr;
        other.mLast = nullptr;
    }

    ~linked_list()
    {
        clear();
    }

    linked_list& operator=(const linked_list& other)
    {
        if (mFirst == other.mFirst)
        {
            return *this;
        }

        clear();
        for (const auto& elem : other)
        {
            push_back(elem);
        }
    }

    linked_list& operator=(linked_list&& other)
    {
        if (mFirst == other.mFirst)
        {
            return *this;
        }

        clear();
        mFirst = other.mFirst;
        mLast = other.mLast;
        mSize = other.mSize;
        other.mFirst = nullptr;
        other.mLast = nullptr;
        other.mSize = 0;
    }

    size_t size() const noexcept { return mSize; }
    bool is_empty() const noexcept { return size() == 0; }

    void push_front(const T& elem)
    {
        node* nw = new node{ elem, nullptr, mFirst };
        mFirst = nw;
        if (mLast == nullptr)
        {
            mLast = nw;
        }

        ++mSize;
    }

    void push_back(const T& elem)
    {
        node* nw = new node{ elem, mLast, nullptr };
        if (mLast != nullptr)
        {
            mLast->next = nw;
        }
        mLast = nw;
        if (mFirst == nullptr)
        {
            mFirst = nw;
        }

        ++mSize;
    }

    void pop_front()
    {
        if (is_empty())
        {
            throw std::out_of_range(__FUNCTION__ ": can't pop front element of empty list.");
        }

        if (mFirst == mLast)
        {
            delete mFirst;
            mFirst = nullptr;
            mLast = nullptr;
        } else
        {
            node* tmp = mFirst;
            mFirst = mFirst->next;
            if (mFirst != nullptr)
            {
                mFirst->prev = nullptr;
            }
            delete tmp;
        }

        --mSize;
    }

    void pop_back()
    {
        if (is_empty())
        {
            throw std::out_of_range(__FUNCTION__ ": can't pop back element from empty list.");
        }

        if (mLast == mFirst)
        {
            delete mLast;
            mFirst = nullptr;
            mLast = nullptr;
        } else
        {
            node* tmp = mLast;
            mLast = mLast->prev;
            mLast->next = nullptr;
            delete tmp;
        }

        --mSize;
    }

    void clear()
    {
        while (!is_empty())
        {
            pop_front();
        }
    }

    const T& back()
    {
        if (is_empty())
        {
            throw std::out_of_range(__FUNCTION__ ": can't get back from empty list.");
        }

        return mLast->data;
    }

    const T& front()
    {
        if (is_empty())
        {
            throw std::out_of_range(__FUNCTION__ ": can't get front from empty list.");
        }

        return mFirst->data;
    }

    iterator insert(const iterator& pos, const T& data)
    {
        if (pos.mCurrent != nullptr)
        {
            node* nw = new node{ data, pos.mPrevious, pos.mCurrent };
            pos.mPrevious->next = nw;
            pos.mCurrent->prev = nw;

            if (pos.mCurrent == mFirst)
            {
                mFirst = nw;
            }

            ++mSize;

            return iterator(nw->prev, nw);
        } else
        {
            push_back(data);
            return iterator(mLast, mLast->prev);
        }
    }

    void erase(const iterator& pos)
    {
        if (pos.mCurrent == nullptr)
        {
            throw std::out_of_range(__FUNCTION__ ": can't erase after end of list.");
        }

        if (pos.mCurrent == mFirst)
        {
            pop_front();
        } else if (pos.mCurrent == mLast)
        {
            pop_back();
        } else
        {
            if (pos.mPrevious != nullptr)
            {
                pos.mPrevious->next = pos.mCurrent->next;
            }
            if (pos.mCurrent->next != nullptr)
            {
                pos.mCurrent->next->prev = pos.mPrevious;
            }

            --mSize;
            delete pos.mCurrent;
        }
    }

    iterator begin() const noexcept { return iterator(nullptr, mFirst); }
    iterator end() const noexcept { return iterator(mLast, nullptr); }
};