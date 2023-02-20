cmake_minimum_required (VERSION 3.13...3.25) # 3.13 is used by Debian 10

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

set(PWSAFE_LANGUAGES CXX C)

if (APPLE)
  list(APPEND PWSAFE_LANGUAGES OBJCXX OBJC)
endif ()

project (passwordsafe
  LANGUAGES ${PWSAFE_LANGUAGES}
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# wxWidgets 3.2.1 doesn't compile under C++17 on some distros.
# 3.0.5 is fine, though...
# Need to revisit this and remove when no longer an issue
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD 17)

########
# prerequisites
include(CheckLibraryExists)
include(CheckIncludeFile)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
  option(USE_PRECOMPILED_HEADERS "Precompile headers" ON)
endif()

# IPO = interprocedural optimisation, not initial public offering (unfortunately)
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

if (IPO_SUPPORTED)
  option(USE_INTERPROCEDURAL_OPTIMIZATION "Enable interprocedural optimization (LTO/LTCG)" ON)

  if (USE_INTERPROCEDURAL_OPTIMIZATION)
    message(STATUS "Interprocedural optimization (LTO/LTCG) is enabled.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

    # Propagate IPO settings to subdirectories
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
  endif ()
endif (IPO_SUPPORTED)

if (UNIX)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif ()

if (NOT WIN32)
  include(FindUnixCommands)
endif (NOT WIN32)

########
# Configurable options:
option (NO_YUBI "Set ON to disable YubiKey support" OFF)
option (NO_GTEST "Set ON to disable gtest unit testing" OFF)
option (GTEST_BUILD "Set OFF to disable gtest download and build on-fly" ON)

set(XML_FRAMEWORK_OPTIONS XercesC None)

add_library (pws_ui_interface INTERFACE)
set (PWS_LIBS pws_ui_interface)

if (WIN32)
  option(PWS_MSVC_STATIC_RUNTIME "Use the static MSVC C++ runtime" ON)

  add_library(pws_mfc_interface INTERFACE)

  if (NOT PWS_MSVC_STATIC_RUNTIME)
    target_compile_definitions(pws_mfc_interface INTERFACE _AFXDLL)
  endif ()

  set(PWS_UI_FRAMEWORK "MFC" CACHE STRING "UI Framework")
  set_property(CACHE PWS_UI_FRAMEWORK PROPERTY STRINGS MFC WX)

  if(PWS_UI_FRAMEWORK STREQUAL "MFC")
    set (PWS_UI_MFC TRUE)
    target_compile_definitions (pws_ui_interface INTERFACE PWS_UI_MFC)
    target_link_libraries(pws_ui_interface INTERFACE pws_mfc_interface)

    set (DEFAULT_XML_FRAMEWORK "MSXML")
    list (APPEND XML_FRAMEWORK_OPTIONS MSXML)
  elseif (PWS_UI_FRAMEWORK STREQUAL "WX")
    set(PWS_UI_WX TRUE)
    target_compile_definitions (pws_ui_interface INTERFACE PWS_UI_WX)

    set (DEFAULT_XML_FRAMEWORK "XercesC")
  endif ()
else (WIN32)
  set(PWS_UI_FRAMEWORK "WX")
  set(PWS_UI_WX TRUE)
  set (DEFAULT_XML_FRAMEWORK "XercesC")
endif (WIN32)

message (STATUS "Using UI framework: ${PWS_UI_FRAMEWORK}")

set(PWS_XML_FRAMEWORK "${DEFAULT_XML_FRAMEWORK}" CACHE STRING "XML Framework")
set_property(CACHE PWS_XML_FRAMEWORK PROPERTY STRINGS ${XML_FRAMEWORK_OPTIONS})

message(STATUS "Using XML framework: ${PWS_XML_FRAMEWORK}")

add_library (pws_xml_interface INTERFACE)
list (APPEND PWS_LIBS pws_xml_interface)

# Set XML framework flags
if (PWS_XML_FRAMEWORK STREQUAL "MSXML")
  set(PWS_XML_MSXML TRUE)
  target_compile_definitions (pws_xml_interface INTERFACE PWS_XML_MSXML)
elseif (PWS_XML_FRAMEWORK STREQUAL "XercesC")
  set(PWS_XML_XERCESC TRUE)
endif ()

if (PWS_UI_WX AND PWS_XML_MSXML)
  message(FATAL_ERROR "MSXML cannot be used with wxWidgets.")
endif ()

if (PWS_UI_WX)
  add_library(pws_qr INTERFACE)
  list(APPEND PWS_LIBS pws_qr)

  set(NO_QR_DEFAULT ON)
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NO_QR_DEFAULT OFF)
  endif ()
  option (NO_QR "Set ON to disable QR support" ${NO_QR_DEFAULT})
  if (NOT NO_QR)
    check_include_file("qrencode.h" HAVE_LIBQRENCODE_DEV)
    if(NOT HAVE_LIBQRENCODE_DEV)
      unset(HAVE_LIBQRENCODE_DEV CACHE)
      message( FATAL_ERROR "libqrencode-dev(el) not installed: install or select NO_QR")
    endif(NOT HAVE_LIBQRENCODE_DEV)

    target_link_libraries(pws_qr INTERFACE qrencode)
  else (NOT NO_QR)
    target_compile_definitions(pws_qr INTERFACE NO_QR)
  endif (NOT NO_QR)
