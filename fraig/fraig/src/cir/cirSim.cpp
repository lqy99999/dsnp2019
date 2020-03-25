/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
}

void
CirMgr::writelog(int count)
{
  for(unsigned i = 0; i < count; ++i){
    for(unsigned j = 0; j < aag[1]; ++j){
      *_simLog << simpattern[j][i];
    }
    *_simLog << " ";
  for(unsigned i = 0; i < dfslist.size(); ++i){
    if(dfslist[i]->getTypeStr() == "PO"){
    *_simLog << dfslist[i]->value;
    }
  }
    *_simLog << endl;
  }
}


void
CirMgr::fileSim(ifstream& patternFile)
{
  int count = 0;
  string line;
  int i=0;
  string  sval = "";
  int val = NULL;
  simpattern = new vector<size_t>[aag[1]];

  if(!patternFile){
    //ERROR
    return;
  }

  IdList allgroup;
  for(int i = 0; i < dfslist.size(); ++i){
    if(dfslist[i]->getTypeStr() == "PO" || dfslist[i]->getTypeStr() == "PI")  continue;
    allgroup.push_back(2*(dfslist[i]->id));
  }
  allgroup.push_back(glist[0]->id);
  group.insert(pair<size_t, IdList>(0, allgroup));

  do{
    patternFile >> line;
    
    if(line.size() == 0) continue;
    // cout << "line: " << i <<" " <<line << endl;
    // ++i;

    if(line.size() != aag[1]){
      cerr << "Error: Pattern(" << line << ") length(" <<line.size() << ") does not match";
      cerr << " the number of inputs(" << aag[1] << ") in a circuit!!" << endl; 
      return;
    }
    else{
      for(int i = 0; i < aag[1]; ++i){
        sval = line[i];
        if(!myStr2Int(sval,val)){
          cerr << "Error: Pattern(" << line << ") contains a non-0/1 character(\'" << sval;
          cerr << "\')." << endl;
          return;
        }
        // in[i]->ans = val;
        simpattern[i].push_back((size_t) val);
        // cout << simpattern[i].size();
      }
    }
    line = "";
    ++count;
  }while(!patternFile.eof());

  
  if(count % 64 != 0){
    for(int i = 0; i < 64-(count%64); ++i){
      for(int i = 0; i < aag[1]; ++i){
        simpattern[i].push_back((size_t) 0);
      }
    }
  }
  runsim();

  cout << endl;
  cout << "Total #FEC Group = " << fecgroup.size() << endl;
  cout << count << " patterns simulated." << endl;
  writelog(count);
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::runsim(){
  for(unsigned i = 0; i < simpattern[0].size(); ++i){
    for(unsigned j = 0; j < aag[1]; ++j){
      // cout << simpattern[j][i];
      in[j]->ans = simpattern[j][i];
    }
    runans();
//     cout << "runsim" << i << endl;
//     cout << "======= CHECK dfslist========" << endl;
//     cout << dfslist.size() << endl;
//    for(int i  = 0; i < dfslist.size(); i++){
//      if(dfslist[i]->getTypeStr() == "PO" || dfslist[i]->getTypeStr() == "PI")  continue;
//      cout << "dfslist[" << i << "] : ";
//      if(dfslist[i] != NULL){
//        cout << dfslist[i]->getid() << endl;
//        cout << "value : " << dfslist[i]->value <<endl;
//         // for(int j = 0;  j<dfslist[i]->fanin.size(); j++){
//         //   cout << "in" << j << ": " <<dfslist[i]->fanin[j]->getid() << endl;
//         // }
//         // for(int j = 0;  j<dfslist[i]->fanout.size(); j++){
//         //   cout << "out" << j << ": " << dfslist[i]->fanout[j]->getid() << endl;
//         // }
//      }
//       else
//       {
//          cout << "NULL" << endl;
//       }
//    }
//  cout << "============CHECK END=============" <<endl;
    splitgroup();
  }
  sortgroup();

}

void
CirMgr::runans(){
  for(unsigned i = 0; i < dfslist.size(); ++i){
    if(dfslist[i]->getTypeStr() == "PO" || dfslist[i]->getTypeStr() == "PI")  continue;
      dfslist[i]->ans = (dfslist[i]->fanin[0]->ans ^ dfslist[i]->n1)&(dfslist[i]->fanin[1]->ans ^ dfslist[i]->n2);
    dfslist[i]->value = (dfslist[i]->value << 1) + dfslist[i]->ans;
  }
  // for(unsigned i = 0; i < dfslist.size(); ++i){
  //   if(dfslist[i]->getTypeStr() == "PO"){
  //   dfslist[i]->ans = (dfslist[i]->fanin[0]->ans ^ dfslist[i]->n1);
  //   dfslist[i]->value = (dfslist[i]->value << 1) + dfslist[i]->ans;
  //   }
  // }
}

void
CirMgr::splitgroup(){
  unordered_map<size_t, IdList>::iterator it;
  unordered_map<size_t, IdList>::iterator it3;
  vector<unsigned>::iterator it2;

  for(it = group.begin(); it != group.end(); ){ 
    IdList* list2 = &((*it).second);
    IdList newlist;
    unordered_map<size_t, IdList> newgroup;
    for(it2 = list2->begin(); it2 != list2->end(); ){
      // cout << "*it2 : " << *it2 << endl;
      // cout << "glist[*it2]->value : " << glist[*it2]->value << endl;
      unsigned idx = (*it2 % 2 == 0)? *it2 >> 1 : (*it2 - 1) >> 1;
      if(glist[idx]->value != it->first){
        if((it3 = newgroup.find(glist[idx]->value)) != newgroup.end()){
          (it3->second).push_back(2*idx);
          // cout << "here  ";
        }else{
          // cout << (glist[idx]->value) << "\t";
          size_t test = (18446744073709551616-(~(glist[idx]->value)));
          // cout << "===" << endl;
          // cout << idx << endl;
          // cout << test << endl;
          if((it3 = newgroup.find(test)) != newgroup.end()){
            (it3->second).push_back(2*idx + 1);
          }
          newlist.push_back(2*idx);
          // cout << "push_back: " << *it2 << endl;
          newgroup.insert(pair<size_t, IdList>(glist[idx]->value, newlist));
          newlist.clear();
        }
        it2 = list2->erase(it2);
      }
      else it2++;
    }
    
    group.insert(newgroup.begin(), newgroup.end());
      if(list2->empty()){
        it = group.erase(it);
      }
      else it++;
    newgroup.clear();
    // for(int i = 0; i < newlist.size(); ++i){
    //   cout << "~~newlist~~" << endl;
    //   cout <<newlist[i] << " " << endl;
    //   cout << "~~end~~" << endl;
    // } 
    

  }
//       cout << "======= CHECK group========" << endl;
//     cout << group.size() << endl;
//     unordered_map<size_t, IdList>::iterator a;
//     vector<unsigned>::iterator b;
//     int i = 0;
//    for(a = group.begin(); a != group.end(); a++){
//      cout << "group" << a->first << endl; 
//      for(b = a->second.begin(); b != a->second.end(); b++){
//        cout << *b << " ";
//       }
//     cout << endl;
//     i++;
// }
// cout << "============CHECK END=============" <<endl;
}


void 
CirMgr::sortgroup(){
  for(auto it = group.begin(); it != group.end(); ++it){
    sort(it->second.begin(), it->second.end());
    if(it->second.size() > 1)
      fecgroup.push_back(&(it->second));
  }
  sort(fecgroup.begin(), fecgroup.end());
}

string
CirMgr::reversebit(const size_t &a){
  bitset<64> b{a};
  string l = (~b).to_string();
  return l;
}
