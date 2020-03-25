/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */ 
                  if(moveBufPtr(_readBufPtr - 1))
                     deleteChar();
                  break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : 
                   addHistory();
                   cout << char(NEWLINE_KEY);
                   resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */ moveBufPtr(_readBufPtr+1); break;
         case ARROW_LEFT_KEY : /* TODO */ moveBufPtr(_readBufPtr-1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */
                  insertChar(' ',TAB_POSITION -  (_readBufPtr - _readBuf)%TAB_POSITION );
                  break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...

   if((ptr > _readBufEnd) || (ptr < _readBuf)){
      mybeep();
      return false;
   }

      while(ptr < _readBufPtr){
         cout <<"\b";
         _readBufPtr -- ;
      }   

      if (ptr > _readBufPtr){
         for(int i = 0; i < (_readBufEnd - _readBufPtr); i++)
            cout << *(_readBufPtr + i);
         
         int count = _readBufEnd - ptr;
         for( int i= 0; i < count; i++) 
            cout << "\b";
      }

   _readBufPtr = ptr;
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//                           ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBufPtr == _readBufEnd){
      mybeep();
      return false;   
   }
   else {
      for(int j = 0; j < _readBufEnd - _readBufPtr; j++){
         *(_readBufPtr + j) = *(_readBufPtr + 1 + j);
         // TEST
         // cout << endl << *(_readBufPtr + j);
      }
      _readBufEnd --; 
      *_readBufEnd = ' ';

      int count = _readBufEnd - _readBufPtr; 

      for(int i = 0; i <= count; i++) // replace the last one in screen
         cout << *(_readBufPtr + i); 
      for(int k = 0; k <= count; k++)
         cout << "\b";
   }
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//                           ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                                  ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);

   for(int t = 0; t < repeat; t ++){
      if(_readBufPtr == _readBufEnd){ // ptr at the end of line
         *(_readBufPtr) = ch;
         cout << ch;
         _readBufPtr ++;
         _readBufEnd++;
      }

      if(_readBufPtr < _readBufEnd){ // ptr on the middle of the line
         int count = _readBufEnd - _readBufPtr;

         for(int i = 0; i <= count; i++)
            *(_readBufEnd - i + 1) = *(_readBufEnd - i);
         *(_readBufPtr) = ch;

         for (int j = 0; j <= count; j++)
            cout << *(_readBufPtr + j);

         _readBufPtr ++;
         _readBufEnd++;

         for(int k = 0; k < count ; k++)
            cout << "\b";

         _historyIdx = _history.size();


      }
   }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   moveBufPtr(_readBuf);
   while( _readBufEnd  >_readBuf)
      deleteChar();

   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufEnd= 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   string str(_readBuf);
   if( index  < _historyIdx){ // moving up 
      if(_historyIdx == 0){
         mybeep();
         index = 0;
      }
      else if(index < 0){
         index = 0;
      }
      else {
         if(!_tempCmdStored && _historyIdx == _history.size() ){
            string str(_readBuf);
            _history.push_back(str);
            _tempCmdStored = true;
         }
         
         _historyIdx = index;   
   retrieveHistory();

         // if(_tempCmdStored && _historyIdx == _history.size() - 1){
         //    _history.pop_back();
         //    _tempCmdStored = false;
         // }

      }

   }



   else if (index > _historyIdx) {// moving down
      if(_historyIdx == _history.size())
         mybeep();

      else if(index >= _history.size()){
            index = _history.size() - 1;
            // _tempCmdStored = false;
         }
      else{
         _historyIdx = index;   
         retrieveHistory();

         if(_tempCmdStored && _historyIdx == _history.size() - 1){
            _history.pop_back();
            _tempCmdStored = false;
         }  
      }
   }

}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   //temporarily store , don't remove ' ' at the beginning and
   // the end of _readBuf
   string str(_readBuf);
   if(str != ""){
      // if(_tempCmdStored){ 
      //    _history.push_back(str);
      //    _tempCmdStored = false;
      // }
      // else{
         // _history.pop_back();
         if(_tempCmdStored){
            _history.pop_back();
         }

         str.erase(0,str.find_first_not_of(" "));
         str.erase(str.find_last_not_of(" ")+1);
         _history.push_back(str);
      // }
      _historyIdx  = _history.size();

      //clean _readBuf
      _readBufPtr = _readBufEnd;
      while (_readBufPtr >= _readBuf){
         *_readBufPtr = '\0';
         _readBufPtr --;
      }
     _readBufEnd =  _readBufPtr = _readBuf;
   }
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