endif(PWS_UI_WX)

# TODO: Add a compile check to see if we actually have ASAN support.
# Just looking at the platform and compiler isn't enough.
option (USE_ASAN "Set ON to enable the address sanitizer (ASAN)" OFF)

option(USE_UBSAN "Set ON to enable the undefined behavior sanitizer (UBSAN)" OFF)

if (USE_UBSAN)
  include(ubsan-setup)
endif()


option(USE_UBSAN "Set ON to enable the undefined behavior sanitizer (UBSAN)" OFF)

if (USE_UBSAN)
  include(ubsan-setup)
endif()

include(create-harden-interface)
list (APPEND PWS_LIBS harden_interface)

include(pws-version)

########
# Find packages

if (PWS_UI_WX)
  if (NOT WIN32)
     # help people with wxWidgets on non-standard installation
     # quick-and-dirty since wxWdigets cmake's support is not yet loaded.
     find_program(PWSHINT_wxconfig wx-config)
     if (NOT PWSHINT_wxconfig)
       message(STATUS
               "Failed to find 'wx-config' executable:\n"
               "   Tip: can be pointed from command-line this way:\n"
               "        $ cmake -D wxWidgets_CONFIG_EXECUTABLE=/path/to/wx-config ."
               )
     endif (NOT PWSHINT_wxconfig)
  endif (NOT WIN32)

  find_package(wxWidgets COMPONENTS adv aui base core html net REQUIRED)
  include("${wxWidgets_USE_FILE}")

  add_library(wxWidgetsInterface INTERFACE)

  target_link_libraries(wxWidgetsInterface INTERFACE ${wxWidgets_LIBRARIES})
  target_link_directories(wxWidgetsInterface INTERFACE ${wxWidgets_LIBRARY_DIRS})
  target_include_directories(wxWidgetsInterface INTERFACE ${wxWidgets_INCLUDE_DIRS})
  target_compile_definitions(wxWidgetsInterface INTERFACE PWS_UI_WX __WX__ ${wxWidgets_DEFINITIONS})

  target_link_libraries (pws_ui_interface INTERFACE wxWidgetsInterface)

  target_compile_definitions(pws_ui_interface INTERFACE PWS_UI_WX)
elseif (PWS_UI_MFC)
  target_compile_definitions(pws_ui_interface INTERFACE PWS_UI_MFC)
endif ()

if(NOT WIN32)
  find_package(OpenSSL REQUIRED)
  list (APPEND PWS_LIBS OpenSSL::Crypto)

  find_package(Magic REQUIRED)
  list (APPEND PWS_LIBS Magic::Magic)
endif()

if (UNIX)
  if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    include_directories("/usr/local/include")
  endif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

  check_include_file("uuid/uuid.h" UUID_H)
  if (NOT UUID_H)
    message(FATAL_ERROR
      "uuid.h not found - uuid-dev / libuuid-devel not installed?")
  endif(NOT UUID_H)
endif (UNIX)

add_library(YubikeyInterface INTERFACE)
list (APPEND PWS_LIBS YubikeyInterface)

if (NO_YUBI)
  target_compile_definitions(YubikeyInterface INTERFACE NO_YUBI)
  set(PWS_YUBI FALSE)
