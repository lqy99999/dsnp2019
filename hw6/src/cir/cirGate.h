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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   static int _mark;
   friend class CirMgr;
   CirGate();
   virtual ~CirGate();

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return lineNo; }

   // Printing functions
   virtual void printGate() const = 0;
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

private:


protected:
  unsigned int id;
  unsigned int lineNo;
  string symbol = "";
  GateList fanin;
  GateList fanout;
  bool n1 = false;
  bool n2 = false;

  bool isfind = false;

  mutable int mark = 0;
};

class CirUNDEFGate : public CirGate{
   public:
   CirUNDEFGate(unsigned int&, unsigned);
   ~CirUNDEFGate() {}
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
   ~CirPOGate() {}
   void printGate() const;
   string getTypeStr() const{return"PO";};
   unsigned getin(){return _in;};

   private:
   unsigned _in;
};

class CirAIGGate : public CirGate{
  public:
   CirAIGGate(unsigned int&, unsigned in1, unsigned in2 , unsigned);
   ~CirAIGGate() {}
   void printGate() const;
   string getTypeStr() const{return"AIG";};
   unsigned getin1(){return _in1;};
   unsigned getin2(){return _in2;};

   private:
   unsigned _in1;
   unsigned _in2;
};

class CirCONSTGate : public CirGate{
   public:
   CirCONSTGate(unsigned int i);
   ~CirCONSTGate() {}
   void printGate() const;
   string getTypeStr() const{return"CONST";};
};

#endif // CIR_GATE_H
