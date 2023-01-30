set (YUBI_SRCS
    YkLib.cpp
    stdafx.cpp
)

add_library(YkLib22 STATIC)
target_sources(YkLib22 PRIVATE ${YUBI_SRCS})
target_link_libraries(YkLib22 PRIVATE harden_interface)
