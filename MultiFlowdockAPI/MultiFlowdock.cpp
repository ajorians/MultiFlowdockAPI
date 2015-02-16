#include "MultiFlowdock.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#include <cassert>
#include <iostream>

using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { delete (x); (x) = NULL; }
#endif

#ifndef ARR_SIZE
#define ARR_SIZE(x) sizeof(x)/sizeof(x[0])
#endif

#ifdef _MSC_VER 
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

MULTIFLOWDOCK_EXTERN int MultiFlowdockCreate(MultiFlowdockAPI* api)
{
   *api = new MultiFlowdock();

   return 0;
}

MULTIFLOWDOCK_EXTERN int MultiFlowdockFree(MultiFlowdockAPI *api)
{
   MultiFlowdock* pFlowdock = (MultiFlowdock*)*api;
   delete pFlowdock;
   return 0;
}

MULTIFLOWDOCK_EXTERN int MultiFlowdockAddFlow(MultiFlowdockAPI api, const char* pstrOrg, const char* pstrFlow, const char* pstrUsername, const char* pstrPassword)
{
   std::string strOrg(pstrOrg), strFlow(pstrFlow), strUsername(pstrUsername), strPassword(pstrPassword);
   MultiFlowdock* pFlowdock = (MultiFlowdock*)api;
   pFlowdock->AddFlow(strOrg, strFlow, strUsername, strPassword);
   return 0;
}

MULTIFLOWDOCK_EXTERN int MultiFlowdockGetMessage(MultiFlowdockAPI api, char* pstrMessage, int& nSizeOfMessage, int nDelete)
{
   MultiFlowdock* pFlowdock = (MultiFlowdock*)api;
   std::string strMessageContent = pFlowdock->GetListenMessage(nDelete == 1);
   if( nSizeOfMessage > 0 )
   {
      memcpy(pstrMessage, strMessageContent.c_str(), nSizeOfMessage + 1);
   }
   else
   {
      nSizeOfMessage = strMessageContent.size();
   }

   return strMessageContent.size()>0 ? 1 : 0;
}

MultiFlowdock::MultiFlowdock()
{
   std::string strPath;
#ifdef WIN32
   strPath = "..\\..\\FlowdockAPI\\FlowdockAPI\\Debug\\FlowdockAPI.dll";
#else
   char path[PATH_MAX] = "/proc/self/exe";
   char dest[PATH_MAX];
   readlink(path, dest, PATH_MAX);
   std::string strEXE(dest);
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//EXE folder
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//Build folder

   strPath = strEXE + "/FlowdockAPI/libFlowdockAPI.so";

#endif
   m_libFlowdockAPI.SetLibrary(strPath);

   bool bOK = m_libFlowdockAPI.Load();
   assert(bOK == true);
}

MultiFlowdock::~MultiFlowdock()
{
   FlowdockFreeFunc FreeAPI = (FlowdockFreeFunc)m_libFlowdockAPI.Resolve("FlowdockFree");
   assert(FreeAPI != NULL);

   for(std::vector<FlowdockAPI>::size_type i=0; i<m_aFlows.size(); i++)
      FreeAPI(&m_aFlows[i]);
}

bool MultiFlowdock::AddFlow(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
{
   FlowdockCreateFunc CreateAPI = (FlowdockCreateFunc)m_libFlowdockAPI.Resolve("FlowdockCreate");
   if( !CreateAPI )
      return false;

   FlowdockAPI pFlowdock = NULL;
   CreateAPI(&pFlowdock, 0);

   m_aFlows.push_back(pFlowdock);

   ///

   FlowdockGetUsersFunc GetUsers = (FlowdockGetUsersFunc)m_libFlowdockAPI.Resolve("FlowdockGetUsers");
   if( !GetUsers )
      return false;

   GetUsers(pFlowdock, strOrg.c_str(), strFlow.c_str(), strUsername.c_str(), strPassword.c_str());

   ///

   FlowdockAddListenFlowFunc AddListenFlow = (FlowdockAddListenFlowFunc)m_libFlowdockAPI.Resolve("FlowdockAddListenFlow");
   if( !AddListenFlow )
      return false;

   AddListenFlow(pFlowdock, strOrg.c_str(), strFlow.c_str());

   FlowdockStartListeningFunc StartListening = (FlowdockStartListeningFunc)m_libFlowdockAPI.Resolve("FlowdockStartListening");
   if( !StartListening )
      return false;

   StartListening(pFlowdock, strUsername.c_str(), strPassword.c_str());

   return true;
}

std::string MultiFlowdock::GetListenMessage(bool bDelete)
{
   std::string strRet;

   for(std::vector<FlowdockAPI>::size_type i=0; i<m_aFlows.size(); i++)
   {
      FlowdockAPI pFlow = m_aFlows[i];
      FlowdockGetListenMessageCountFunc GetListenMessagesCount = (FlowdockGetListenMessageCountFunc)m_libFlowdockAPI.Resolve("FlowdockGetListenMessageCount");
      if( !GetListenMessagesCount )
      {
         assert(false);
         break;
      }

      int nCount = GetListenMessagesCount(pFlow);
      if( nCount <= 0 )
         continue;

      FlowdockGetListenMessageTypeFunc GetListenMessagesType = (FlowdockGetListenMessageTypeFunc)m_libFlowdockAPI.Resolve("FlowdockGetListenMessageType");
      if( !GetListenMessagesType )
         return 0;

      int nType = GetListenMessagesType(pFlow, 0);
      if( nType == 0 )
      {
         FlowdockGetMessageContentFunc GetMessage = (FlowdockGetMessageContentFunc)m_libFlowdockAPI.Resolve("FlowdockGetMessageContent");
         if( !GetMessage )
            return 0;

         char* pstrMessage = NULL;
         int nSizeOfMessage = 0;
         GetMessage(pFlow, 0, pstrMessage, nSizeOfMessage);

         pstrMessage = new char[nSizeOfMessage + 1];

         GetMessage(pFlow, 0, pstrMessage, nSizeOfMessage);

         std::string strMessage(pstrMessage);
         strRet = strMessage;
      }

      if(nType != 0 || bDelete == true)
      {
         FlowdockRemoveListenMessageFunc RemoveListenMessage = (FlowdockRemoveListenMessageFunc)m_libFlowdockAPI.Resolve("FlowdockRemoveListenMessage");
         if( !RemoveListenMessage )
            return 0;

         RemoveListenMessage(pFlow, 0);
      }

   }

   return strRet;
}

