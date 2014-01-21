# Create the library
FILE(GLOB codegen_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/Codegen.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/AudioStreamInput.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/Whitening.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/AudioBufferInput.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/MatrixUtility.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/Metadata.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/Fingerprint.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/Base64.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/SubbandAnalysis.cxx"
  )
FILE(GLOB codegen_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Find package taglib.
FIND_PATH(TagLib_INCLUDE_DIRS fileref.h
  PATH_SUFFIXES taglib
  DOC "Directory where TagLib headers are located"
  )
FIND_LIBRARY(TagLib_LIBRARY tag
  DOC "The TagLib library"
  )

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

INCLUDE_DIRECTORIES(SYSTEM
  ${TagLib_INCLUDE_DIRS}
  )

ADD_LIBRARY(codegen SHARED ${codegen_SRCS})
TARGET_LINK_LIBRARIES(codegen
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${TagLib_LIBRARY}
                      ${ZLIB_LIBRARIES}
                      )
SET_TARGET_PROPERTIES(codegen PROPERTIES
  PUBLIC_HEADER "${codegen_HEADERS}"
  VERSION 4.1.1
  SOVERSION 4
  )
INSTALL(TARGETS codegen
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib # .so
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib # .a
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
  )

# The main file
ADD_EXECUTABLE(echoprint-codegen
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cxx"
  )
TARGET_LINK_LIBRARIES(echoprint-codegen codegen)
INSTALL(TARGETS echoprint-codegen
  DESTINATION "${INSTALL_BIN_DIR}"
  )
