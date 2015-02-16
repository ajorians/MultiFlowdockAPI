#ifndef MULTIFLOWDOCKAPI_MULTIFLOWDOCKAPI_H
#define MULTIFLOWDOCKAPI_MULTIFLOWDOCKAPI_H

#ifdef WIN32
#define MULTIFLOWDOCK_EXTERN	extern "C" __declspec(dllexport)
#else
#define MULTIFLOWDOCK_EXTERN extern "C"
#endif

typedef void*	MultiFlowdockAPI;

typedef int (*MultiFlowdockCreateFunc)(MultiFlowdockAPI* api);
typedef int (*MultiFlowdockFreeFunc)(MultiFlowdockAPI* api);

typedef int (*MultiFlowdockAddFlowFunc)(MultiFlowdockAPI api, const char* pstrOrg, const char* pstrFlow, const char* pstrUsername, const char* pstrPassword);
typedef int (*MultiFlowdockGetMessageFunc)(MultiFlowdockAPI api, char* pstrMessage, int& nSizeOfMessage, int nDelete);

MULTIFLOWDOCK_EXTERN int MultiFlowdockCreate(MultiFlowdockAPI* api);
MULTIFLOWDOCK_EXTERN int MultiFlowdockFree(MultiFlowdockAPI* api);

MULTIFLOWDOCK_EXTERN int MultiFlowdockAddFlow(MultiFlowdockAPI api, const char* pstrOrg, const char* pstrFlow, const char* pstrUsername, const char* pstrPassword);
MULTIFLOWDOCK_EXTERN int MultiFlowdockGetMessage(MultiFlowdockAPI api, char* pstrMessage, int& nSizeOfMessage, int nDelete);

#endif
