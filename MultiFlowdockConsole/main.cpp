#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>//For atoi
#include "MultiFlowdockAPI.h"
#include "Library.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32
#include <unistd.h>//For usleep
#include <limits.h>//For PATH_MAX
#endif

using namespace std;

int main(int argc, char *argv[])
{
   std::string strPath;
#ifdef WIN32
   strPath = "..\\..\\MultiFlowdockAPI\\Debug\\MultiFlowdockAPI.dll";
#else
   char path[PATH_MAX] = "/proc/self/exe";
   char dest[PATH_MAX];
   readlink(path, dest, PATH_MAX);
   std::string strEXE(dest);
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//EXE folder
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//Build folder

   strPath = strEXE + "/MultiFlowdockAPI/libMultiFlowdockAPI.so";

#endif
   RLibrary library(strPath);

   if( !library.Load() )
      return 0;

   MultiFlowdockCreateFunc CreateAPI = (MultiFlowdockCreateFunc)library.Resolve("MultiFlowdockCreate");
   if( !CreateAPI )
      return 0;

   MultiFlowdockAPI pFlowdock = NULL;
   CreateAPI(&pFlowdock);

   ///

   MultiFlowdockAddFlowFunc AddFlow = (MultiFlowdockAddFlowFunc)library.Resolve("MultiFlowdockAddFlow");
   if( !AddFlow )
      return 0;

   AddFlow(pFlowdock, "aj-org", "main", "ajorians@gmail.com", "1Smajjmd");

   while(true)
   //for(int i=0; i<10; i++)
   {
#ifdef _WIN32
      Sleep(1000);//1 second
#else
      usleep(1000*1000);
#endif

      MultiFlowdockGetMessageFunc GetMessage = (MultiFlowdockGetMessageFunc)library.Resolve("MultiFlowdockGetMessage");
      if( !GetMessage )
         return 0;

      char* pstrMessage = NULL;
      int nSizeOfMessage = 0;
      if( 1 == GetMessage(pFlowdock, pstrMessage, nSizeOfMessage, 0) )
      {
         pstrMessage = new char[nSizeOfMessage + 1];

         GetMessage(pFlowdock, pstrMessage, nSizeOfMessage, 1);

         std::string strMessage(pstrMessage);

         delete[] pstrMessage;
         cout << "Message: " << strMessage << endl;
      }
   }

   ///

   MultiFlowdockFreeFunc FreeAPI = (MultiFlowdockFreeFunc)library.Resolve("MultiFlowdockFree");
   if( !FreeAPI )
      return 0;

   FreeAPI(&pFlowdock);

   return 0;
}

