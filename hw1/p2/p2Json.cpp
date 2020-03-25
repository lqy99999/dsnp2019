/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include<fstream>
#include <string>
#include <string.h>
#include <vector>
#include<stdlib.h>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here

bool Json::read(const string& jsonFile)
{  
   fstream file;
   char buffer[500];
   char _value[50];
   // char _key [50];
   string skey;
   int svalue;

   file.open(jsonFile, ios::in);

   if(!file){
      return false;
   } else{
      do{
         file.getline(buffer,sizeof(buffer));
         if(strchr(buffer,'}') != NULL)
            break;

         if(strchr(buffer,':')!=NULL){
         sscanf(buffer,"%*[^:]%*s%[^,]",_value);
         //test2.json is failed.
        // sscanf(buffer,"%*[^\"]\"%[^\"]",_key);
         //%*[^\"]\"%[^\"]

         bool test = false;
         for(int i  = 0; i <sizeof(buffer); i++){
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
         JsonElem jelem(skey, svalue);
         _obj.push_back(jelem);

         skey = "";
         }
      }while(!file.eof());
   }
      // TEST
      // cout << _obj.size();
      file.close();
      return true;
}

ostream& operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void Json::print(){
   if(!_obj.empty()){
   cout << "{" << endl;
      for(float i = 0; i < _obj.size(); i++){
         // cout << "  " << _obj.at(i).getKey();
         // cout << "  :  " << _obj.at(i).getValue();
         cout << "  ";
         cout << _obj.at(i);
         if(i < _obj.size()-1)
            cout << "," << endl;
      }
      cout << endl <<  "}"<<endl;
   }else {
      cout << "{" << endl;
      cout << "}" << endl;
   }

}

int Json::sum(){
   int sum = 0;
   for(int i = 0; i < _obj.size(); i++){
      sum += _obj.at(i).getValue();
   }
   return sum;
}

float Json::ave(){
   int sum = 0;
   float ave = 0;
   for(int i = 0; i < _obj.size(); i++){
      sum += _obj.at(i).getValue();
   }
   ave = (float)sum / (float)_obj.size();
   return ave;
}

void Json::max(){
   int  max = _obj.at(0).getValue();
   int index = 0;
   for(int i = 0; i < _obj.size(); i++){
      if(max < _obj.at(i).getValue()){
         max = _obj.at(i).getValue();
         index = i;
      }
   }
   cout << "The maximum element is: ";
   cout << "{ " << _obj.at(index) << " }." << endl;
}

void Json::min(){
   int  min = _obj.at(0).getValue();
   int index = 0;
   for(int i = 0; i < _obj.size(); i++){
      if(min > _obj.at(i).getValue()){
         min = _obj.at(i).getValue();
         index = i;
      }
   }
   cout << "The minimum element is: ";
   cout << "{ " << _obj.at(index) << " }." << endl;
}

bool Json::isEmpty(){
   if(_obj.size() == 0)
      return true;
   else 
      return false;
}

void Json::add(const string& k, int v){
   JsonElem newjson(k, v);
   _obj.push_back(newjson);
}

// bool Json::check(const string& cmd){
//    string add = "", addkey = "", addvalue = "";
//    sscanf(cmd, "%3s%*s%s%s",add,addkey,addvalue);
//    if(add != "ADD"||addkey.empty()||addvalue.empty()){
//       return false;
//    }else
//       return true;
// }

string JsonElem::getKey(){
   return _key;
}

int JsonElem::getValue(){
   return _value;
}




