#
# FindMagic
# ---------
#
# Find the Magic header and library.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# none
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``Magic_FOUND``
#   True if Magic found.
#
# ``Magic_INCLUDE_DIRS``
#   Location of magic.h.
#
# ``Magic_LIBRARIES``
#   List of libraries when using Magic.
#
#
# Installation files of DEB package 'libmagic-dev'
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# /usr
# /usr/include
# /usr/include/file
# /usr/include/file/file.h
# /usr/include/magic.h
# /usr/lib
# /usr/lib/x86_64-linux-gnu
# /usr/lib/x86_64-linux-gnu/libmagic.a
# /usr/share
# /usr/share/doc
# /usr/share/doc/libmagic-dev
# /usr/share/doc/libmagic-dev/copyright
# /usr/share/man
# /usr/share/man/man3
# /usr/share/man/man3/libmagic.3.gz
# /usr/lib/x86_64-linux-gnu/libmagic.so
# /usr/share/doc/libmagic-dev/changelog.Debian.gz

#
# Locate header 'magic.h'
#
find_path(Magic_INCLUDE_DIR
  NAMES magic.h
  PATHS /usr/include /usr/local/include
)
mark_as_advanced(Magic_INCLUDE_DIR)

#
# Locate shared library 'libmagic'
#
find_library(Magic_LIBRARY
  NAMES magic
  PATHS /usr/lib /usr/lib64 /usr/local/lib
)
mark_as_advanced(Magic_LIBRARY)

find_package_handle_standard_args(Magic
  REQUIRED_VARS Magic_LIBRARY Magic_INCLUDE_DIR
)

if (Magic_FOUND)
  set(Magic_INCLUDE_DIRS ${Magic_INCLUDE_DIR})
  set(Magic_LIBRARIES ${Magic_LIBRARY})

  if (NOT TARGET Magic::Magic)
    add_library(Magic::Magic UNKNOWN IMPORTED)
    set_target_properties(Magic::Magic PROPERTIES
      IMPORTED_LOCATION "${Magic_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${Magic_INCLUDE_DIR}"
    )
  endif ()
endif ()
