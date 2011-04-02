// vim: set fdm=marker :
/**
@file aisdihashmap.h

AISDIHashMap and related functions interface.

@author
Pawel Cichocki

@date
last revision
- 2006.03.24 Pawel Cichocki: wrote it

COPYRIGHT:
Copyright (c) 2006 Instytut Informatyki, Politechnika Warszawska
ALL RIGHTS RESERVED
*******************************************************************************/

#include <utility>
#include <iostream>
#include <iterator>
#include <string>
#include <assert.h>

#define TABSIZE 64000

/// Default keys' comparing function for AISDIHashMap - it uses
/// operator== by default.
/// @returns 0 if keys are equal, non-zero otherwise.
/// @param key1 First key to compare.
/// @param key2 Second key to compare.
//{{{ porownanie
template <class Key>
inline int _compFunc(const Key& key1,const Key& key2)
{
    return (key1==key2);
};//}}}
//{{{ funkcja hashujaca
template <class Key>
inline unsigned hashF(const Key& k) {
    int hash=0;
    for(int i=0; i<k.length(); i++)
        hash = 37*hash + k[i];
    hash %= TABSIZE;
    if(hash < 0)
        hash += TABSIZE;
    return hash;
}//}}}
/// A map with a similar interface to std::map.
template<class K, class V,
         unsigned hashFunc(const K&),
         int compFunc(const K&,const K&)=&_compFunc<K> >
class AISDIHashMap
{
public:
//{{{ definicje
    typedef K key_type;
    typedef V value_type;
    typedef std::pair<K,V> data_type;
    typedef unsigned size_type;
//}}}
//{{{ struct ListNode
    class ListNode
    {
        typedef std::pair<K,V> T;
        ListNode* next;   ///< WskaŸnik na kolejny element na liœcie/pierœcieniu
        ListNode* prev;   ///< WskaŸnik na poprzedni element nal liœcie/pierœcieniu
        T data;           ///< Dane
//	   void* internalDataPointer;   ///< wskaŸnik pomocniczy
        ListNode(const std::pair<K,V>& d) : data(d)
        {
            next = this;
            prev = this;
        }
        ListNode(const std::pair<K,V>& d, ListNode* n, ListNode* p) : next(n), prev(p), data(d)
        {
            next->prev = this;
            prev->next = this;
        }
        ~ListNode() {
            next->prev = prev;
            prev->next = next;
        }
    };
//}}}
//{{{ klasa lista
    class Lista
    {
    protected:
        ListNode *first;
    public:
        Lista() {
            first = new ListNode(std::make_pair<K,V>());
        }
        Lista(const Lista &org) {
            ListNode *temp;
            node = new ListNode(std::make_pair<K,V>());
            temp = org.first->next;
            while(temp != org.first)
            {
                new ListNode(temp->data,first,first->prev);
                temp = temp->next;
            }
        }
        ~Lista() {
            clear();
            delete first;
        }
        ListNode *insert(const data_type &ins) {
            ListNode *temp;
            temp = node->next;
            while(temp != node) {
                if(_compFunc(temp->data.first,ins.first)) {
                    return temp;
                }
            }
            unsafe_insert(ins);
            return node->prev;
        }
        ListNode *unsafe_insert(const data_type &ins) {
            new ListNode(ins,node,node->prev);
        }
//{{{ const iterator dla listy
        class const_iterator : public std::iterator<std::forward_iterator_tag,
            std::pair<K, V> >
        {
        public:
            typedef std::pair<K, V> T;

        protected:
            /// Points to the list element
            ListNode* node;
            friend class Lista;

            const_iterator(ListNode* x) : node(x) {}
        public:
            const_iterator() {}
            const_iterator(const const_iterator& a) : node(a.node) {}

            inline const T& operator*() const
            {
                return node->data;
            }

            inline const T* operator->() const
            {
                return &(node->data);
            }

            // preincrementacja
            /*inline*/
            const_iterator& operator++()
            {
                node = node->next;
                return *this;
            }
            // postincrementacja
            /*inline*/
            const_iterator operator++(int)
            {
                iterator tmp = *this;
                node = node->next;
                return tmp;
            }
            // predekrementacja
            /*inline*/
            const_iterator& operator--()
            {
                node = node->prev;
                return *this;
            }
            // postdekrementacja
            /*inline*/
            const_iterator operator--(int)
            {
                iterator tmp = *this;
                node = node->prev;
                return tmp;
            }

            inline bool operator==(const const_iterator& a) const
            {
                return node == a.node;
            }

            inline bool operator!=(const const_iterator& a) const
            {
                return node != a.node;
            }
        };
//}}}
//{{{ iterator dla listy
        class iterator : public const_iterator
        {
            iterator(ListNode* x) : const_iterator(x) {}
            friend class Lista;

