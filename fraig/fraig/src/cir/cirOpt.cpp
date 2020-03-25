/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void 
CirMgr::sweep()
{
   // define but not used gate
   //dfs
    for(int i = 0 ; i < dfslist.size(); ++i){
      dfslist[i]->sw = false;
    }
    for(int i = 1; i < glist.size(); ++i){
      if(glist[i] != NULL && glist[i]->sw == true && glist[i]->getTypeStr() != "PI"){
        cout << "Sweeping: " << glist[i] -> getTypeStr() << "(" << glist[i] -> getid() << ") removed..." << endl;
        removegate(glist[i]);
        // glist[i] = NULL;
      }
    }

  //     cout << "======= CHECK glist========" << endl;
  //     for(int i  = 0; i < glist.size(); i++){
  //       cout << "glist[" << i << "] : ";
  //       if(glist[i] != NULL){
  //         cout << glist[i]->getid() << endl;
  //         for(int j = 0;  j<glist[i]->fanin.size(); j++){
  //           cout << "in" << j << ": " << glist[i]->fanin[j]->getid() << endl;
  //         }
  //         for(int j = 0;  j<glist[i]->fanout.size(); j++){
  //           cout << "out" << j << ": " << glist[i]->fanout[j]->getid() << endl;
  //         }
  //       }
  //        else
  //        {
  //           cout << "NULL" << endl;
  //        }
  //     }
  // cout << "============CHECK END=============" <<endl;


  //  //deal with null fanout
  //  for(int i = 0; i < glist.size(); ++i){
  //    if(glist[i] != NULL){
  //      vector<CirGate*>:: iterator it;
  //       for(it = glist[i]->fanout.begin(); it != glist[i]->fanout.end();){
  //           // cout << "i = " << i << " i->id = " << (*it)->getid() << endl;
  //           if(glist[(*it)->getid()] == NULL){ 
  //             it = glist[i]->fanout.erase(it);
  //           }
  //           else ++it;
  //       }
  //    }
  //  }
   
    //delete gate in aig
    vector<CirGate*>:: iterator it;
    for(it = aig.begin(); it != aig.end(); ){
      if(glist[(*it)->getid()]->getTypeStr() =="CONST"){
        it = aig.erase(it);
      }
      else  ++it;
    }
   aag[4] = aig.size();

  //     cout << "======= CHECK aig========" << endl;
  //     for(int i  = 0; i < aig.size(); i++){
  //       cout << "aig[" << i << "] : ";
  //       if(aig[i] != NULL){
  //         cout << aig[i]->getid() << endl;
  //         for(int j = 0;  j<aig[i]->fanin.size(); j++){
  //           cout << "in" << j << ": " << aig[i]->fanin[j]->getid() << endl;
  //         }
  //         for(int j = 0;  j<aig[i]->fanout.size(); j++){
  //           cout << "out" << j << ": " << aig[i]->fanout[j]->getid() << endl;
  //         }
  //       }
  //        else
  //        {
  //           cout << "NULL" << endl;
  //        }
  //     }
  // cout << "============CHECK END=============" <<endl;

  //     cout << "======= CHECK aig========" << endl;
  //     for(int i  = 0; i < aig.size(); i++){
  //       cout << "aig[" << i << "] : ";
  //       if(aig[i] != NULL){
  //         cout << aig[i]->getid() << endl;
  //         for(int j = 0;  j<aig[i]->fanin.size(); j++){
  //           cout << "in" << j << ": " << aig[i]->fanin[j]->getid() << endl;
  //         }
  //         for(int j = 0;  j<aig[i]->fanout.size(); j++){
  //           cout << "out" << j << ": " << aig[i]->fanout[j]->getid() << endl;
  //         }
  //       }
  //        else
  //        {
  //           cout << "NULL" << endl;
  //        }
  //     }

}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  IdList getout;
  if(sim){
    cerr << "Error: circuit has been simulated!! Do \"CIRFraig\" first!!" << endl;
  }
  else{
    for(int i = 0; i < dfslist.size(); ++i){
      CirGate* p = dfslist[i];
      if(p->getTypeStr()=="PI" || p->getTypeStr()=="PO" || p->getTypeStr()=="CONST")
        continue;

      //const0 and x
      // cout << "now node id = " << p->id << endl;
      if(p->fanin[0] != NULL && p->fanin[0]->getTypeStr() == "CONST"){
        p->gettogether(p->n1, true);
        getout.push_back(p->id);
        
      }
      if(p->fanin[1] != NULL && p->fanin[1]->getTypeStr() == "CONST"){
         p->gettogether(p->n2, false);
         getout.push_back(p->id);
      }
      if(p->fanin[0] != NULL && p->fanin[1] != NULL && p->fanin[0] == p->fanin[1]){
        if(p->n1 == p->n2)  p->getsame(p->n1);
        else p->getconst(glist[0]);
        getout.push_back(p->id);
      }

      // removegate(p);
      for(int i = 0; i < getout.size(); ++i){
        CirGate* p = glist[getout[i]];
        if(p != NULL){
          glist[getout[i]] = NULL;
          delete p;
        }
      }
    }
    
    //delete gate in aig
    vector<CirGate*>:: iterator it;
    for(it = aig.begin(); it != aig.end(); ){
      if(glist[(*it)->getid()]->getTypeStr() =="CONST"){
        it = aig.erase(it);
      }
      else  ++it;
    }
   aag[4] = aig.size();
//             cout << "======= CHECK getout========" << endl;
//     cout << getout.size() << endl;
//    for(int i  = 0; i < getout.size(); i++){
//      cout << "getout[" << i << "] : ";
//      if(glist[getout[i]] != NULL){
//        cout << glist[getout[i]]->getid() << endl;
//         for(int j = 0;  j<glist[getout[i]]->fanin.size(); j++){
//           cout << "in" << j << ": " <<glist[getout[i]]->fanin[j]->getid() << endl;
//         }
//         for(int j = 0;  j<glist[getout[i]]->fanout.size(); j++){
//           cout << "out" << j << ": " << glist[getout[i]]->fanout[j]->getid() << endl;
//         }
//      }
//       else
//       {
//          cout << "NULL" << endl;
//       }
//    }

//       cout << "======= CHECK aig========" << endl;
//       for(int i  = 0; i < aig.size(); i++){
//         cout << "aig[" << i << "] : ";
//         if(aig[i] != NULL){
//           cout << aig[i]->getid() << endl;
//           for(int j = 0;  j<aig[i]->fanin.size(); j++){
//             cout << "in" << j << ": " << aig[i]->fanin[j]->getid() << endl;
//           }
//           for(int j = 0;  j<aig[i]->fanout.size(); j++){
//             cout << "out" << j << ": " << aig[i]->fanout[j]->getid() << endl;
//           }
//         }
//          else
//          {
//             cout << "NULL" << endl;
//          }
//       }
//  cout << "============CHECK END=============" <<endl;


  //         cout << "======= CHECK glist========" << endl;
  //     for(int i  = 0; i < glist.size(); i++){
  //       cout << "glist[" << i << "] : ";
  //       if(glist[i] != NULL){
  //         cout << glist[i]->getid() << endl;
  //         for(int j = 0;  j<glist[i]->fanin.size(); j++){
  //           cout << "in" << j << ": " << glist[i]->fanin[j]->getid() << endl;
  //         }
  //         for(int j = 0;  j<glist[i]->fanout.size(); j++){
  //           cout << "out" << j << ": " << glist[i]->fanout[j]->getid() << endl;
  //         }
  //       }
  //        else
  //        {
  //           cout << "NULL" << endl;
  //        }
  //     }
  // cout << "============CHECK END=============" <<endl;



    dfslist.clear();
    dfs(out);

//     cout << "======= CHECK dfslist========" << endl;
//     cout << dfslist.size() << endl;
//    for(int i  = 0; i < dfslist.size(); i++){
//      cout << "dfslist[" << i << "] : ";
//      if(dfslist[i] != NULL){
//        cout << dfslist[i]->getid() << endl;
//         for(int j = 0;  j<dfslist[i]->fanin.size(); j++){
//           cout << "in" << j << ": " <<dfslist[i]->fanin[j]->getid() << endl;
//         }
//         for(int j = 0;  j<dfslist[i]->fanout.size(); j++){
//           cout << "out" << j << ": " << dfslist[i]->fanout[j]->getid() << endl;
//         }
//      }
      
//       else
//       {
//          cout << "NULL" << endl;
//       }
//    }
//  cout << "============CHECK END=============" <<endl;
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

void
CirMgr::removegate(CirGate* p){
  p->delficon();
  p->delfocon();
  glist[p->getid()] = NULL;
  delete p;
}

