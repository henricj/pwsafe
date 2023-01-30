include(CheckLibraryExists)

if (WIN32)
  set (OS_SRCS
    windows/cleanup.cpp
    windows/debug.cpp
    windows/dir.cpp
    windows/env.cpp
    windows/file.cpp
    windows/getopt.c
    windows/KeySend.cpp
    windows/lib.cpp
    windows/logit.cpp
    windows/media.cpp
    windows/mem.cpp
    windows//pws_str.cpp
    windows/rand.cpp
    windows/registry.cpp
    windows/run.cpp
    windows/sleep.cpp
    windows/utf8conv.cpp
    windows/UUID.cpp
    )
  if (PWS_YUBI)
      list(APPEND OS_SRCS
          windows/yubi/stdafx.cpp
          windows/yubi/YkLib.cpp)
  endif ()

  if (PWS_UI_MFC)
      add_subdirectory (windows/pws_autotype)
      add_subdirectory (windows/pws_osk)
  endif (PWS_UI_MFC)

  if (PWS_YUBI)
      add_subdirectory (windows/yubi)
  endif ()
elseif (APPLE)
  set (OS_SRCS
    mac/cleanup.cpp
    mac/debug.cpp
    mac/dir.cpp
    mac/env.cpp
    mac/file.cpp
    mac/KeySend.cpp
    mac/logit.cpp
    mac/macsendstring.cpp
    mac/mem.cpp
    mac/pws_str.cpp
    mac/pws_time.cpp
    mac/rand.cpp
    mac/registry.cpp
    mac/run.cpp
    mac/sleep.cpp
    mac/utf8conv.cpp
    mac/UUID.cpp
    mac/media.cpp
    )
elseif (UNIX)
  set (OS_SRCS
    unix/KeySend.cpp
    unix/UUID.cpp
    unix/cleanup.cpp
    unix/debug.cpp
    unix/dir.cpp
    unix/env.cpp
    unix/file.cpp
    unix/keyname.cpp
    unix/logit.cpp
    unix/media.cpp
    unix/mem.cpp
    unix/pws_str.cpp
    unix/pws_time.cpp
    unix/rand.cpp
    unix/registry.cpp
    unix/run.cpp
    unix/sleep.cpp
    unix/unicode2keysym.cpp
    unix/utf8conv.cpp
    unix/xsendstring.cpp
    )
  set (OS_YUBI_SRC unix/PWYubi.cpp)
else()
  message (FATAL_ERROR "Unsupported OS ${CMAKE_SYSTEM_NAME} - can't build OS library")
endif()

if(PWS_YUBI)
    list(APPEND OS_SRCS ${OS_YUBI_SRC})
endif()

add_library(os STATIC)
target_sources(os PRIVATE ${OS_SRCS})
target_include_directories(os INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# The Yubikey interface library will either define NO_YUBI or add the
# include path and actual link library.
target_link_libraries(os PRIVATE pws_ui_interface YubikeyInterface harden_interface)

if (WIN32 AND PWS_UI_WX)
  target_link_libraries(os PUBLIC urlmon)
endif ()

if (USE_PRECOMPILED_HEADERS)
    target_precompile_headers(os PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/os_pch.h>")
endif ()

