# - Try to find the Taglib library
# Once done this will define
#
#  TAGLIB_FOUND - system has the taglib library
#  TAGLIB_INCLUDE_DIRS - the taglib cflags
#  TAGLIB_LIBRARIES - The libraries needed to use taglib

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
# Copyright (c) 2011, Serebriyskiy Artem, <v.for.vandal@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(NOT TAGLIB_MIN_VERSION)
  set(TAGLIB_MIN_VERSION "1.4")
endif(NOT TAGLIB_MIN_VERSION)

if(NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PC_TAGLIB QUIET taglib)
    set(TAGLIB_DEFINITIONS ${PC_TAGLIB_CFLAGS_OTHER})
    set(TAGLIB_VERSION ${PC_TABLIG_VERSION})
endif(NOT WIN32)

#reset vars
set(TAGLIB_LIBRARIES)


include(FindLibraryWithDebug)
include(FindPackageHandleStandardArgs)

find_path(TAGLIB_INCLUDE_DIRS
NAMES
tag.h
PATH_SUFFIXES taglib
HINTS
${KDE4_INCLUDE_DIR}
${INCLUDE_INSTALL_DIR}
${PC_TAGLIB_INCLUDE_DIRS}

)

find_library_with_debug(TAGLIB_LIBRARIES
WIN32_DEBUG_POSTFIX d
NAMES tag
HINTS
${KDE4_LIB_DIR}
${LIB_INSTALL_DIR}
${PC_TAGLIB_LIBRARY_DIRS}
)

find_package_handle_standard_args(Taglib DEFAULT_MSG 
    TAGLIB_INCLUDE_DIRS TAGLIB_LIBRARIES)

mark_as_advanced(TAGLIB_INCLUDE_DIRS TAGLIB_LIBRARIES)

if(TAGLIB_FOUND)
  if(NOT Taglib_FIND_QUIETLY )
      message(STATUS "Taglib found: ${TAGLIB_LIBRARIES} Includes:  ${TAGLIB_INCLUDE_DIRS}")
  endif(NOT Taglib_FIND_QUIETLY )
else(TAGLIB_FOUND)
  if(Taglib_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Taglib")
  endif(Taglib_FIND_REQUIRED)
endif(TAGLIB_FOUND)

