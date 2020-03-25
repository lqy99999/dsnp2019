/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
   
public:
   static int _mark;
   friend class CirMgr;
   CirGate();
   virtual ~CirGate() {};

   // Basic access methods
   virtual string  getTypeStr() const = 0;
   unsigned getLineNo() const { return lineNo; }

   virtual bool isAig() const { return false; }

   // Printing functions
   virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   
  void setn1(){n1 = true;}
   void setn2(){n2 = true;}
   bool getn1(){return n1;}
   bool getn2(){return n2;}
   unsigned getid(){return id;}

   void addfanin(CirGate* &f){
      fanin.push_back(f);
      f->fanout.push_back(this);
   };

   void dfs(CirGate*n, GateList &l);


   static void addmark(){_mark++;};
   void printfanin(int level, int cnt, bool bar) const;
   void printfanout(int level, int cnt, bool bar) const;
   void printFoDFS(int, int, bool) const;

   void delficon();
   void delfocon();
   void gettogether(bool one, bool zero);
   void getsame(bool rev);
   void getconst(CirGate* const0);

   //p ... m c -> c connect to p, p->fanout.push_back(c)
   void confanin(CirGate* p);
   void confanin_n(CirGate* p, bool rev);

   //p merge to myself
   void mergegate(CirGate *p);


private:
     
protected:
  bool sw = true;
  size_t ans = 0;

  size_t value = 0;

  unsigned int id;
  unsigned int lineNo;
  string symbol = "";
  GateList fanin;
  GateList fanout;
  bool n1 = false;
  bool n2 = false;

  mutable int mark = 0;
};


class CirUNDEFGate : public CirGate{
   public:
   CirUNDEFGate(unsigned int&, unsigned);
   ~CirUNDEFGate(){
      id = NULL;
      lineNo = NULL;
      symbol = "";
      fanin.clear();
      fanout.clear();
   }
   void printGate() const;
   string getTypeStr() const{return"UNDEF";};
};

class CirPIGate : public CirGate{
  public:
   CirPIGate(unsigned int&i, unsigned line);
   ~CirPIGate() {}
   void printGate() const;
   string getTypeStr() const{return"PI";};
};

class CirPOGate : public CirGate{
  public:
   CirPOGate(unsigned int&i, unsigned in,unsigned line);
   ~CirPOGate(){
      id = 0;
      lineNo = 0;
      symbol = "";
      fanin.clear();
      fanout.clear();
   }
   void printGate() const;
   string getTypeStr() const{return"PO";};
   unsigned getin(){return _in;};

   private:
   unsigned _in;
};

class CirAIGGate : public CirGate{
  public:
   CirAIGGate(unsigned int&, unsigned in1, unsigned in2 , unsigned);
   ~CirAIGGate(){
      id = 0;
      lineNo = 0;
      symbol = "";
      fanin.clear();
      fanout.clear();
   }
   void printGate() const;
   string getTypeStr() const{return"AIG";};
   unsigned getin1(){return _in1;};
   unsigned getin2(){return _in2;};
   bool isAig() const { return true; }

   private:
   unsigned _in1;
   unsigned _in2;
};

class CirCONSTGate : public CirGate{
   public:
   CirCONSTGate(unsigned int i);
   ~CirCONSTGate(){
      id = 0;
      lineNo = 0;
      symbol = "";
      fanin.clear();
      fanout.clear();
   }
   void printGate() const;
   string getTypeStr() const{return"CONST";};
};


// class Hashnode
// {
//   public:
//    friend class CirMgr;
//     Hashnode();
//     Hashnode(CirGate* p){
//       if(p->fanin[0] != NULL) input0 = p->fanin[0]->getid();
//       if(p->fanin[1] != NULL) input1 = p->fanin[1]->getid();

//       if(input0 < input1){
//           key = (input1 << 5) + (input0 << 7) + (p->n1 << 1) + p->n2;
//        }
//        else{
//           key = (input0 << 5) + (input1 << 7) + (p->n2 << 1) + p->n1;
//        }
//     }
//     ~Hashnode(){}
//     size_t operator() () const{ return key;}
//     bool operator ==(const Hashnode& n) const{return key == n.key;}

//   private:
//     size_t input0 = 0;
//     size_t input1 = 0;
//     size_t key = 0;
// };
#endif // CIR_GATE_H
