#ifndef OS_PCH_H
#define OS_PCH_H

#if WIN32

#ifdef USE_MFC

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

// Show warnings
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif

#include <afxtempl.h>
#include <afxpriv.h>

#include <htmlhelp.h>
#include <afxdlgs.h>
#include <afxcontrolbars.h>

#include <afxole.h>

#include <atlimage.h>

#else // USE_MFC

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#endif // USE_MFC

#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif // WIN32

#include <algorithm>
#include <bitset>
#include <codecvt>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include <cassert>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#endif // OS_PCH_H
