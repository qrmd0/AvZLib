/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-06-10 11:54:47
 * @Description:
 */
#ifndef __AVZ1_ITERATOR_H__
#define __AVZ1_ITERATOR_H__

#include <iterator>

#include "avz1_memory.h"

namespace AvZ {

template <typename T>
using __PredicateT = std::function<bool(T* ptr)>;

template <class T>
struct __filter_trait;

template <>
struct __filter_trait<Plant> {
    static Plant* __get_end()
    {
        return GetMainObject()->plantArray() + GetMainObject()->plantTotal();
    }
    static Plant* __get_begin()
    {
        return GetMainObject()->plantArray();
    }
    static __PredicateT<Plant> __get_alive_predicate()
    {
        return [](Plant* ptr) -> bool { return !ptr->isDisappeared() && !ptr->isCrushed(); };
    }
};

template <>
struct __filter_trait<Zombie> {
    static Zombie* __get_end()
    {
        return GetMainObject()->zombieArray() + GetMainObject()->zombieTotal();
    }
    static Zombie* __get_begin()
    {
        return GetMainObject()->zombieArray();
    }
    static __PredicateT<Zombie> __get_alive_predicate()
    {
        return [](Zombie* ptr) -> bool { return !ptr->isDisappeared() && !ptr->isDead(); };
    }
};

template <>
struct __filter_trait<Item> {
    static Item* __get_end()
    {
        return GetMainObject()->itemArray() + GetMainObject()->itemTotal();
    }
    static Item* __get_begin()
    {
        return GetMainObject()->itemArray();
    }
    static __PredicateT<Item> __get_alive_predicate()
    {
        return [](Item* ptr) -> bool { return !ptr->isDisappeared() && !ptr->isCollected(); };
    }
};

template <>
struct __filter_trait<Seed> {
    static Seed* __get_end()
    {
        return GetMainObject()->seedArray() + GetMainObject()->seedArray()->count();
    }
    static Seed* __get_begin()
    {
        return GetMainObject()->seedArray();
    }
    static __PredicateT<Seed> __get_alive_predicate()
    {
        return [](Seed* ptr) -> bool { return ptr->isUsable(); };
    }
};

template <>
struct __filter_trait<PlaceItem> {
    static PlaceItem* __get_end()
    {
        return GetMainObject()->placeItemArray() + GetMainObject()->placeItemTotal();
    }
    static PlaceItem* __get_begin()
    {
        return GetMainObject()->placeItemArray();
    }
    static __PredicateT<PlaceItem> __get_alive_predicate()
    {
        return [](PlaceItem* ptr) -> bool { return !ptr->isDisappeared(); };
    }
};

template <typename T>
class FilterIterator {
private:
    T* __cur;
    __PredicateT<T> __pred;
    T* __end;

    void forward()
    {
        ++__cur;
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = void;
    using pointer = T*;
    using reference = value_type&;

    FilterIterator(pointer ptr, __PredicateT<T>&& func)
        : __cur(ptr)
        , __pred(std::move(func))
        , __end(__filter_trait<T>::__get_end())
    {
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

    FilterIterator(pointer ptr, const __PredicateT<T>& func)
        : __cur(ptr)
        , __pred(func)
        , __end(__filter_trait<T>::__get_end())
    {
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

    pointer toPtr() const
    {
        return __cur;
    }

    reference operator*()
    {
        return *(value_type*)(__cur);
    }

    reference operator*() const
    {
        return *(value_type*)(__cur);
    }

    pointer operator->()
    {
        return __cur;
    }

    const pointer operator->() const
    {
        return __cur;
    }

    FilterIterator<T>& operator++()
    {
        forward();
        return *this;
    }

    FilterIterator<T> operator++(int)
    {
        auto tmp = *this;
        forward();
        return tmp;
    }

    bool operator==(const FilterIterator<T>& rhs) const
    {
        return __cur == rhs.__cur;
    }

    bool operator!=(const FilterIterator<T>& rhs) const
    {
        return __cur != rhs.__cur;
    }
};

template <typename T>
class BasicFilter {
protected:
    __PredicateT<T> __pred;

public:
    using Iterator = FilterIterator<T>;

    BasicFilter()
        : __pred([](T*) { return true; })
    {
    }

    BasicFilter(__PredicateT<T>&& func)
        : __pred(std::move(func))
    {
    }

    BasicFilter(const __PredicateT<T>& func)
        : __pred(func)
    {
    }

    virtual void setPredicate(__PredicateT<T>&& func)
    {
        this->__pred = std::move(func);
    }

    virtual void setPredicate(const __PredicateT<T>& func)
    {
        this->__pred = func;
    }

    Iterator begin()
    {
        return Iterator(__filter_trait<T>::__get_begin(), this->__pred);
    }

    Iterator end()
    {
        return Iterator(__filter_trait<T>::__get_end(), this->__pred);
    }
};

template <typename T>
class AliveFilter : public BasicFilter<T> {
private:
    __PredicateT<T> __alive_pred;

public:
    AliveFilter()
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        this->__pred = this->__alive_pred;
    }

    AliveFilter(__PredicateT<T>&& func)
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        setPredicate(std::move(func));
    }

    AliveFilter(const __PredicateT<T>& func)
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        setPredicate(func);
    }

    virtual void setPredicate(__PredicateT<T>&& func) override
    {
        this->__pred = [this, func = std::move(func)](T* ptr) -> bool {
            return this->__alive_pred(ptr) && func(ptr);
        };
    }

    virtual void setPredicate(const __PredicateT<T>& func) override
    {
        this->__pred = [this, func](T* ptr) -> bool {
            return this->__alive_pred(ptr) && func(ptr);
        };
    }
};
}

#endif
