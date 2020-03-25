/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }

      iterator& operator ++ () // ++i
      { _node = _node ->_next; return *(this); }

      iterator operator ++ (int) // i++
      { _node = _node->_next; iterator t(_node->_prev); return t; }

      iterator& operator -- () 
      { _node = _node->_prev; return *(this); }

      iterator operator -- (int) 
      {  _node = _node->_prev; iterator t(_node->_next); return t; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (i._node != _node); }
      bool operator == (const iterator& i) const { return (!(i != _node)); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { iterator t(_head); return t; }
   iterator end() const { iterator t(_head->_prev); return t; }
   bool empty() const 
   { 
      if(_head == _head->_next)   return true; 
      else return false;
   }
   
   size_t size() const 
   {  
      size_t count = 0;
      iterator t(_head);
      iterator e = end();
      while(t != e){
         ++count;
         ++t;
      }
      return count; 
   }

   void push_back(const T& x) 
   { 
      if(!empty()){
         iterator t(_head->_prev); // dummy
         DListNode<T> * tmp = new DListNode<T> (x, t._node->_prev, t._node);
         (t._node->_prev) ->_next = tmp;
         t._node ->_prev = tmp;
      }
      else{
         DListNode<T> * tmp = new DListNode<T> (x, _head, _head); // _head = dummy
         _head->_next = tmp;
         _head->_prev = tmp;
         _head = tmp;
      }
      _isSorted = false;
   }

   void pop_front() 
   { 
      if(empty()) return;
      else if (size() == 1){
         DListNode<T> * tmp = _head;
         _head = _head->_prev;
         _head->_next = _head;
         _head->_prev = _head;
         delete tmp;
         return;
      }
      else{
         DListNode<T> * tmp = _head;
         _head = _head->_next;
         _head->_prev = tmp->_prev;
         (tmp->_prev)->_next = _head;
         delete tmp;
         return;
      }
   }

   void pop_back() 
   { 
      if(empty()) return;
      else if(size() == 1){
         DListNode<T> * tmp = _head;
         _head = _head->_prev;
         _head->_prev = _head;
         _head->_next = _head;
         delete tmp;
         return;
      }
      else{
         iterator t = end();
         ((t._node->_prev)->_prev)->_next = t._node;
         (t._node)->_prev = ((t._node->_prev)->_prev);
         return;
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) 
   { 
      if(size()==0)   return false; 
      else if(pos._node == _head){
         pop_front();
      }
      else if(pos._node == end()._node->_prev){
         pop_back();
      }
      else{
         iterator pre = --pos;
         iterator aft = ++(++pos);
         pre._node->_next = aft._node;
         aft._node->_prev = pre._node;
      }
      return true;
   }

   bool erase(const T& x) \
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
      iterator tmp = begin();
      while(tmp != end()){
         erase(tmp);
         ++ tmp;
      }
      _head = end()._node;
      _head->_next = _head->_prev = _head;
      _isSorted = false;
   }  // delete all nodes except for the dummy node

   void sort() const
   { 
      if(_isSorted)  return;
      iterator end(_head->_prev);

      for(iterator i =  begin(); i != end; ++i){
         for(iterator j = i; j != end; ++j){
            if(*i > *j)
               swap(i, j);
         }
      }
      _isSorted = true;
      return;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

   void swap(iterator &i, iterator &j) const{
      T tmp = *i;
      *i = *j;
      *j = tmp;
   }
};

#endif // DLIST_H
