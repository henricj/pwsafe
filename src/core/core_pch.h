#ifndef PCH_H
#define PCH_H


#ifdef WIN32

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

// The following is needed because gdiplustypes.h uses the Windows macros
// min & max but these "can" cause the error "C3861: 'min': identifier not found"
// when building under VS2015.
// However, although the code below prevents this, it REQUIRES the use
// of std::min & std::max everywhere.  As not macros, these also require
// the arguments to be the same type e.g. "int, int" or "long, long" etc.
// unlike the Windows macros min & max.
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
#include <random>
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

#ifdef PWS_UI_MFC

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

// Don't show warning for automatic inline conversion
#pragma warning(disable: 4711)

// Don't show warning for "identifier was truncated to '255' characters" in STL.
#pragma warning(disable: 4786)

// Ensure that switch enum statements without a "default" case statement catch all
// possible enum values
#pragma warning(error: 4062)

#elif defined(_WIN32)
#include <Windows.h>
#endif // PWS_UI_MFC

#endif // PCH_H

