/*
* Copyright (c) 2003-2023 Rony Shapiro <ronys@pwsafe.org>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/

/**
* \file Windows-specific implementation of media.h
*/

#include "../media.h"
#include "../debug.h"
#include <Urlmon.h>

// Windows uses the file extension to get the Media/MIME type

stringT pws_os::GetMediaType(const stringT &sfilename)
{
  HRESULT hResult;
  TCHAR *pwzMimeOut = NULL;
  stringT sMediaType(_T("unknown"));

  wchar_t extn[_MAX_EXT];
  _tsplitpath_s(sfilename.c_str(), NULL, 0, NULL, 0, NULL, 0, extn, _MAX_EXT);

  DWORD dwMimeFlags = FMFD_URLASFILENAME | FMFD_IGNOREMIMETEXTPLAIN | FMFD_RETURNUPDATEDIMGMIMES;
  hResult = FindMimeFromData(NULL, extn, NULL, 0, NULL, dwMimeFlags, &pwzMimeOut, 0);

  if (SUCCEEDED(hResult)) {
    sMediaType = pwzMimeOut;
    CoTaskMemFree(pwzMimeOut);
  }

  return sMediaType;
}
