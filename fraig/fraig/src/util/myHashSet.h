/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* bk, size_t bksize, size_t bkidx, size_t idx): _bk(bk), _bksize(bksize),_bkidx(bkidx),_idx(idx){ _node =& _bk[_bkidx][_idx];}
      iterator(const iterator& i):_bk(i._bk), _bksize(i._bksize),_bkidx(i._bkidx),_node(i._node),_idx(i._idx){}
      ~iterator(){}

      const Data& operator * () const {return *(_node); }
      Data& operator * () {return *_node; }

      iterator& operator ++ () 
      { 
         if(_bk[_bkidx].size() != 0 && _idx +1 < _bk[_bkidx].size()){
               _node =& _bk[_bkidx][++_idx];
               return(*this);
         }
         else{
               _idx = 0;
               ++_bkidx;
               while(_bkidx < _bksize && _bk[_bkidx].empty()){
                  ++_bkidx;
               }
         }
         _node = &_bk[_bkidx][_idx];
         return (*this); 
      }

      iterator operator ++(int){
         iterator t = *this;
         ++(*this);
         return t;
      }

      iterator& operator --(){
         if(_bk[_bkidx].size() != 0 && _idx > 0){
            *_node = _bk[_bkidx][--_idx];
            return *this;
         }
         else{
            while(_bkidx  > 0){
               --_bkidx;
               if(!_bk[_bkidx].empty())
                  break;
            }
            _idx = _bk[_bkidx].size();
         }
         *_node = _bk[_bkidx][_idx];
         return *this;
      }

      iterator operator --(int){
         iterator t = *this;
         --(*this);
         return t;
      }

      iterator& operator =(const iterator& i){
         _bkidx = i._bkidx;
         _node = i._node;
         _idx = i._idx;
         return *(this);
      }
      
      iterator& operator == (const iterator& i){
         return (_node == i._node);
      }

      bool operator != (const iterator& i) const { 
         return (_node != i._node); 
      }

   private:
      vector<Data>*     _bk;
      size_t   _bkidx;

      size_t   _idx;
      Data*    _node;

      size_t   _bksize;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      for(int i = 0; i < _numBuckets; ++i){
         if(!_buckets[i].empty())
            return iterator(_buckets, _numBuckets, i, 0);
      }
      return end();
   }

   // Pass the end
   iterator end() const {
      return iterator(_buckets, _numBuckets, _numBuckets, 0);  // hash.end():  after last node
   }

   // return true if no valid data
   bool empty() const {
      if(this.size() != 0) return true;
      return false;
   }

   // number of valid data
   size_t size() const {
      size_t s = 0; 
      for(int i = 0; i<_numBuckets; ++i ){
         s = s + _buckets[i].size();
      }
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t key = bucketNum(d);
      for(int i = 0; i < _buckets[key].size(); ++i){
         if(d == _buckets[key][i])
            return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t key = bucketNum(d);
      for(int i = 0; i < _buckets[key].size(); ++i){
         if(d == _buckets[key][i]){
            d = _buckets[key][i];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t key = bucketNum(d);
      for(int i = 0; i < _buckets[key].size(); ++i){
         if(d == _buckets[key][i]){
            _buckets[key][i] = d;
            return true;
         }
      }
      _buckets[key].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      bool exist = check(d);
      if(exist)   return false;

      size_t key = bucketNum(d);
      _buckets[key].push_back(d);
      // cout << "key = " << key << endl;
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      size_t key = bucketNum(d);
      for(int i = 0; i < _buckets[key].size(); ++i){
         if(d == _buckets[key][i]){
            _buckets[key][i] = _buckets[key][_buckets[key].size() - 1];
            _buckets[key].pop_back();
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
