/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   // string jsonFile;
   // fstream file;
   // is >> file;
   
   char buffer[500];
   char _value[50];
   string skey;
   int svalue;

   // file.open(jsonFile, ios::in);
   // do{
   while(is.getline(buffer, sizeof(buffer))){
      if(strchr(buffer,'}') != NULL)
         break;

      if(strchr(buffer,':')!=NULL){
         sscanf(buffer,"%*[^:]%*s%[^,]",_value);
         //test2.json is failed.
        // sscanf(buffer,"%*[^\"]\"%[^\"]",_key);
         //%*[^\"]\"%[^\"]

         bool test = false;
         for(unsigned int i  = 0; i <sizeof(buffer); i++){
            if (buffer[i] == '\"'){
               test = !test;
               i++;
            } 
            if(test)
               skey = skey+buffer[i];  
         }
         svalue = atoi(_value);
         // TEST
         //  cout << skey<<endl;
         // cout << svalue<<endl;
         DBJsonElem jelem(skey, svalue);
         j._obj.push_back(jelem);

         skey = "";
      }
   }  
   j.open = true;
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
      if(!j._obj.empty()){
         cout << "{" << endl;
      for(float i = 0; i < j._obj.size(); i++){
         // cout << "  " << _obj.at(i).getKey();
         // cout << "  :  " << _obj.at(i).getValue();
         cout << "  ";
         cout << j._obj.at(i);
         if(i < j._obj.size()-1)
            cout << "," << endl;
      }
      cout << endl <<  "}"<<endl;
   }else {
      cout << "{" << endl;
      cout << "}" << endl;
   }
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
   _obj.shrink_to_fit();
   open = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for(unsigned int i = 0; i < _obj.size(); i++){
      if(elm.key() == _obj.at(i).key())
         return false;
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(_obj.empty())
      return 0.0;

   int sum = 0;
   float ave = 0;
   for(unsigned int i = 0; i < _obj.size(); i++){
      sum += _obj.at(i).value();
   }
   ave = (float)sum / (float)_obj.size();
   return ave;

}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.empty()){
      idx = _obj.size();
      return INT_MAX;
   }

   for(size_t i = 0; i < _obj.size(); i++){
      if(maxN < _obj[i].value()){
         maxN = _obj[i].value();
         idx = i;
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.empty()){
      idx = _obj.size();
      return INT_MIN;
   }

   for(size_t i = 0; i < _obj.size(); i++){
      if(minN > _obj[i].value()){
         minN = _obj[i].value();
         idx = i;
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(_obj.empty())
      return 0;
   else{
      int s = 0;
      for(unsigned int i = 0; i < _obj.size(); i++)
         s += _obj.at(i).value();
      return s;
   }
}
