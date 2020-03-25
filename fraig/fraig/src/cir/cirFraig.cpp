/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include <unordered_map>

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{ 
  unordered_map<size_t, CirGate*> _task;
  unordered_map<size_t, CirGate*>::iterator it;
  for(int i = 0; i < dfslist.size(); ++i){
      if(dfslist[i]->isAig()){
        // cout << "dfslist[" << i << "] : ";
        // cout << dfslist[i]->getid() << " is merging" << endl;
        
        size_t key = buildkey(dfslist[i]);
        // cout << "key = " << key << endl;
        it = _task.find(key);

        if(it != _task.end()){
          mergegate(it->second, dfslist[i]);
        }
        else{
          // cout << "insert " <<dfslist[i]->getid() << endl;
          _task.insert(pair<size_t, CirGate*>(key,dfslist[i]));
        }
      } 
  }

  vector<CirGate*>:: iterator it2;
  for(it2 = aig.begin(); it2 != aig.end(); ){
    if(glist[(*it2)->getid()]->getTypeStr() =="CONST"){
      it2 = aig.erase(it2);
    }
    else  ++it2;
  }
  aag[4] = aig.size();
  dfslist.clear();
  dfs(out);
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void 
CirMgr::mergegate(CirGate*a, CirGate*b){
  cout << "Strashing: " << a->id << " merging " << b->id << "..." << endl;
  a->mergegate(b);

  glist[b->id] = NULL;
  removegate(b);
}

size_t
CirMgr::buildkey(CirGate* p){
  size_t input0 = 0;
  size_t input1 = 0;
  if(p->fanin[0] != NULL) input0 = p->fanin[0]->getid();
  if(p->fanin[1] != NULL) input1 = p->fanin[1]->getid();

  // cout  << "input0 : " << input0 << endl;
  // cout  << "input1 : " << input1 << endl;
  
  if(input0 < input1)
      return ((input1 << 5) + (input0 << 20) + (p->n1 << 1) + p->n2);
  else
      return ((input0 << 5) + (input1 << 20) + (p->n2 << 1) + p->n1);
}