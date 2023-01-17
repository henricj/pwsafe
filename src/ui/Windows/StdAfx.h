#pragma once

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

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
#include <atomic>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <climits>
#include <clocale>
#include <cmath>
#include <codecvt>
#include <condition_variable>  // NOLINT
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <ostream>  // NOLINT
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef PWS_UI_MFC

namespace Gdiplus {
  using std::min;
  using std::max;
};


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

// Save including it everywhere!
#include "Windowsdefs.h"
#elif defined(_WIN32)
#include <Windows.h>
#endif // PWS_UI_MFC