else ()
  set(PWS_YUBI TRUE)

  if (WIN32)
    target_link_libraries (YubikeyInterface INTERFACE YkLib22)
  else (WIN32)
    CHECK_LIBRARY_EXISTS(ykpers-1 yk_init "ykpers-1/ykpers.h" HAVE_YKPERS_H)

    if (HAVE_YKPERS_H)
      target_include_directories(YubikeyInterface INTERFACE /usr/include/ykpers-1)
      target_link_libraries(YubikeyInterface INTERFACE ykpers-1)
    else ()
      target_compile_definitions(YubikeyInterface INTERFACE NO_YUBI)
      set(PWS_YUBI FALSE)
      message(STATUS "Yubikey support disabled")
    endif()
  endif (WIN32)
endif()

if (PWS_XML_XERCESC)
  find_package (XercesC REQUIRED)

  target_compile_definitions (pws_xml_interface INTERFACE USE_XML_LIBRARY=XERCES PWS_XML_XERCESC WCHAR_INCOMPATIBLE_XMLCH)

  target_link_libraries (pws_xml_interface INTERFACE XercesC::XercesC)
endif (PWS_XML_XERCESC)

if (PWS_XML_MSXML)
  add_library(msxmlInterface INTERFACE)

  target_compile_definitions(msxmlInterface INTERFACE USE_XML_LIBRARY=MSXML PWS_XML_MSXML)
  target_link_libraries(msxmlInterface INTERFACE msxml6)

  target_link_libraries (pws_xml_interface INTERFACE msxmlInterface)
endif (PWS_XML_MSXML)

if (USE_ASAN AND NOT MSVC)
  # Requires libasan, which I can't get CHECK_LIBRARY_EXISTS to find under Debian
  CHECK_LIBRARY_EXISTS("asan" "" "" HAS_ASAN)
  CHECK_LIBRARY_EXISTS("asan" "" "/usr/lib/x86_64-linux-gnu/" HAS_ASAN1)
  if (NOT HAS_ASAN AND NOT HAS_ASAN1)
    message (WARNING
      "-fsanitize=address requested but libasan not found.")
  endif (NOT HAS_ASAN AND NOT HAS_ASAN1)
endif (USE_ASAN AND NOT MSVC)

# Common configurations

include_directories(src)

add_compile_definitions(
  __STDC_WANT_LIB_EXT1__=1
  UNICODE
  $<$<CONFIG:DEBUG>:_DEBUG>
  $<$<NOT:$<CONFIG:DEBUG>>:NDEBUG>
)

# Assume that we're either MSVC or a Unix-like
if (MSVC)
  # Debug build looks for dlls with _D postfix, this provides it:
  set (CMAKE_DEBUG_POSTFIX "_D")
  # This copies dlls to same directory as exec upon 'install' build
  set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")

  set_property (GLOBAL PROPERTY USE_FOLDERS ON)

  include(set-options-msvc)
elseif (APPLE)
  include(set-options-macos)
