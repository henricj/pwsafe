set (CLI_SRCS
  main.cpp
  search.cpp
  argutils.cpp
  searchaction.cpp
  strutils.cpp
  safeutils.cpp
  diff.cpp
  impexp.cpp)

set (CLI_TEST_SRC
  add-entry-test.cpp
  arg-fields-test.cpp
  split-test.cpp
  safeutils.cpp
  argutils.cpp
  searchaction.cpp
  strutils.cpp
  search-test.cpp
  search.cpp)

add_executable(pwsafe-cli)
target_sources(pwsafe-cli PRIVATE ${CLI_SRCS})
target_link_libraries(pwsafe-cli PRIVATE core os core pws_xml_interface pws_ui_interface harden_interface)

install (TARGETS pwsafe-cli RUNTIME DESTINATION "bin")

if (WIN32)
  target_sources(pwsafe-cli PRIVATE cli.rc)
  target_link_libraries(pwsafe-cli PRIVATE Rpcrt4)
  set_target_properties(pwsafe-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
else ()
  if (NOT APPLE)
    target_link_libraries(pwsafe-cli PRIVATE uuid)
  endif ()
  target_link_libraries (pwsafe-cli PRIVATE pthread Magic::Magic)
endif (WIN32)


if (USE_PRECOMPILED_HEADERS)
  target_compile_definitions(pwsafe-cli PRIVATE USE_PRECOMPILED_HEADER)
  target_precompile_headers(pwsafe-cli PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>")
endif ()

