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
#include <unordered_map>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;


#define MAX_LEN 65536
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

   // Member functions about circuit optimization
   void sweep();
   void optimize();
   
   void removegate(CirGate* p);
   
   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
  
  void writelog(int count);
  //input has sim value
  void runsim();
  //run the remained gate
  void runans();
  // split the group
  void splitgroup();
  void sortgroup();
  string reversebit(const size_t &a);

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

  //b merge to a 
   void mergegate(CirGate*a, CirGate*b);
   size_t buildkey(CirGate* a);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

   void printFECPairs() const;
   void writeGate(ostream&, CirGate*) const;
   
   void connect();
   void dfs(const GateList &);

private:
   ofstream           *_simLog;
    
    bool sim = false;
    unordered_map<size_t, IdList> group;
    vector<IdList*> fecgroup;
    vector<size_t>* simpattern;

    unsigned int aag[5]; // M I L O A
    vector<string> tp;
    vector<string> comment;    
    
    GateList in; // vector<CirGate*>  input 
    GateList out; // vector<CirGate*>  output 

    GateList aig;
    GateList glist;// all gate
    GateList dfslist;

    // only change in beginning and strash
    IdList flist; // get float input


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
