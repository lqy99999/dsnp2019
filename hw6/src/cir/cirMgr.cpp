/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include<fstream>
#include <ctype.h>
#include <cassert>
#include <assert.h>
#include <cstring>
#include <string>
#include <string.h>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;


static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::CirMgr(){
     CirGate* g = new CirCONSTGate(0);
     glist.push_back(g);
}

bool
CirMgr::readCircuit(const string& fileName)
{
   fstream file;
   char buffer[MAX_LEN];
   string line;
   string tmp = " ";
   size_t idx = 0;
   bool com = false;
   bool first = true;
   vector<int> i_input;
   vector<string> i_name;

   vector<int> o_input;
   vector<string> o_name;

   file.open(fileName, ios::in);

   if(!file){
      cerr << "Can't open file." << endl;
      return false;
   } 
   else
   {
      do
      {
         file.getline(buffer,sizeof(buffer));
         // cout << "Line: " << cnt << endl;
         line = buffer;

         while(true)
         {
            if(idx == string::npos) break; 
            if(!com)
            {
               if(first) // get header // 會多一個在vector<string>head 後面
               {
                  while(tmp != "")
                  {
                     idx = myStrGetTok(line, tmp, idx);
                     tp.push_back(tmp);
                     // cout << "Header: " << tmp;
                     // cout << "        idx = " << idx << endl;
                  }
                  idx = 0;
                  tmp = " ";
                  first = false;
                  tp.clear();
                  readHeader();
                  glist.resize(aag[0] + aag[3] + 1);
                  break;
               }

               // get input/ouput/AIG def
               idx = myStrGetTok(line, tmp, idx);

               if(line[0] == 'c')
               {
                  com = true;
                  continue;
               }

               if(line[0]=='i' || line[0]=='o'){
                  int input;
                  int cut1 = line.find_first_of(' ', 0);
                  if(!myStr2Int(line.substr(1,cut1 - 1), input))
                     return false;
                  if(line[0] == 'i'){
                     i_input.push_back(input);
                     i_name.push_back(line.substr(cut1 +1, line.size()-cut1 - 1));
                     // cout << "i" << input << " "<< line.substr(cut1 +1, line.size()-cut1 - 1) << endl;
                  }
                  else{
                     o_input.push_back(input);
                     o_name.push_back(line.substr(cut1 +1, line.size()-cut1 - 1));
                     // cout << "o" << input << " "<< line.substr(cut1 +1, line.size()-cut1 - 1) << endl;
                  }
                  continue;
               }  

               tp.push_back(tmp);
               // cout << "AIG: " << tmp;
               // cout << "        idx = " << idx << endl;
            } 
            else  // get comment
            {
               idx = myStrGetTok(line, tmp, idx, '\n');
               comment.push_back(tmp);  // 會多一個在vector<string>comment 後面
               // cout << "COM: " << tmp;
               // cout << "        idx = " << idx << endl;
            } 
         }
         idx = 0;
         tmp = "";
         // svalue = atoi(_value);
      }while(!file.eof());
         // cout << "END" << endl;
      readInput();
      // cout << "INPUT END " << endl;
      readOutput();
      // cout << "OUTPUT END " << endl;
      readAig();
      // cout << "AIG END " << endl;
  
      connect();
      dfs(out);

      for(int i = 0; i < i_input.size(); i++){
         in[i_input[i]]->symbol = i_name[i];
      }
      for(int i = 0; i < o_input.size(); i++){
         out[o_input[i]]->symbol = o_name[i];
      }
      // cout << "======= CHECK glist========" << endl;
      // for(int i  = 0; i < glist.size(); i++){
      //   cout << "glist[" << i << "] : ";
      //   if(glist[i] != NULL)
      //    cout << glist[i]->getid() << endl;
      //    else
      //    {
      //       cout << "NULL" << endl;
      //    }
      // }

      // cout << "======= CHECK flist========" << endl;
      // cout << "flist.size: " << flist.size() << endl;
      // for(int i  = 0; i < flist.size(); i++){
      //   cout << "flist[" << i << "] : ";
      //   if(flist[i] != NULL)
      //    cout << flist[i] << endl;
      //    else
      //    {
      //       cout << "NULL" << endl;
      //    }
      // }

      // cout << "======= CHECK in========" << endl;
      // for(int i  = 0; i < in.size(); i++){
      //    cout << "in[" << i << "] : "<< in[i]->getid() << endl;
      // }

      // cout << "======= CHECK out========" << endl;
      // for(int i  = 0; i < out.size(); i++){
      //    cout << "out[" << i << "] : " << out[i]->getid() << endl;
      // }

      // cout << "======= CHECK aig========" << endl;
      // for(int i  = 0; i < aig.size(); i++){
      //    cout << "aig[" << i << "] : " << aig[i]->getid() << endl;
      // }

      // cout << "======= CHECK dfslist========" << endl;
      // for(int i  = 0; i < dfslist.size(); i++){
      //    cout << "dfslist[" << i << "] : " << dfslist[i]->getid() << endl;
      // }

      // cout << "==========CHECK END===========" << endl;


   }
   file.close();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI";
   cout << setw(12) << right << aag[1] << endl;
   cout <<"  PO";
   cout << setw(12) << right << aag[3] << endl;
   cout << "  AIG";
   cout << setw(11) << right << aag[4] << endl;
   cout <<"------------------" << endl;
   cout << "  Total";
   cout << setw(9) << right << aag[1] +aag[3] + aag[4] << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   int j = 0;
   for(int i = 0; i < dfslist.size(); ++i){
      if(glist[dfslist[i]->getid()]->getTypeStr() == "UNDEF"){
         continue;
      }
      cout << "[" << j << "] ";
      ++j;
      
      dfslist[i]->printGate();

      if(dfslist[i]->getTypeStr() == "PO"){
         unsigned input = dynamic_cast<CirPOGate*>(dfslist[i])->getin();

         if(glist[input]->getTypeStr() == "UNDEF") cout << "*";
         if(dfslist[i]->getn1()) cout << "!";

         cout << input;
         if(dfslist[i]->symbol != "") cout << " (" << dfslist[i]->symbol << ")";
      }

      if(dfslist[i]->getTypeStr() == "AIG"){
         unsigned input = dynamic_cast<CirAIGGate*>(dfslist[i])->getin1();
         if(glist[input]->getTypeStr() == "UNDEF") cout << "*";
         if(dfslist[i]->getn1()) cout << "!";
         cout << input;

         cout << " ";

         input = dynamic_cast<CirAIGGate*>(dfslist[i])->getin2();
         if(glist[input]->getTypeStr() == "UNDEF") cout << "*";
         if(dfslist[i]->getn2()) cout << "!";
         cout << input;
      }
      cout << endl;
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0; i < in.size(); i++){
      cout << " " << in[i]->getid();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0; i < out.size(); i++){
      cout << " " << out[i]->getid();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   IdList notdefine;
   IdList floatfanin;
   // cout << "here1" << endl;
   for(unsigned int i = 1; i < glist.size(); ++i){
      // cout << "here2" << endl;
      if(glist[i] != NULL && glist[i]->fanout.empty() && glist[i]->getTypeStr() != "PO") 
         notdefine.push_back(glist[i]->getid());
   }

   for(unsigned i = 0; i < flist.size(); ++i){
      for(unsigned j = 0; j < glist[flist[i]]->fanout.size(); ++j)
         if(!(glist[flist[i]]->fanout[j]->isfind)){
            floatfanin.push_back(glist[flist[i]]->fanout[j]->getid());
            glist[flist[i]]->fanout[j]->isfind = true;
         }
   }

   if(!floatfanin.empty()){
      cout << "Gates with floating fanin(s):";
      for(int i = 0; i <  floatfanin.size(); ++i)
         cout << " " << floatfanin[i];
      cout << endl;
   }

   if(!notdefine.empty()){
      cout << "Gates defined but not used  :";   
      for(int i = 0; i <  notdefine.size(); ++i)
         cout << " " << notdefine[i];
      cout << endl;
   }

}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << aag[0] << " " << aag[1] << " " << aag[2] <<" ";
   outfile << aag[3] << " ";

   int cnt = 0;
   for(int i = 0; i < dfslist.size(); ++i){
      if(dfslist[i]->getTypeStr()=="AIG"){
         ++cnt;
      }
   }
   outfile << cnt <<endl;

   for(int i = 0; i < in.size(); ++i){
      outfile << (in[i]->id)*2 << endl; 
   }
   for(int i = 0; i < out.size(); ++i){
      unsigned t = dynamic_cast<CirPOGate*>(out[i])->getin();
      t = (dynamic_cast<CirPOGate*>(out[i])->n1)? t*2+1 :t*2;
      outfile << t << endl;
   }
   for(int i = 0; i < dfslist.size(); ++i){
      if(dfslist[i]->getTypeStr()=="AIG"){
         unsigned t = (dfslist[i] ->id) *2;
         outfile << t << " ";
         if(dfslist[i]->n1) outfile << (dynamic_cast<CirAIGGate*>(dfslist[i])->getin1()) * 2 +1;
         else outfile << (dynamic_cast<CirAIGGate*>(dfslist[i])->getin1()) * 2;

         outfile << " ";

         if(dfslist[i]->n2) outfile << (dynamic_cast<CirAIGGate*>(dfslist[i])->getin2()) * 2 +1;
         else outfile << (dynamic_cast<CirAIGGate*>(dfslist[i])->getin2()) * 2;

         outfile << endl;
      }
      else continue;
   }

   for(int i = 0; i < in.size(); ++i){
      if(in[i]->symbol != "")
         outfile << "i" << i << " " << in[i]->symbol << endl;
      else continue;
   }

   for(int i = 0; i < out.size(); ++i){
      if(out[i]->symbol != ""){
         outfile << "o" << i << " " << out[i]->symbol << endl;
      }
      else continue;
   }
}