else ()
  add_compile_definitions(Wall)

  if (USE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-4 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address")
  endif (USE_ASAN)
endif ()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

if(HAVE_YKPERS_H)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/ykpers-1")
  list(APPEND CMAKE_REQUIRED_LIBRARIES ykpers-1)
endif(HAVE_YKPERS_H)

if (NOT MSVC)
   # Following is because (a) -O3 breaks the test and application, and
   # (b) -O3 is the default for cmake
   set (CMAKE_CXX_FLAGS_RELEASE -O2)
endif (NOT MSVC)

if (WIN32)
   set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif (WIN32)

# The global compiler and linker options should all have been set by now.

enable_testing()

# Subdirectories

add_subdirectory (src/os) # os-specific library
add_subdirectory (src/core) # core library
add_subdirectory(src/ui/cli "./cli")
add_subdirectory (help) # online help

if (PWS_UI_MFC)
  include(CheckCXXSourceRuns)
  # See if we can run what we're compiling.
  check_cxx_source_runs("int main() { return 0; }" PWS_CAN_RUN_BINARIES)

  if (PWS_CAN_RUN_BINARIES)
    add_subdirectory (src/Tools/Windows/I18N/ResText)
    add_subdirectory (src/Tools/Windows/I18N/ResPWSL)
  endif ()
  add_subdirectory (src/ui/Windows)
  if (PWS_CAN_RUN_BINARIES)
    add_subdirectory (src/ui/Windows/I18N)
  endif ()
elseif (PWS_UI_WX)
  add_subdirectory (src/ui/wxWidgets)

  target_include_directories (pws_ui_interface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/wxWidgets")

  find_package(Gettext QUIET)

  if (GETTEXT_FOUND)
    if (NOT GETTEXT_XGETTEXT_EXECUTABLE)
        get_filename_component(GETTEXT_DIR GETTEXT_MSGFMT_EXECUTABLE DIRECTORY)
        find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext HINT "${GETTEXT_DIR}")
    endif ()

    if (GETTEXT_XGETTEXT_EXECUTABLE)
      add_subdirectory (src/ui/wxWidgets/I18N)
    endif ()
  endif ()
endif ()

add_executable(pwsafe)
target_sources(pwsafe PRIVATE ${PWSAFE_SRCS} ${PWSAFE_HDRS})
target_link_libraries(pwsafe PRIVATE core os ${PWS_LIBS})
target_include_directories(pwsafe PRIVATE "${PROJECT_BINARY_DIR}")

if (WIN32)
  set_target_properties(pwsafe PROPERTIES WIN32_EXECUTABLE ON)

  target_compile_options(pwsafe PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/GA>")

  if (PWS_UI_MFC)
    if (PWS_CAN_RUN_BINARIES)
      add_dependencies (pwsafe language-dlls) # i18n dlls
    endif ()
    target_link_libraries (pwsafe PRIVATE pws_at pws_osk)
    #target_compile_definitions(pwsafe PRIVATE _BIND_TO_CURRENT_CRT_VERSION;_BIND_TO_CURRENT_MFC_VERSION)

    target_link_options(pwsafe PRIVATE /ENTRY:wWinMainCRTStartup /MAP)
  endif (PWS_UI_MFC)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # squash lintian hardening-no-relro warning, probably good for all Linuxen
  set_target_properties(pwsafe PROPERTIES LINK_FLAGS "-z relro")
endif ()

if (USE_PRECOMPILED_HEADERS)
  if (PWS_UI_MFC)
    target_precompile_headers(pwsafe PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/ui/Windows/StdAfx.h>")
  elseif (PWS_UI_WX)
    target_precompile_headers(pwsafe PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/ui/wxWidgets/wx_pch.h>")
  endif ()
endif ()

target_link_libraries(pwsafe PRIVATE pws_version)

if (APPLE)
  find_library(QUARTZCORE_LIBS QuartzCore)
  target_link_libraries(pwsafe PRIVATE ${QUARTZCORE_LIBS} curl)
elseif (WIN32)
  target_link_libraries(pwsafe PRIVATE ${PWS_DLL_LIBS}
    version Rpcrt4 htmlhelp SetupAPI Hid usp10 Dbghelp Urlmon gdiplus)
else ()
  target_link_libraries(pwsafe PRIVATE uuid Xtst X11 curl)
endif()

if (NOT NO_GTEST)
  if (GTEST_BUILD)
    # Download and unpack googletest at configure time
    # See https://crascit.com/2015/07/25/cmake-gtest/
    configure_file(Misc/CMakeLists.gtest.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
    execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # Set some other gtest configurations:
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

    # Add googletest directly to our build. This adds
    # the following targets: gtest, gtest_main, gmock
    # and gmock_main
    add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                      "${CMAKE_BINARY_DIR}/googletest-build")

    unset(CMAKE_POLICY_DEFAULT_CMP0069)

#    include_directories("${gtest_SOURCE_DIR}/include"
  #                       "${gmock_SOURCE_DIR}/include"
 #                       )
  endif(GTEST_BUILD)

  add_subdirectory (src/test) # tests (gtest framework)
endif(NOT NO_GTEST)

# Installation stuff (for 'make install', but mainly for 'make package')
install (TARGETS pwsafe RUNTIME DESTINATION "bin")

if (NOT WIN32)
  install (DIRECTORY xml DESTINATION "share/passwordsafe")
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/pwsafe.1.gz
                     COMMAND ${GZIP} ARGS -9 -n -c ${PROJECT_SOURCE_DIR}/docs/pwsafe.1 > pwsafe.1.gz
                     WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  ADD_CUSTOM_TARGET(manpage ALL DEPENDS ${CMAKE_BINARY_DIR}/pwsafe.1.gz)
  install (FILES ${CMAKE_BINARY_DIR}/pwsafe.1.gz DESTINATION "share/man/man1") 
  install (FILES "install/desktop/pwsafe.desktop" DESTINATION "share/applications")
  install (FILES "install/graphics/pwsafe.png" DESTINATION "share/icons/hicolor/48x48/apps/")
  install (DIRECTORY "${CMAKE_BINARY_DIR}/src/ui/wxWidgets/I18N/mos/"
           DESTINATION "share/locale")

  install (DIRECTORY ${PROJECT_BINARY_DIR}/help/
           DESTINATION "share/passwordsafe/help"
           FILES_MATCHING PATTERN "*.zip"
           PATTERN "CMakeFiles" EXCLUDE)

else ()
  install (FILES "xml/pwsafe.xsd" DESTINATION "bin")
  install (FILES "xml/pwsafe_filter.xsd" DESTINATION "bin")
endif (NOT WIN32)

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/install/cmake_uninstall.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
      IMMEDIATE @ONLY)

  add_custom_target(uninstall
      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


###################################################################################################
#
# P A C K A G I N G
#
###################################################################################################

#
# Major version number of software version (e.g. 1.08-BETA).
#                                                ^
set (CPACK_PACKAGE_VERSION_MAJOR "${pwsafe_VERSION_MAJOR}")

#
# Minor version number of software version (e.g. 1.08-BETA), which should consist of two digits.
#                                                   ^
string(LENGTH ${pwsafe_VERSION_MINOR} VERSION_MINOR_LENGTH)
if (VERSION_MINOR_LENGTH GREATER 1)
  set (CPACK_PACKAGE_VERSION_MINOR "${pwsafe_VERSION_MINOR}")
else ()
  set (CPACK_PACKAGE_VERSION_MINOR "0${pwsafe_VERSION_MINOR}")
endif ()

#
# Mandatory part is complete. What follows is optional.
#
set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

#
# Revision number of software version (e.g. 1.08.1-BETA).
#                                                ^
if (pwsafe_REVISION)
  set (CPACK_PACKAGE_VERSION_PATCH "${pwsafe_REVISION}")
  set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
endif (pwsafe_REVISION)

#
# Special build tag of software version (e.g. 1.08.1-BETA).
#                                                    ^
if (pwsafe_SPECIALBUILD)
  set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${pwsafe_SPECIALBUILD}")
endif (pwsafe_SPECIALBUILD)

#
# Miscellaneous package information.
#
set (CPACK_PACKAGE_CONTACT "Rony Shapiro <ronys@pwsafe.org>")
set (CPACK_PACKAGE_VENDOR "Rony Shapiro")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Password Safe is a password database utility.")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/install/description.txt")
set (CPACK_STRIP_FILES "TRUE")

#
# Package file name is project name.
#
if (NOT CPACK_PACKAGE_NAME)
  string(TOLOWER "${PROJECT_NAME}" CPACK_PACKAGE_NAME)
endif ()

#
# Determine the architecture (amd64, i386, etc.) with command 'dpkg --print-architecture
#
if (NOT WIN32)
if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
      find_program(DPKG_EXECUTABLE dpkg)
      mark_as_advanced(DPKG_EXECUTABLE)

      if (NOT DPKG_EXECUTABLE)
        find_program(UNAME_EXECUTABLE uname)
        mark_as_advanced(UNAME_EXECUTABLE)
        if (NOT UNAME_EXECUTABLE)
          set (PACKAGE_ARCHITECTURE "")
        else ()
          execute_process(COMMAND "${UNAME_EXECUTABLE}" -m
                         OUTPUT_VARIABLE PACKAGE_ARCHITECTURE)
          string(PREPEND PACKAGE_ARCHITECTURE "-")
          set (PACKAGE_ARCHITECTURE "")
        endif ()
      else ()
        execute_process(COMMAND "${DPKG_EXECUTABLE}" --print-architecture
                        OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        set (PACKAGE_ARCHITECTURE "-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
      endif ()
    endif ()
endif (NOT WIN32)

#
# Determine name of distribution (Ubuntu, LinuxMint, ...)
#

# Find out what linux distro we're on... currently only used for 
# debian based distros
find_program(LSB_RELEASE_EXECUTABLE lsb_release
  DOC "Linux Standard Base and Distribution command line query client")
mark_as_advanced(LSB_RELEASE_EXECUTABLE)

if (LSB_RELEASE_EXECUTABLE)
  execute_process(COMMAND ${LSB_RELEASE_EXECUTABLE} -is
    OUTPUT_VARIABLE RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  execute_process(COMMAND ${LSB_RELEASE_EXECUTABLE} -rs
    OUTPUT_VARIABLE RELEASE_NUM_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(LSB_RELEASE_EXECUTABLE) # look for /etc/os-release
  set (OS_RELEASE_FILE "/etc/os-release")
  if (EXISTS ${OS_RELEASE_FILE})
    file ( STRINGS ${OS_RELEASE_FILE} OS_RELEASE_LIST )
    foreach ( OS_RELEASE_ITEM IN ITEMS ${OS_RELEASE_LIST} )
      string (REGEX REPLACE "\;" "" OS_RELEASE_ITEM ${OS_RELEASE_ITEM})
      if (${OS_RELEASE_ITEM} MATCHES "^[ ]*ID[ ]*=[ ]*(.+)")
        string (REGEX REPLACE ".*=[ ]*(.+)" "\\1"
          RELEASE_ID_SHORT ${OS_RELEASE_ITEM})
      elseif (${OS_RELEASE_ITEM} MATCHES "^[ ]*VERSION_ID[ ]*=[ ]*([0-9]+)")
        string (REGEX REPLACE ".*=[ ]*([0-9]+)" "\\1"
          RELEASE_NUM_SHORT ${OS_RELEASE_ITEM})
      endif ()
      endforeach (OS_RELEASE_ITEM)
  endif (EXISTS ${OS_RELEASE_FILE})
endif(LSB_RELEASE_EXECUTABLE)

if (RELEASE_ID_SHORT)
  string(TOLOWER ${RELEASE_ID_SHORT} DISTRO_NAME)
endif (RELEASE_ID_SHORT)

#
# Determine major distroversion number from output (e.g. '18.04') of command 'lsb_release -rs'
# or from VERSION_ID of /etc/os-release
#
# Get the first number up to the separator ('.' or '-').
# The remaining characters doesn't matter.
#
if (${RELEASE_NUM_SHORT} MATCHES "^([0-9]+)([\\.\\-]*.)*$")
  if (CMAKE_MATCH_COUNT GREATER 0)
    set (DISTRO_MAJOR_VERSION ${CMAKE_MATCH_1})
  else ()
    set (DISTRO_MAJOR_VERSION ${RELEASE_NUM_SHORT})
  endif ()
else ()
  set (DISTRO_MAJOR_VERSION ${RELEASE_NUM_SHORT})
endif ()

#
# Build name of package file (e.g. passwordsafe-ubuntu18-1.08.1-BETA-amd64.deb)
#
set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${DISTRO_NAME}${DISTRO_MAJOR_VERSION}-${CPACK_PACKAGE_VERSION}${PACKAGE_ARCHITECTURE})

# Post-build stuff: e.g., sign the package
set(CPACK_POST_BUILD_SCRIPTS "${PROJECT_SOURCE_DIR}/CMakeModules/PostBuild.cmake")

if (EXISTS "/etc/debian_version")
## we are on a debian based distro, but we can't make assumptions about which
## we will do general deb related setup and relegate deps to specific distros 

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/changelog.Debian.gz
                   COMMAND /bin/gzip ARGS -9 -n -k 
                   "${PROJECT_SOURCE_DIR}/install/deb/changelog.Debian"
                   COMMAND /bin/mv ARGS 
                   "${PROJECT_SOURCE_DIR}/install/deb/changelog.Debian.gz" 
                   ${CMAKE_BINARY_DIR}/changelog.Debian.gz)

ADD_CUSTOM_TARGET(changelog ALL DEPENDS ${CMAKE_BINARY_DIR}/changelog.Debian.gz)
install (FILES ${CMAKE_BINARY_DIR}/changelog.Debian.gz 
         DESTINATION "share/doc/passwordsafe")
install (FILES "install/deb/copyright.debian" DESTINATION "share/doc/passwordsafe"
         RENAME "copyright")

set (CPACK_DEBIAN_PACKAGE_SECTION "utils")
set (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://pwsafe.org/")
set (CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION "TRUE")
set (CPACK_DEBIAN_PACKAGE_CONFLICTS "passwordsafe-common") # Debian package maintainer split stuff

if (${RELEASE_ID_SHORT} STREQUAL "Ubuntu")
  ## Ubuntu-specific
  if (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "18.0")
    set (LIBCURL "libcurl4 (>= 7.58.0-2)")
  else (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "18.0")
    set (LIBCURL "libcurl3 (>= 7.47.0)")
  endif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "18.0")
  if (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "19.0")
    set (LIBQRENCODE "libqrencode4 (>= 4.0.2-1)")
  else (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "19.0")
    set (LIBQRENCODE "libqrencode3 (>= 3.4.4-1)")
  endif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "19.0")
  if (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "22.10")
    set (LIBWX  "libwxgtk3.2-0 (>= 3.2.0)")
  elseif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "20.0")
    set (LIBWX  "libwxgtk3.0-gtk3-0v5 (>= 3.0.4)")
  else (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "22.10")
    set (LIBWX  "libwxgtk3.0-0v5 (>= 3.0.0-2) | libwxgtk3.0-0 (>= 3.0.0-2)")
  endif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "22.10")
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.[0-9]+" "\\1.\\2" XercesC_VER ${XercesC_VERSION})
  set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.11.1-0ubuntu7.2), ${LIBCURL}, libmagic1 (>= 1:5.25), libuuid1 (>= 2.17.2-0ubuntu1), ${LIBWX}, libxtst6 (>= 2:1.1.0-2), libxerces-c${XercesC_VER} (>= 3.1.0-1), libykpers-1-1 (>= 1.7.0-1), ${LIBQRENCODE}")
endif (${RELEASE_ID_SHORT} STREQUAL "Ubuntu")

if ((${RELEASE_ID_SHORT} STREQUAL "Debian") OR (${RELEASE_ID_SHORT} STREQUAL "Raspbian"))
  ## Debian-specific:
  if (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "10")
    set (LIBCURL "libcurl4 (>= 7.64.0-4)")
    set (LIBXERCES "libxerces-c3.2 (>= 3.2.2+debian-1+b1)")
    set (LIBQRENCODE "libqrencode4 (>= 4.0.2-1)")
  else (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "10")
    set (LIBCURL "libcurl3 (>= 7.52.1-5)")
    set (LIBXERCES "libxerces-c3.1 (>= 3.1.1-1+b1)")
    set (LIBQRENCODE "libqrencode3 (>= 3.4.4-1+b2)")
  endif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "10")
  set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7-18lenny4), ${LIBCURL}, libmagic1 (>= 1:5.22), libuuid1 (>= 1.41.3-1),
                                     libwxgtk3.0-0 (>= 3.0.0-2) | libwxgtk3.0-0v5 (>= 3.0.0-2) | libwxgtk3.0-gtk3-0v5 (>= 3.0.5.1),
                                     libxtst6 (>= 2:1.0.3-1), ${LIBXERCES}, libykpers-1-1 (>= 1.7.0-1), ${LIBQRENCODE}")
  set (CPACK_DEBIAN_PACKAGE_RECOMMENDS "xvkbd (>= 3.3-1+b1)")
