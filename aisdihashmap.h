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

/// Default keys' comparing function for AISDIHashMap - it uses
/// operator== by default.
/// @returns 0 if keys are equal, non-zero otherwise.
/// @param key1 First key to compare. 
/// @param key2 Second key to compare. 
template <class Key>   
inline int _compFunc(const Key& key1,const Key& key2)
{
   return (key1==key2);
};

/// A map with a similar interface to std::map.
template<class K, class V,
         unsigned hashFunc(const K&),
         int compFunc(const K&,const K&)=&_compFunc<K> >
class AISDIHashMap
{
public:
   typedef K key_type;
   typedef V value_type;
   typedef unsigned size_type;

   AISDIHashMap();
   ~AISDIHashMap();

   /// Coping constructor.
   explicit AISDIHashMap(const AISDIHashMap<K, V, hashFunc, compFunc>& a);

   /// const_iterator.
   class const_iterator{};
   /// iterator.
   class iterator{};

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
   V& operator[](const K& k);   

   /// Tests if a map is empty.
   bool empty( ) const;

   /// Returns the number of elements in the map.
   size_type size() const;

   /// Returns the number of elements in a map whose key matches a parameter-specified key.
   size_type count(const K& _Key) const;

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
   void clear( );
};