void 
CirMgr::readHeader()
{
   for(int i = 0; i < 5; ++i)
   {
      int num;
      if(myStr2Int(tp[i+1], num))
         aag[i] = (unsigned int)num;
      else
      {
         // ERROR
      }
      // cout << "aag[" << i << "]: " << aag[i] << endl; 
   }
}

void 
CirMgr::readInput()
{
   for(size_t i = 0; i < aag[1]; ++i)
   {
      int num;
      if(myStr2Int(tp[i], num))
      {
         unsigned t = toNum(num);
         CirPIGate *a = new CirPIGate(t, i + 2);
         in.push_back(a);
         glist[t] = a;
      }
      else
      {
         // ERROR
      }
   }
}

void 
CirMgr::readOutput()
{
   unsigned t;
   for(int i = 0; i < aag[3]; ++i)
   {
      int num;
      if(myStr2Int(tp[i+aag[1]], num))
      {
         t = aag[0]+ i + 1;
         CirPOGate* a =  new CirPOGate(t, num/2, 2 +aag[1] + i);
         // cout << "output _in: " << num/2 << endl;
         if(num % 2 == 1) a->setn1();
         out.push_back(a);
         glist[t] = a;
      }
      else
      {
         //ERROR
      }
   }
}

void 
CirMgr::readAig()
{
   for(int i = 0; i < aag[4]; ++i)
   {
      int num;
      int idx = (3*i) + aag[1] + aag[3];
      if(myStr2Int(tp[idx], num))
      {
         int in1;
         myStr2Int(tp[idx+1], in1);
         unsigned i1 = toNum(in1);

         int in2;
         myStr2Int(tp[idx+2], in2);
         unsigned i2 = toNum(in2);
         
         unsigned t = toNum(num);
         CirAIGGate* a = new CirAIGGate(t, i1, i2, 2 +aag[1] + aag[3] + i);
         // cout << "t: " << t << endl;
         // cout << "aig _in1: " << i1 << "\t _in2: " << i2 << endl;
         if(in1 % 2 == 1) a->setn1();
         if(in2 % 2 == 1) a->setn2();

         aig.push_back(a);
         glist[t] = a;
      }
      else
      {
         //ERROR
      }
   }
}

