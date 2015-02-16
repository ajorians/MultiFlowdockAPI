#ifndef MULTIFLOWDOCKAPI_MULTIFLOWDOCK_H
#define MULTIFLOWDOCKAPI_MULTIFLOWDOCK_H

#include <pthread.h>
#include <vector>
#include <string>
#include "MultiFlowdockAPI.h"
#include "FlowdockAPI/FlowdockAPI.h"
#include "Library.h"

class MultiFlowdock
{
public:
   MultiFlowdock();
   ~MultiFlowdock();

   bool AddFlow(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);
   std::string GetListenMessage(bool bDelete);

protected:
   RLibrary m_libFlowdockAPI;
   std::vector<FlowdockAPI> m_aFlows;
};


#endif
