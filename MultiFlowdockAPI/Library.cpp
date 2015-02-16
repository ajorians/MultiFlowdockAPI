#include "Library.h"

#ifdef WIN32
#include <atlstr.h>//For CString
#endif

#include <cassert>

RLibrary::RLibrary()
{
}

RLibrary::RLibrary(const std::string &strFilename)
: m_strFilename(strFilename)
{
}

void RLibrary::SetLibrary(const std::string &strFilename)
{
   m_strFilename = strFilename;
}

bool RLibrary::Load()
{
   assert(!m_strFilename.empty());
#ifdef WIN32
	m_hModule = LoadLibraryW(CStringW(m_strFilename.c_str()));
#else
	m_hModule = dlopen(m_strFilename.c_str(), RTLD_LAZY);
#endif

	return m_hModule != NULL;
}

void* RLibrary::Resolve(const char* pstrExport)
{
	void *ptrRet;

#ifdef WIN32
	ptrRet = GetProcAddress(m_hModule, pstrExport);
#else
	ptrRet = dlsym(m_hModule, pstrExport);
#endif

	return ptrRet;
}