void
CirMgr::connect()
{
   // cout << "out" << endl;
   for(int i  = 0; i < out.size(); ++i){ 
      unsigned int idx = (dynamic_cast<CirPOGate *>(out[i]))->getin();
      // cout << "_in: " << idx << endl;
      CirGate* f = glist[idx];
      // cout << "here1" << endl;
      if(f != NULL){
         out[i]->addfanin(f);
         // cout << "here2" << endl;
      }
      else{
         CirGate* u = new CirUNDEFGate(idx, out[i]->getLineNo());
         out[i]->addfanin(u);
         glist[idx] = u;
         flist.push_back(idx);
         // cout << "here3" << endl;
      }
   }

   // cout << "aig" << endl;
   for(int i  = 0; i < aig.size(); ++i){  
      unsigned int idx1 = (dynamic_cast<CirAIGGate *>(aig[i]))->getin1();
      unsigned int idx2 = (dynamic_cast<CirAIGGate *>(aig[i]))->getin2();
      // cout << "_in1: " << idx1 << endl;
      // cout << "_in2: " << idx2 << endl;
      CirGate* f = glist[idx1];
      CirGate* g = glist[idx2];
      // cout << "here1" << endl;

      if(f != NULL){
         // cout << "here2" << endl;
         aig[i]->addfanin(f);
      }
      else{
         // cout << "here3" << endl;
         CirGate* u = new CirUNDEFGate(idx1, aig[i]->getLineNo());
         aig[i]->addfanin(u);
         glist[idx1] = u;
         flist.push_back(idx1);
      }

      if(g != NULL){
         // cout << "here4" << endl;   
         aig[i]->addfanin(g);
      }
      else{
         // cout << "here5" << endl;
         CirGate* s = new CirUNDEFGate(idx2, aig[i]->getLineNo());
         aig[i]->addfanin(s);
         glist[idx2] = s;
         flist.push_back(idx2);
      }

   }
}


void
CirMgr::dfs(const GateList &n)
{
   CirGate::addmark();
   for(int i = 0; i < n.size(); ++i){
      n[i]->dfs(n[i], dfslist);
   }
}