endif ((${RELEASE_ID_SHORT} STREQUAL "Debian") OR (${RELEASE_ID_SHORT} STREQUAL "Raspbian"))

if ((${RELEASE_ID_SHORT} STREQUAL "LinuxMint") OR (${RELEASE_ID_SHORT} STREQUAL "Linuxmint"))
  ## LinuxMint-specific:
  if (NOT ${RELEASE_NUM_SHORT} VERSION_LESS_EQUAL "19.3")
    set (LIBCURL "libcurl4 (>= 7.58.0)")
    set (LIBXERCES "libxerces-c3.2 (>= 3.1.0-1)")
    set (LIBQRENCODE "libqrencode4 (>= 4.0.2-1)")
    set (LIBWX  "libwxgtk3.0-gtk3-0v5 (>= 3.0.4)")
  elseif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "19.1")
    set (LIBCURL "libcurl4 (>= 7.58.0)")
    set (LIBXERCES "libxerces-c3.2 (>= 3.1.0-1)")
    set (LIBQRENCODE "libqrencode3 (>= 3.4.4-1)")
    set (LIBWX  "libwxgtk3.0-gtk3-0v5 (>= 3.0.4)")
  else (NOT ${RELEASE_NUM_SHORT} VERSION_LESS "19.1")
    set (LIBCURL "libcurl3 (>= 7.47.0)")
    set (LIBXERCES "libxerces-c3.1 (>= 3.1.3+debian-1)")
    set (LIBQRENCODE "libqrencode3 (>= 3.4.4)")
    set (LIBWX  "libwxgtk3.0-0v5 (>= 3.0.0-2) | libwxgtk3.0-0 (>= 3.0.0-2)")
  endif (NOT ${RELEASE_NUM_SHORT} VERSION_LESS_EQUAL "19.3")
  set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.11.1-0ubuntu7.2), ${LIBCURL}, libmagic1 (>= 1:5.25), libuuid1 (>= 2.17.2-0ubuntu1), ${LIBWX}, libxtst6 (>= 2:1.1.0-2), ${LIBXERCES}, libykpers-1-1 (>= 1.7.0), ${LIBQRENCODE}")
