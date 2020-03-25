/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _parent(p),  _left(l), _right(r) {}

   T     _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;

};


template <class T>
class BSTree
{
   public:
      BSTree() {
         _root = new BSTreeNode<T>(T());
         _dummy = new BSTreeNode<T>(T());

         _dummy->_parent = _root;
         _root ->_right = _dummy;

         _size = 0;
      }
      ~BSTree() { clear(); delete _root; }

   class iterator 
   { 
      friend class BSTree;

      public:
      iterator(BSTreeNode<T>* n=0): _node(n){}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {}

      const T& operator* () const{return _node -> _data;}
      T& operator* () {return _node -> _data;}

      iterator& operator ++()
      {
         // empty
         if(_node->_left ==NULL && _node->_right == NULL && _node -> _parent == NULL){
         
         }
         //internal node
         else if(_node->_right != NULL){
            _node = _node ->_right;

            while(_node -> _left != NULL){
               _node = _node->_left;
            }
         }
         // external node
         else{ 
            BSTreeNode<T>* pre = _node ->_parent;
            while(pre !=  NULL && pre->_right == _node){
               _node = pre;
               pre = pre->_parent;
            }
            _node = pre;
         }

         return *(this);
      }

      iterator operator ++(int)
      {
         iterator t = *this;
         ++(*this);
         return t;
      }

      iterator& operator --()
      {
         // empty
         if(_node->_left ==NULL && _node->_right == NULL  && _node -> _parent == NULL){

         }
         //internal node
         else if(_node -> _left != NULL){
            _node = _node->_left;

            while(_node -> _right != NULL)
               _node = _node ->_right;
         }
         //external node
         else{
            BSTreeNode<T>* pre = _node ->_parent;
            while(pre !=  NULL && pre->_left == _node){
               _node = pre;
               pre = pre->_parent;
            }
            _node = pre;
         }

         return *(this);
      }

      iterator operator --(int)
      {
         iterator t = *this;
         --(*this);
         return t;
      }

      iterator& operator =(const iterator& i)
      {
         _node = i._node;
         return *(this);
      }

      bool operator !=(const iterator& i) const
      {
         return (_node !=  i._node);
      }

      bool operator ==(const iterator& i) const
      {
         return (_node ==  i._node);
      }

      private:
         BSTreeNode<T>* _node;
   };

///////////////////////////////////////////////////////////////////

   iterator begin() const 
   {
      if(empty())
         return iterator(_dummy);

      BSTreeNode<T>* t = _root;
      while(t -> _left != NULL){
         t = t -> _left;
      }
      return iterator(t);
   }

   iterator end() const 
   {
      return iterator(_dummy);
   }

   bool empty() const 
   {
      if(_size == 0)
         return true;
      return false;
   }

   void insert(const T& x) 
   {
      if(empty()){
         _root->_data = x;
         ++_size;
         return;
      }
      bool isEnd = false;

      BSTreeNode<T>* tmp = _root; 
      BSTreeNode<T>* last; 
      while(tmp != NULL){
         last = tmp;
         if(tmp->_data < x){
            if(tmp -> _right == _dummy){
               isEnd = true;
               break;
            }
            tmp = tmp -> _right;
         }
         else if(tmp->_data >= x)
         {
            // cout << "here" << endl;
            tmp = tmp -> _left;
         }

      }

      if(isEnd){
         BSTreeNode<T>* n = new BSTreeNode <T> (x, last, 0, _dummy); 
         _dummy -> _parent = n;
         last -> _right = n;
      }else{
         BSTreeNode<T>* n = new BSTreeNode <T> (x, last); 

         if(last ->_data > x)
            last ->_left = n;
         else
            last ->_right = n;
         // cout << " end" << endl;
      }
   
      ++_size;
   }

