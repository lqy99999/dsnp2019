/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include<stdlib.h>
#include <iomanip>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   // cout << "Enter command: ";
   while (true) {
      string cmd, addkey, addvalue;
      int _addvalue = NULL;
      cout << "Enter command: ";
      cin >> cmd;

      if(cmd == "ADD"){
         cin >> addkey;
         cin >> addvalue;
         _addvalue = stoi(addvalue);
      }
      
      if(cmd == "PRINT"){
         json.print();
      }else if (cmd == "EXIT")
         break;  
      else if (cmd == "SUM"){
         if(!json.isEmpty()){
         cout << "The summation of the values is: " << json.sum()  << ".";
         cout << endl;
         }
         // else 
         // cout << "Error: No element found!!" << endl;
      }
      else if (cmd == "AVE"){
         if(!json.isEmpty()){
         cout << "The average of the values is: ";
         cout << fixed << setprecision(1) <<  json.ave() << ".";
         cout << endl;
          }
         // else 
         // cout << "Error: No element found!!" << endl;
      }
      else if (cmd == "MAX"){
         if(!json.isEmpty())
          json.max();
         // else 
         //  cout << "Error: No element found!!" << endl;
      }
      else if (cmd == "MIN"){
         if(!json.isEmpty())
           json.min();
         // else 
         //  cout << "Error: No element found!!" << endl;
      }
      else if (cmd == "ADD"){
         if(_addvalue != NULL)
            json.add(addkey,_addvalue);
      }

   }

}