endif ((${RELEASE_ID_SHORT} STREQUAL "LinuxMint") OR (${RELEASE_ID_SHORT} STREQUAL "Linuxmint"))

set (CPACK_DEBIAN_PACKAGE_DESCRIPTION "Password Safe is a password database utility.\n Like many other such products, commercial and otherwise,\n it stores your passwords in an encrypted file.")

set (CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
  "${CMAKE_CURRENT_SOURCE_DIR}/install/deb/prerm;${CMAKE_CURRENT_SOURCE_DIR}/install/deb/postinst")
endif (EXISTS "/etc/debian_version")

## RPM-specific
if (RELEASE_ID_SHORT)
    if (${RELEASE_ID_SHORT} STREQUAL "openSUSE")
    ## openSUSE-specific
        set (RPM_PACKAGES "xerces-c, ykpers, qrencode, libjpeg8, libjpeg62, libmagic1, \
                         libwx_baseu-suse3_0_5, libwx_baseu_net-suse3_0_5, \
                         libwx_gtk2u_aui-suse3_0_5, libwx_gtk2u_core-suse3_0_5, \
                         libwx_gtk2u_html-suse3_0_5, wxWidgets-lang")
    elseif (${RELEASE_ID_SHORT} STREQUAL "fedora")
    ## Fedora-specific
        set (RPM_PACKAGES "wxBase3, wxGTK3, xerces-c, ykpers, qrencode-libs, file-libs")
    endif (${RELEASE_ID_SHORT} STREQUAL "openSUSE")
