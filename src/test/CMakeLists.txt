set (TEST_SRCS
  AESTest.cpp AliasShortcutTest.cpp FileV3Test.cpp ItemAttTest.cpp OSTest.cpp BlowFishTest.cpp
  FileV4Test.cpp ItemDataTest.cpp SHA256Test.cpp CommandsTest.cpp ItemFieldTest.cpp StringXTest.cpp
  coretest.cpp HMAC_SHA256Test.cpp KeyWrapTest.cpp TwoFishTest.cpp AuxParseTest.cpp UtilTest.cpp
  FileEncDecTest.cpp ImportTextTest.cpp)

if (WIN32)
  list (APPEND TEST_SRCS ../core/core.rc2)
  set_source_files_properties(../core/core.rc2 PROPERTIES LANGUAGE RC)
endif(WIN32)


# Setup test data
file (MAKE_DIRECTORY "data")
file (COPY "data/image1.jpg" DESTINATION "data")
file (COPY "data/text1.txt" DESTINATION "data")
file (COPY "data/import-text-unit-test1.txt" DESTINATION "data")
file (COPY "data/import-text-unit-test2.csv" DESTINATION "data")
file (COPY "data/import-text-unit-test3.csv" DESTINATION "data")
file (COPY "data/import-text-unit-test4.csv" DESTINATION "data")
file (COPY "data/import-text-unit-test5.csv" DESTINATION "data")

add_executable(coretest ${TEST_SRCS})
target_link_libraries(coretest PRIVATE gtest core os pws_ui_interface pws_xml_interface harden_interface)

if (WIN32)
  set_target_properties(coretest PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_RELEASE 1)
  target_link_libraries(coretest PRIVATE Rpcrt4)
elseif (APPLE)
  target_link_libraries(coretest PRIVATE pthread "-framework CoreFoundation")
else ()
  target_link_libraries(coretest PRIVATE uuid pthread Magic::Magic Xtst X11)
endif()

if (USE_PRECOMPILED_HEADERS)
    target_precompile_headers(coretest PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/coretest_pch.h>")
endif ()

add_test(NAME Coretests
  COMMAND coretest
  )