        public:
            typedef std::pair<K, V> T;
            iterator() {}
            iterator(const const_iterator& a) : const_iterator(a) {}
            iterator(const iterator& a) {
                node = a.node;
            }

            inline T& operator*() const
            {
                return node->data;
            }
            inline T* operator->() const
            {
                return &(node->data);
            }

            iterator& operator++()
            {   // preincrementacja
                //++(*(const_iterator*)this);
                //return (*this);
                node = node->next;
                return *this;
            }

            iterator operator++(int)
            {   // postincrementacja
                //iterator temp = *this;
                //++*this;
                //return temp;
                iterator tmp = *this;
                node = node->next;
                return tmp;
            }

            iterator& operator--()
            {   // predekrementacja
                //--(*(const_iterator*)this);
                //return (*this);
                node = node->prev;
                return *this;
            }

            iterator operator--(int)
            {   // postdekrementacja
                //iterator temp = *this;
                //--*this;
                //return temp;
                iterator tmp = *this;
                node = node->prev;
                return tmp;
            }
        };
//}}}

    };
//}}}
private:
//{{{ zmienne
    Lista *tab[TABSIZE];
    size_type tabsize;
//}}}
public:
//{{{ metody tabliy
    AISDIHashMap() {
        int i = 0;
        for(; i<TABSIZE; i++) {
            tab[i] = new Lista();
        }
        tabsize = 0;
    }
    AISDIHashMap(const AISDIHashMap &org) {
        int i = 0;
        tabsize = org.tabsize;
        for(; i<TABSIZE; i++) {
            tab[i] = new Lista(org.tab[i]);
        }
    }
    ~AISDIHashMap() {
        int i = 0;
        for(; i<TABSIZE; i++)
            delete tab[i];
    }

    /// Coping constructor.
    explicit AISDIHashMap(const AISDIHashMap<K, V, hashFunc, compFunc>& a);

    /// const_iterator.
    //class const_iterator {};
    /// iterator.
    //class iterator {};
	
//}}}
//{{{ iterator dla tablicy
    class iterator {

    };
//}}}
//{{{ const iterator dla tablicy
    class const_iterator {

    };
//}}}
//{{{ metody tablicy cd.
    /// Returns an iterator addressing the first element in the map.
    inline iterator begin();
    inline const_iterator begin();

    /// Returns an iterator that addresses the location succeeding the last element in a map.
    inline iterator end();
    inline const_iterator end() const;

    /// Inserts an element into the map.
    /// @returns A pair whose bool component is true if an insertion was
    ///          made and false if the map already contained an element
    ///          associated with that key, and whose iterator component coresponds to
    ///          the address where a new element was inserted or where the element
    ///          was already located.
    std::pair<iterator, bool> insert(const std::pair<K, V>& entry);

    /// Returns an iterator addressing the location of the entry in the map
    /// that has a key equivalent to the specified one or the location succeeding the
    /// last element in the map if there is no match for the key.
    iterator find(const K& k);
    const_iterator find(const K& k) const;

    /// Inserts an element into a map with a specified key value
    /// if one with such a key value does not exist.
    /// @returns Reference to the value component of the element defined by the key.
    V& operator[](const K& k) {
        size_type hash = hashFunc(k);
        assert(0);
        //return (tab[hash][k])->second;
    }

    /// Tests if a map is empty.
    bool empty( ) const {
        return (tabsize == 0);
    }

    /// Returns the number of elements in the map.
    size_type size() const {
        return tabsize;
    }

    /// Returns the number of elements in a map whose key matches a parameter-specified key.
    size_type count(const K& _Key) const
    {
        assert(0);
        return 0;
    }

    /// Removes an element from the map.
    /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator i);

    /// Removes a range of elements from the map.
    /// The range is defined by the key values of the first and last iterators
    /// first is the first element removed and last is the element just beyond the last elemnt removed.
    /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator first, iterator last);

    /// Removes an element from the map.
    /// @returns The number of elements that have been removed from the map.
    ///          Since this is not a multimap itshould be 1 or 0.
    size_type erase(const K& key);

    /// Erases all the elements of a map.
    void clear( ) {
        int i=0;
        for(; i<TABSIZE; i++)
            tab[i]->clear();
    }
//}}}
};