endif (RELEASE_ID_SHORT)

set (CPACK_RPM_PACKAGE_REQUIRES "${RPM_PACKAGES}")
set (CPACK_RPM_PACKAGE_SUGGESTS "xvkbd")
set (CPACK_RPM_PACKAGE_URL "https://pwsafe.org/")
set (CPACK_RPM_PACKAGE_LICENSE "Artistic2.0")
set (CPACK_RPM_PACKAGE_GROUP "Applications/Utils")
set (CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
     "/usr/share/man" "/usr/share/man/man1" "/usr/share/locale"
     "/usr/share/locale/da" "/usr/share/locale/da/LC_MESSAGES"
     "/usr/share/locale/de" "/usr/share/locale/de/LC_MESSAGES"
     "/usr/share/locale/es" "/usr/share/locale/es/LC_MESSAGES"
     "/usr/share/locale/fr" "/usr/share/locale/fr/LC_MESSAGES"
     "/usr/share/locale/it" "/usr/share/locale/it/LC_MESSAGES"
     "/usr/share/locale/ko" "/usr/share/locale/ko/LC_MESSAGES"
     "/usr/share/locale/nl" "/usr/share/locale/nl/LC_MESSAGES"
     "/usr/share/locale/pl" "/usr/share/locale/pl/LC_MESSAGES"
     "/usr/share/locale/ru" "/usr/share/locale/ru/LC_MESSAGES"
     "/usr/share/locale/sv" "/usr/share/locale/sv/LC_MESSAGES"
     "/usr/share/locale/zh" "/usr/share/locale/zh/LC_MESSAGES"
     "/usr/share/applications" "/usr/share/pixmaps"
     "/usr/share/icons/hicolor" "/usr/share/icons/hicolor/48x48" "/usr/share/icons/hicolor/48x48/apps"
     "/usr/share/icons" "/usr/share/icons/hicolor"
     "/usr/share/icons/hicolor/48x48" "/usr/share/icons/hicolor/48x48/apps"
    )
include(CPack)
### End of packaging section
### End of CMakeLists.txt
