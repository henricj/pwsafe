set (RESPWSL_SRCS
    ResPWSL.cpp
    StdAfx.cpp
    StringFileInfo.cpp
    StringTable.cpp
    VersionInfo.cpp
    VersionInfoBuffer.cpp
    VersionInfoString.cpp
)

set (RESPWSL_RES ResPWSL.rc)

add_executable( ResPWSL ${RESPWSL_SRCS} ${RESPWSL_RES})
target_link_libraries( ResPWSL PRIVATE pws_mfc_interface harden_interface )

set_property( TARGET ResPWSL
              PROPERTY FOLDER Tools )
              
target_compile_definitions(ResPWSL PRIVATE 
              _CRT_SECURE_NO_WARNINGS 
              _CRT_NON_CONFORMING_SWPRINTFS)

if (USE_PRECOMPILED_HEADERS)
  target_precompile_headers(ResPWSL PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.h>")
endif ()
