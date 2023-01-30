include(CheckLibraryExists)

set (CORE_SRCS
  CheckVersion.cpp
  Command.cpp
  CoreImpExp.cpp
  CoreOtherDB.cpp
  ExpiredList.cpp
  ItemAtt.cpp
  Item.cpp
  ItemData.cpp
  ItemField.cpp
  Match.cpp
  PolicyManager.cpp
  PWCharPool.cpp
  PWHistory.cpp
  PWPolicy.cpp
  PWSAuxParse.cpp
  PWScore.cpp
  PWSdirs.cpp
  PWSfile.cpp
  PWSfileHeader.cpp
  PWSfileV1V2.cpp
  PWSfileV3.cpp
  PWSfileV4.cpp
  PWSFilters.cpp
  PWSLog.cpp
  PWSprefs.cpp
  PWSrand.cpp
  PWStime.cpp
  Report.cpp
  RUEList.cpp
  StringX.cpp
  SysInfo.cpp
  UnknownField.cpp
  UTF8Conv.cpp
  Util.cpp
  VerifyFormat.cpp
  XMLprefs.cpp
  crypto/AES.cpp
  crypto/BlowFish.cpp
  crypto/KeyWrap.cpp
  crypto/pbkdf2.cpp
  crypto/sha1.cpp
  crypto/sha256.cpp
  crypto/TwoFish.cpp
  XML/XMLFileHandlers.cpp
  XML/XMLFileValidation.cpp
  pugixml/pugixml.cpp
  )

if (PWS_XML_MSXML)
  list (APPEND CORE_SRCS
    XML/MSXML/MFileSAX2Handlers.cpp
    XML/MSXML/MFileValidator.cpp
    XML/MSXML/MFileXMLProcessor.cpp
    XML/MSXML/MFilterSAX2Handlers.cpp
    XML/MSXML/MFilterXMLProcessor.cpp
  )
endif (PWS_XML_MSXML)

if (PWS_XML_XERCESC)
  list (APPEND CORE_SRCS
    XML/Xerces/XFileSAX2Handlers.cpp
    XML/Xerces/XFileValidator.cpp
    XML/Xerces/XFileXMLProcessor.cpp
    XML/Xerces/XFilterSAX2Handlers.cpp
    XML/Xerces/XFilterXMLProcessor.cpp
    XML/Xerces/XSecMemMgr.cpp
   )
endif (PWS_XML_XERCESC)

if (NOT PWS_XML_MSXML AND NOT PWS_XML_XERCESC)
  list (APPEND CORE_SRCS
    XML/Pugi/PFileXMLProcessor.cpp
    XML/Pugi/PFilterXMLProcessor.cpp
  )
endif (NOT PWS_XML_MSXML AND NOT PWS_XML_XERCESC)

if (PWS_UI_WX)
  find_package(Perl REQUIRED)

  set (CORE_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/generated")

  file(MAKE_DIRECTORY "${CORE_GENERATED}")

  set (CORE_ST_CPP "${CORE_GENERATED}/core_st.cpp")
  set (CORE_ST_H "${CORE_GENERATED}/core_st.h")

  set (RC2CPP "${PROJECT_SOURCE_DIR}/Misc/rc2cpp.pl")

  add_custom_command (
    OUTPUT "${CORE_ST_CPP}" "${CORE_ST_H}"
    DEPENDS core.rc2
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/core.rc2" core.rc2
    COMMAND "${PERL_EXECUTABLE}" "${RC2CPP}" core.rc2
    COMMAND "${CMAKE_COMMAND}" -E rm core.rc2
    WORKING_DIRECTORY "${CORE_GENERATED}"
    VERBATIM
    )

  list (APPEND CORE_SRCS "${CORE_ST_CPP}")
elseif (PSW_UI_MFC)
  list (APPEND CORE_SRCS core.rc2)
endif ()

add_library(core STATIC)
target_sources(core PRIVATE ${CORE_SRCS})
target_link_libraries(core PRIVATE pws_ui_interface pws_xml_interface harden_interface)
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (CORE_GENERATED)
  target_include_directories(core PRIVATE "${CORE_GENERATED}")
endif ()

if (USE_PRECOMPILED_HEADERS)
    target_precompile_headers(core PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/core_pch.h>")
endif ()

