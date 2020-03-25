/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {_isSorted = false;}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *_node; }
      T& operator * () { return *_node; }

      iterator& operator ++ ()  // ++i
      {_node = _node + 1; return(*this);}

      iterator operator ++ (int) // i++
      {iterator t (_node); _node = _node + 1;  return t; }

      iterator& operator -- () 
      {_node = _node - 1; return (*this); }

      iterator operator -- (int) 
      {iterator t(_node); _node = _node - 1;  return t; }

      iterator operator + (int i) const 
      {return iterator(_node + i); }

      iterator& operator += (int i) 
      {_node = _node + i; return (*this); }

      iterator& operator = (const iterator& i) 
      {_node = i._node; return (*this); }

      bool operator != (const iterator& i) const 
      { return (_node != i._node); }

      bool operator == (const iterator& i) const 
      { return (_node == i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const 
   { 
      if(_capacity == 0)
         return 0;
      return iterator(& _data[0]); 
   }

   iterator end() const 
   { 
      if(_capacity == 0)
         return 0;
      return iterator(& _data[_size - 1] + 1); 
   }

   bool empty() const 
   { 
      if(_size == 0)
         return true;
      return false; 
   }
   
   size_t size() const 
   { 
      return _size; 
   }

   T& operator [] (size_t i) 
   { 
      if(_capacity == 0)
         return 0;
      return &_data[i]; 
   }

   const T& operator [] (size_t i) const 
   { 
      if(_capacity == 0)
         return 0;
      return &_data[i]; 
   }

   void push_back(const T& x) 
   { 
      if(_capacity == _size)
         expand();
      
      _data[_size] = x;
      ++_size;
      _isSorted = false;
   }

   void pop_front() 
   { 
      if(empty())
         return;
      _data[0] = _data[_size - 1];
      --_size;
   }
   
   void pop_back() 
   { 
      if(empty())   return;
      --_size;
   }

   bool erase(iterator pos) 
   { 
      if(empty())
         return false; 

      *pos = _data[_size - 1];
      
      _isSorted = false;
      --_size;
      return true;
   }

   bool erase(const T& x) 
   { 
      iterator t = find(x);
      if(t == end()) return false;

      erase(t);
      return true; 
   }

   iterator find(const T& x) 
   { 
      iterator t = begin();
      while(t != end()){
         if(*t == x)
            return t;
         ++t;
      }
      return end(); 
   }

   void clear() 
   { 
      // iterator tmp = begin();
      // while(tmp != end()){
      //    erase(tmp);
      //    ++ tmp;
      // }
      _size = 0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions

   void expand()
   {
      if(_capacity == 0)
         _capacity = 1;
      else
         _capacity = 2*_capacity;

         T* tmp = new T[_capacity];
         if(_data == NULL){
            _data = tmp;
         }
         else{
            for(size_t i =0; i < _size; ++i){
               tmp[i] = _data[i];
            }
            T* fxxk = _data;

            _data = tmp;
            delete[] fxxk;
         }

   }
};

#endif // ARRAY_H
