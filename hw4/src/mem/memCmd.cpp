/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   string er = "";
   try{
      if (option == "")
         throw CMD_OPT_MISSING;

      if(option.size()){
         string token1;
         int idx_o = myStrGetTok(option, token1);
         string token2;
         int idx_a = myStrGetTok(option, token2, idx_o);
         string token3;
         int idx_end = myStrGetTok(option, token3, idx_a);
         string token4;
         myStrGetTok(option, token4, idx_end);

         // cout << "======================" << endl;
         // cout << token1 << endl;
         // cout << token2 << endl;
         // cout << token3 << endl;
         // cout << "======================" << endl;
         int size_o = 0;
         int size_a = 0;
         if(token4 != ""){
            er = token4;
            throw CMD_OPT_EXTRA;
         }

            if(myStr2Int(token1,size_o)){
               if(size_o <= 0){
                  er = token1;
                  throw CMD_OPT_ILLEGAL;
               }
               if(size_o == 0){
                  er = token1;
                  throw CMD_OPT_ILLEGAL;
               }

            if(token2 == "" && token3 == ""){
               mtest.newObjs(size_o);
               return CMD_EXEC_DONE;
            }
            
            if(myStrNCmp("-Array", token2, 2) ==0){
               if(token3 == ""){
                  er = token2;
                  throw CMD_OPT_MISSING;   
               }
               if(myStr2Int(token3, size_a)){
                  if(size_a == 0){
                     er = token3;
                     throw CMD_OPT_ILLEGAL;
                  }
                  mtest.newArrs((size_t)size_o, (size_t)size_a);
                  return CMD_EXEC_DONE;
               }else{
                  er = token3;
                  throw  CMD_OPT_ILLEGAL;
               }
            }else{
               er = token2;
               throw CMD_OPT_EXTRA;
            }

         }
            else if(myStrNCmp("-Array", token1, 2) == 0){
               if(token2 == "" && token3 == ""){
                  er = token1;
                  throw CMD_OPT_MISSING;
               }
               if(myStr2Int(token2, size_o)){
                  if(size_o == 0){
                     er = token2;
                     throw CMD_OPT_ILLEGAL;
                  }
                  if(token3 == ""){
                     er = token2;
                     throw CMD_OPT_MISSING;
                  }
                  if(myStr2Int(token3, size_a)){
                     if(size_a == 0){
                     er = token3;
                     throw CMD_OPT_ILLEGAL;
                     }
                     // cout << "===========" << endl;
                     // cout << "size_a : " << size_a << endl;
                     // cout << "size_o : " << size_o << endl;
                     // cout << "===========" << endl;
                     
                     mtest.newArrs(size_a, size_o);
                     return CMD_EXEC_DONE;
                  }else{
                     er = token3;
                     throw CMD_OPT_ILLEGAL;
                  }  
               }else{
                  er = token2;
                  throw CMD_OPT_ILLEGAL;
               }  
            }
            else{
               er = token1;
               throw CMD_OPT_ILLEGAL;
            }
         }
   }catch(bad_alloc){
      return CMD_EXEC_ERROR;
   }
   catch(const CmdOptionError err) {
      return CmdExec::errorOption(err, er);
   }
   // Use try-catch to catch the bad_alloc exception
   // return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
  string er = "";
   try{
      if (option == ""){
         throw CMD_OPT_MISSING;
      }

      if(option.size()){
         string token1;
         int idx_cmd = myStrGetTok(option, token1);
         string token2;
         int idx_size = myStrGetTok(option, token2, idx_cmd);
         string token3;
         int idx_a = myStrGetTok(option, token3, idx_size);
         string token4;
         myStrGetTok(option, token4, idx_a);

      // cout << "===========" << endl;
      // cout << token1 << endl;
      // cout << token2 << endl;
      // cout << token3 << endl;
      // cout << token4 << endl;
      // cout << "===========" << endl;


         if(token4.size()){
            er = token4;
            throw CMD_OPT_ILLEGAL;
         }
         else
         {
            int i = myStrNCmp("-Index", token1, 2); // exists == 0;
            int r = myStrNCmp("-Random", token1, 2); 
            if(i == 0 || r == 0){
               int size_o;
               if(myStr2Int(token2, size_o)){
                  if(size_o < 0){
                     er = token2;
                     throw CMD_OPT_ILLEGAL;
                  }
                  // -I / -R size_o  -A size_a
                  if(myStrNCmp("-Array", token3, 2) == 0){
                     //-I
                     if(i ==0){
                        if(mtest.getArrListSize() <= (size_t)size_o){
                           cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << size_o << "!!" << endl;
                           er = token2;
                           throw CMD_OPT_ILLEGAL;  
                        }
                        mtest.deleteArr(size_o);
                     }
                     // -R
                     else{
                        if(size_o <= 0){
                           er = token2;
                           throw CMD_OPT_ILLEGAL;
                        }
                        for (size_t i = 0; i < size_o; i++){
                           mtest.deleteArr( size_t(rnGen(mtest.getArrListSize())) );
                        }
                     }
                     return CMD_EXEC_DONE;
                  }
                  // -I / -R size_o 
                  else if(token3 == ""){
                     //-I
                     if(i ==0){
                        if(mtest.getObjListSize() <= (size_t)size_o){
                           cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << size_o << "!!" << endl;
                           er = token2;
                           throw CMD_OPT_ILLEGAL;  
                        }
                        mtest.deleteObj(size_o);
                     } //-R
                     else{
                        if(size_o <= 0){
                           er = token2;
                           throw CMD_OPT_ILLEGAL;
                        }
                        for (size_t i = 0; i < size_o; i++){
                           mtest.deleteObj( rnGen(mtest.getObjListSize()));
                        }
                     }
                  }
                  // -Array illegal
                  else{
                     er = token3;
                     throw CMD_OPT_EXTRA;
                  }
               }
               //size_o illegal
               else{
                  er = token2;
                  throw CMD_OPT_ILLEGAL;
               }
            }
            // -A size_o -I/-R size_a
            else if(myStrNCmp("-Array", token1, 2) == 0){
               if(token2 == ""){
                  throw CMD_OPT_MISSING;
               }
               int i = myStrNCmp("-Index", token2, 2);
               int r = myStrNCmp("-Random", token2, 2);
                if( i == 0 ||  r == 0){
                  int size_o;
                  if(myStr2Int(token3, size_o)){
                     if(size_o <= 0){
                        er = token2;
                        throw CMD_OPT_ILLEGAL;
                     }
                        if(i == 0){
                           if(size_o  >= mtest.getArrListSize()){
                              cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " <<
                              size_o << "!!" << endl;
                              er = token3;
                              throw CMD_OPT_ILLEGAL;
                           }
                           mtest.deleteArr(size_o);
                        }else{
                           if(size_o < 0){
                              er = token3;
                              throw CMD_OPT_ILLEGAL;
                           }
                           if(mtest.getArrListSize() == 0){
                              cerr << "Size of array list is 0!!"  << endl;
                              er = token2;
                              throw CMD_OPT_ILLEGAL;  
                           }
                           for (size_t i = 0; i < size_o; i++)
                              mtest.deleteArr( rnGen(mtest.getArrListSize()));
                        }
                        return CMD_EXEC_DONE;
                  }
                  else{
                     er = token2;
                     throw CMD_OPT_ILLEGAL;
                  }
               }
            }
            //illegal
            else{
               er = token1;
               throw CMD_OPT_ILLEGAL;
            }
         }
      }
   }catch(const CmdOptionError err) {
      return CmdExec::errorOption(err, er);
   }
   // return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


