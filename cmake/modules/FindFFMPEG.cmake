# - Try to find FFMPEG
# Once done this will define
#
#  FFMPEG_FOUND - system has FFMPEG
#  FFMPEG_INCLUDE_DIRS - the FFMPEG include directory
#  FFMPEG_LIBRARIES - Link these to use FFMPEG
#  FFMPEG_DEFINITIONS - Compiler switches required for using FFMPEG
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Copyright (c) 2010 Christophe Giboudeaux <cgiboudeaux@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # in cache already
  set(FFMPEG_FOUND TRUE)
else (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)

  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (NOT WIN32)
    include(FindPkgConfig)
    pkg_check_modules(_AVCODEC libavcodec)
    pkg_check_modules(_AVFORMAT libavformat)
    pkg_check_modules(_AVUTIL libavutil)
    #pkg_check_modules(_POSTPROC libpostproc)
    pkg_check_modules(_SWSCALE libswscale)

  endif (NOT WIN32)

  find_library(AVCODEC_LIBRARY
    NAMES
      avcodec
    PATHS
      ${_AVCODEC_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
    NO_DEFAULT_PATH
  )

  find_path(AVCODEC_INCLUDE_DIR
    NAMES
      avcodec.h
    PATHS
     ${_AVCODEC_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      libavcodec
      ffmpeg
    NO_DEFAULT_PATH
  )

  if(AVCODEC_LIBRARY AND AVCODEC_INCLUDE_DIR)
    set(AVCODEC_FOUND TRUE)
    mark_as_advanced(AVCODEC_LIBRARY AVCODEC_INCLUDE_DIR)
  endif(AVCODEC_LIBRARY AND AVCODEC_INCLUDE_DIR)

  find_library(AVFORMAT_LIBRARY
    NAMES
      avformat
    PATHS
      ${_AVFORMAT_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
    NO_DEFAULT_PATH
  )

  find_path(AVFORMAT_INCLUDE_DIR
    NAMES
      avformat.h
    PATHS
      ${_AVFORMAT_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      libavformat
      ffmpeg
    NO_DEFAULT_PATH
  )

  if(AVFORMAT_LIBRARY AND AVFORMAT_INCLUDE_DIR)
    set(AVFORMAT_FOUND TRUE)
    mark_as_advanced(AVFORMAT_LIBRARY AVFORMAT_INCLUDE_DIR)
  endif(AVFORMAT_LIBRARY AND AVFORMAT_INCLUDE_DIR)

  find_library(AVUTIL_LIBRARY
    NAMES
      avutil
    PATHS
      ${_AVUTIL_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
    NO_DEFAULT_PATH
  )
  mark_as_advanced(AVUTIL_LIBRARY)

  find_path(AVUTIL_INCLUDE_DIR
    NAMES
      avutil.h
    PATHS
      ${_AVUTIL_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      libavutil
      ffmpeg
    NO_DEFAULT_PATH
  )

  if(AVUTIL_LIBRARY AND AVUTIL_INCLUDE_DIR)
    set(AVUTIL_FOUND TRUE)
    mark_as_advanced(AVUTIL_LIBRARY AVUTIL_INCLUDE_DIR)
  endif(AVUTIL_LIBRARY AND AVUTIL_INCLUDE_DIR)

  find_library(SWSCALE_LIBRARY
    NAMES
      swscale
    PATHS
      ${_SWSCALE_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
    NO_DEFAULT_PATH
  )

  find_path(SWSCALE_INCLUDE_DIR
    NAMES
      swscale.h
    PATHS
      ${_SWSCALE_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      libswscale
      ffmpeg
    NO_DEFAULT_PATH
  )

  if(SWSCALE_LIBRARY AND SWSCALE_INCLUDE_DIR)
    set(SWSCALE_FOUND TRUE)
  mark_as_advanced(SWSCALE_LIBRARY SWSCALE_INCLUDE_DIR)
  endif(SWSCALE_LIBRARY AND SWSCALE_INCLUDE_DIR)

  #  find_library(POSTPROC_LIBRARY
  #    NAMES
  #      postproc
  #    PATHS
  #      ${_POSTPROC_LIBDIR}
  #      /usr/lib
  #      /usr/local/lib
  #      /opt/local/lib
  #      /sw/lib
  #    NO_DEFAULT_PATH
  #  )
  #  mark_as_advanced(POSTPROC_LIBRARY)

  #  find_path(POSTPROC_INCLUDE_DIR
  #    NAMES
  #      postprocess.h
  #    PATHS
  #      ${_POSTPROC_INCLUDEDIR}
  #      /usr/include/postproc
  #      /usr/local/include/postproc
  #      /opt/local/include/postproc
  #      /sw/include
  #    PATH_SUFFIXES
  #      libpostproc
  #      ffmpeg
  #  )
  #  mark_as_advanced(POSTPROC_INCLUDE_DIR)

  # if(POSTPROC_LIBRARY AND POSTPROC_INCLUDE_DIR)
  #   set(SWSCALE_FOUND TRUE)
  # mark_as_advanced(POSTPROC_LIBRARY POSTPROC_INCLUDE_DIR)
  # endif(POSTPROC_LIBRARY AND POSTPROC_INCLUDE_DIR)

  if(AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)
    set (FFMPEG_LIBRARIES
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY}
    )
    set(FFMPEG_INCLUDE_DIRS
        ${AVCODEC_INCLUDE_DIR}
        ${AVFORMAT_INCLUDE_DIR}
        ${AVUTIL_INCLUDE_DIR}
        ${SWSCALE_INCLUDE_DIR}
    )
    set(FFMPEG_FOUND TRUE)
    mark_as_advanced(FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)
  endif(AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)

endif (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
