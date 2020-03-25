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
#include <bitset>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;
int CirGate::_mark = 0;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate::CirGate()
{}


void
CirGate::reportGate() const
{
   cout << "================================================================================" << endl;
   string s = "= " + getTypeStr() + "(" + to_string(id) + ")";
   if(symbol != "") s = s + "\"" + symbol + "\"";
   s = s + ", line " + to_string(lineNo);
   cout <<  s << endl;

   s = "= FECs: ";
   cout << s << endl;

   s = "= Value: ";
   cout << s;

   s="";
   bitset<64> b{value};
   string l = b.to_string();
   for(int i = 0; i  < 64; i++){
      if(i % 8 == 0 && i != 0){s += "_";}
      s += l[63-i];
   }
   cout << s << endl;
   cout << "================================================================================" << endl;
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
      for(int i = 0; i < n->fanin.size();  ++i){
         if(n->fanin[i] != NULL)
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
   assert(level >= 0);
   addmark();
   printFoDFS(0,level,0);
}

void 
CirGate::printFoDFS(int space, int level, bool inv) const{
   assert (level >= 0);

   for (int i = 0; i < space; i++){
      cout << " ";
   }

   if (inv){cout << '!';}
   cout << getTypeStr() << ' ' << id;
   if (mark == _mark && level>0 && fanout.size()!=0)
      cout << " (*)";
   cout<<endl;
   if(mark != _mark && level>0 and fanout.size()!=0)
   {
      mark = _mark;
      for(int i=0;i<fanout.size();i++){
         if(fanout[i]->fanin[0]->getid()==id){
            fanout[i]->printFoDFS(space+2,level-1,fanout[i]->n1);
         }
         if(fanout[i]->fanin[1]->getid()==id){
            fanout[i]->printFoDFS(space+2,level-1,fanout[i]->n2);
         }
      }
   }
}

void
CirGate::delficon(){
   if(fanin.size()==0) return;
   if(fanin[0] != NULL){
      for(int i = 0; i < fanin[0]->fanout.size(); ++i){
         if(fanin[0]->fanout[i]->id == id)
            fanin[0]->fanout.erase(fanin[0]->fanout.begin()+i);
       }
   }
   if(fanin[1] != NULL){
      for(int i = 0; i < fanin[1]->fanout.size(); ++i){
         if(fanin[1]->fanout[i]->id == id)
            fanin[1]->fanout.erase(fanin[1]->fanout.begin()+i);
       }
   }
}

void
CirGate::delfocon(){
  for(int i = 0; i < fanout.size(); ++i){
    if(fanout[i]->fanin[0] == this)
      fanout[i]->fanin[0] = NULL;
    if(fanout[i]->fanin[1] == this)
      fanout[i]->fanin[1] = NULL;
  }
}

// zero -> true -> fanin[0] -> 0 
//           -> false -> fanin[1] -> 0
void 
CirGate::gettogether(bool one, bool zero){
   // 1
  delficon();
//   cout << one;
//   cout << " is in fanin[" << !zero << "]" << endl;
  if(zero)  cout << "Simplifying: " << fanin[0]->id << " merging ";
  else cout << "Simplifying: " << fanin[1]->id << " merging ";
  if(one){
     cout << "!";
    if(zero){
       if(fanin[1] != NULL)
          confanin_n(fanin[1], n2);
    }
    else{
       if(fanin[0] != NULL)
         confanin_n(fanin[0], n1);
    }
  }
  //0
  else{
     if(zero){
        confanin(fanin[0]);
     }
     else{
        confanin(fanin[0]);
     }
  }
  cout << id << "..." << endl;
//   cout << endl;
}

void 
CirGate::getsame(bool rev){
  delficon();
//   cout << id << " has the same input " << fanin[0]->id  << " "<< fanin[1]->id << endl;
//    cout << "inverse? " << rev << endl;

  cout << "Simplifying: " << fanin[0]->id << " merging ";
  if(rev)   cout << "!";

  confanin_n(fanin[0], rev);
  cout << id << "..." << endl;
   // cout << endl;
}

void
CirGate::getconst(CirGate* const0){
   delficon();
   cout << "Simplifying: " << "0" << " merging ";

   confanin(const0);
   cout << id << "..." << endl;
}

void
CirGate::confanin(CirGate* p){
   for (int i = 0; i < fanout.size(); ++i){
      if(fanout[i] != NULL){
         for(int j = 0; j < fanout[i]->fanin.size(); ++j){
            if(fanout[i]->fanin[j] != NULL){
               if(id == fanout[i]->fanin[j]->getid()){
                  fanout[i]->fanin[j] = p;
                  p->fanout.push_back(fanout[i]);
               }
            }
         } 
      }
   }
}

void
CirGate::confanin_n(CirGate* p, bool rev){
   for (int i = 0; i < fanout.size(); ++i){
      if(fanout[i] != NULL){
         for(int j = 0; j < fanout[i]->fanin.size(); ++j){
            if(fanout[i]->fanin[j] != NULL){
               if(id == fanout[i]->fanin[j]->getid()){
                  fanout[i]->fanin[j] = p;
                  if(j == 0){
                     fanout[i]->n1 = rev ^ fanout[i]->n1;
                  }  
                  else if(j == 1){
                     fanout[i]->n2 = rev ^ fanout[i]->n2;
                  }
               }
                  p->fanout.push_back(fanout[i]);
            }
         } 
      }
   }
}

void
CirGate::mergegate(CirGate *p){
   for(int i = 0; i < p->fanout.size(); ++i){
      fanout.push_back(p->fanout[i]);

      if(p->fanout[i]->fanin[0] == p)
         p->fanout[i]->fanin[0] = this;
      if(p->fanout[i]->fanin[1] == p)
         p->fanout[i]->fanin[1] = this;
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