   bool erase(iterator pos) 
   {
      if(pos == end())
         return false;

      // if(pos._node->_right == _dummy){
      //    if(pos._node->_left == NULL){
      //       pos._node -> _parent -> _right = _dummy;
      //       _dummy -> _parent = pos._node->_parent;
      //       delete pos._node;
      //    }
      //    else{
      //       pos._node->_parent->_right = pos._node->_left;
      //       pos._node->_left ->_parent = pos._node->_parent;
      //       BSTreeNode<T>* hi = max(_root);
      //       hi->_right = _dummy;
      //       _dummy ->_parent  = hi;
      //    }
      //    return true;
      // }

      //case 1: no children
      if(pos._node -> _left == NULL && pos._node -> _right == NULL){
         BSTreeNode<T>* last = pos._node -> _parent; 
         
         if(last -> _left == pos._node)
            last -> _left = NULL;
         else
            last -> _right = NULL;

         pos._node -> _parent = NULL;
      }

      //case 2: 1 children
      else if(pos._node -> _left == NULL ||  pos._node -> _right == NULL){
         BSTreeNode<T>* last = pos._node -> _parent; 
         if(last != NULL){

            if(last -> _left == pos._node){
               if(pos._node -> _left != NULL){
                  last -> _left = pos._node->_left;
                  pos._node->_left->_parent = last;
               }
               else{
                  last->_left = pos._node->_right;   
                  pos._node->_right->_parent = last;
               }
            }
            else{
               if(pos._node -> _left != NULL){
                  last -> _right = pos._node->_left;
                  pos._node->_left->_parent = last;
               }
               else{
                  if(pos._node->_right != _dummy){
                     last->_right = pos._node->_right;      
                     pos._node->_right->_parent = last;
                  }else{
                     last->_right = _dummy;
                     _dummy -> _parent = last;
                  }
               }      
            }
            
            if(pos._node->_right == _dummy){
               BSTreeNode<T>* a = max(last);
               _dummy->_parent = a;
               a->_right = _dummy;
            }
            pos._node -> _parent = pos._node -> _right = pos._node ->_left =  NULL;

         }else{
            if(pos._node -> _right == _dummy){
               delete _root;
               _root = NULL;
               _dummy ->_parent = NULL;
            }else{
               _root = _root ->_right;
               _root -> _parent = NULL;
               delete pos._node;
            }
         }
      }

      //case 3: 2 children
      else 
      // (pos._node->_left != NULL && pos._node -> _right != NULL)
      {
         // cout<<"fuck1"<<endl;
         
         // cout<< pos._node->_right->_data << endl;
         if(pos._node->_right == _dummy)
         {
            iterator tmp(pos);
            --tmp;

            // cout<<*tmp<<endl;

            tmp._node->_right = _dummy;
            _dummy->_parent = tmp._node;

            if(pos._node == _root)
            {
               _root = _root->_left;
               _root ->_parent = NULL;
               delete pos._node;
            }
            else
            {
                           // cout << "here" << endl
               pos._node -> _parent  -> _right = pos._node -> _left;
               pos._node ->_left -> _parent = pos._node -> _parent;
               delete pos._node;
            }
         }
         else
         {
            // cout<<"fuck"<<endl;
            iterator next(pos);
            ++next;
            *pos = *next;
            erase(next);
         }

      }

      --_size;
      return true;
}

   bool erase(const T& x) 
   {
      iterator t = find(x);
      if(t == end())
         return false;

      erase(t);
      return true;
   }

   size_t size() const 
   {  
      return _size;
   }

   iterator find(const T& x) 
   {
      if(empty())
         return end();

      BSTreeNode<T>* tmp = _root; 
      BSTreeNode<T>* last; 
      while(tmp != NULL){
         last = tmp;
         if(tmp -> _data == x)
            return iterator(tmp);
         if(tmp->_data < x)
            tmp = tmp -> _right;
         else
            tmp = tmp -> _left;
      }
      return end();
   }

   void clear() 
   {
      size_t a = _size;
      for(size_t i = 0; i < a; ++i){
         pop_front();
      }

   }

   void pop_front() 
   {
      erase(begin());
   }

   void pop_back() 
   {
      iterator t(end());
      erase(--t);
   }

   void sort()
   { 
      return;
   }

   void print() const
   { 
      return;
   }

   private:
      BSTreeNode<T>* _root;
      BSTreeNode<T>* _dummy;
      size_t _size = 0;

      // //Find the next node
      // BSTreeNode<T>* succ(BSTreeNode<T>* tmp) {
      //    if(tmp->_right != NULL)
      //       return min(tmp->_right);

      //    BSTreeNode<T>* pre = tmp->_parent;
      //    while(pre !=  NULL && pre->_right == tmp){
      //       tmp = pre;
      //       pre = pre->_parent;
      //    }
      //    if(pre == NULL)
      //       return tmp;
      //    return pre;
      // }

      // //Find the leftmost node
      // BSTreeNode<T>* min(BSTreeNode<T>* tmp){
      //    if(tmp->_left == NULL)
      //       return tmp;
      //    else
      //       return min(tmp->_left);
      // }

      // //Find the last node
      // BSTreeNode<T>* presucc(BSTreeNode<T>* tmp){
      //    if(tmp->_left != NULL)
      //       return max(tmp->_left);

      //    BSTreeNode<T>* pre = tmp->_parent;
      //    while(pre != NULL && pre->_left == tmp){
      //       tmp = pre;
      //       pre = pre->_parent;
      //    }
      //    if(pre == NULL)
      //       return tmp;
      //    return pre;
      // }

      //Find the rightmost node
      BSTreeNode<T>* max(BSTreeNode<T>* tmp){
         while(tmp->_right != _dummy)
            tmp = tmp->_right;
         return tmp;
      }
};

#endif // BST_H
