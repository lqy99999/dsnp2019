/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

#define MAX_LEN 65536
// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr();
   ~CirMgr() {
     in.clear();
     out.clear();
     aig.clear();
     flist.clear();
     glist.clear();
     dfslist.clear();
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const 
   {
     CirGate* g = glist[gid];
     if(g == NULL)  return 0; 
     return g;
    }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void connect();
   void dfs(const GateList &);
  

private:
    unsigned int aag[5]; // M I L O A
    vector<string> tp;
    vector<string> comment;    
    
    GateList in; // vector<CirGate*>  input 
    GateList out; // vector<CirGate*>  output 
    GateList aig;
    IdList flist; // get float input
    GateList glist;// all gate

    GateList dfslist;

    void readHeader();
    void readInput(); 
    void readOutput();
    void readAig();

    unsigned int toNum(int & a)
    {
      int tmp = a % 2;
      if(tmp == 1)  return(unsigned int)((a-1)/2);
      else return(unsigned int)(a/2);
    }
};

#endif // CIR_MGR_H
