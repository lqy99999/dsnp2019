/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;
int CirGate::_mark = 0;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate::CirGate()
{}

CirGate::~CirGate()
{
   id = 0;
   lineNo = 0;
   symbol = "";
   fanin.clear();
   fanout.clear();
}

void
CirGate::reportGate() const
{
   cout << "==================================================" << endl;
   string s = "= " + getTypeStr() + "(" + to_string(id) + ")";
   if(symbol != "") s = s + "\"" + symbol + "\"";
   s = s + ", line " + to_string(lineNo);
   cout << setw(49) << left << s << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   addmark();
   int t = 0;
   printfanin(level, t, false);

}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   addmark();
   int t = 0;
   printfanout(level, t, false);
}

void
CirGate::dfs(CirGate* n, GateList &l){
   if(n->mark != _mark){
      // if(n->fanin.empty()){
      //    n->mark = _mark;
      // }

      for(int i = 0; i < n->fanin.size();  ++i){
         dfs(n->fanin[i],l);
      }
      n->mark = _mark;
      l.push_back(n);
   }
}

void
CirGate::printfanin(int level, int cnt, bool bar) const{
   bool b = false;
   if(cnt != level + 1){
      for(int i  = 0; i < cnt; i++) cout << "  "; 
      string s = "";
      if(bar) s = "!";
      s = s + getTypeStr() + " " + to_string(id);
      cout << s;

      if(mark != _mark){
         cout << endl;
         ++cnt;
         for(int i = 0; i < fanin.size();++i){
            if(i == 0){
               if(n1)
                  b = true;
            }else if(i == 1){
               if(n2)
                  b = true;
            }
            fanin[i]->printfanin(level, cnt, b);
            b = false;
         }
         if(cnt != level + 1)
            mark = _mark; 
      }
      else{
         if(cnt != level && !fanin.empty())
            cout << " (*)";
         cout << endl;
      }

   }
}

void
CirGate::printfanout(int level, int cnt, bool bar) const{
   bool b = false;
   if(cnt != level + 1){
      for(int i  = 0; i < cnt; i++) cout << "  "; 
      string s = "";
      if(bar) s = "!";
      s = s + getTypeStr() + " " + to_string(id);
      cout << s;

      if(mark != _mark){
         cout << endl;
         ++cnt;
         for(int i = 0; i < fanout.size();++i){ 
               if(fanout[i]->fanin[0] != NULL && fanout[i]->fanin[0]->id == id){
                  if(fanout[i]->n1)
                     b = true;
               }
               else if(fanout[i]->fanin[1] != NULL && fanout[i]->fanin[1]->id == id){
                  if(fanout[i]->n2)
                     b = true;
               }
            fanout[i]->printfanout(level, cnt, b);
            b = false;
         } 
         if(cnt != level + 1)
            mark = _mark;
      }
      else{
         if(cnt != level && !fanout.empty())
            cout << " (*)";
         cout << endl;
      }
   }
}

/////////////////////////// UNDEFGate////////////////////////////////////////
CirUNDEFGate::CirUNDEFGate(unsigned int& i , unsigned line)
{
   id = i;
   lineNo = line;
}

void
CirUNDEFGate::printGate() const
{
   cout << "CONST0";
}

/////////////////////////// PIGate////////////////////////////////////////
CirPIGate::CirPIGate(unsigned int& i , unsigned line)
{
   id = i;
   lineNo = line;
}

void
CirPIGate::printGate() const
{
   cout << "PI  " << id;
   if(symbol != "") cout << " (" << symbol << ")";
}

/////////////////////////// POGate////////////////////////////////////////
CirPOGate::CirPOGate(unsigned int& i, unsigned in, unsigned line)
{
   id = i;
   lineNo = line;
   _in = in;
}

void
CirPOGate::printGate() const
{
   cout << "PO  " << id << " ";
}

/////////////////////////// AIGGate////////////////////////////////////////
CirAIGGate::CirAIGGate(unsigned int& i, unsigned in1, unsigned in2, unsigned line)
{
   id = i;
   lineNo = line;
   _in1 = in1;
   _in2 = in2;
}

void
CirAIGGate::printGate() const
{
   cout << "AIG " << id << " ";
}

/////////////////////////// CONSTGate////////////////////////////////////////
CirCONSTGate::CirCONSTGate(unsigned int i)
{
   id = i;
}

void
CirCONSTGate::printGate() const
{
   cout << "CONST0";
}

