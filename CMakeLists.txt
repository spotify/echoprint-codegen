project(echoprint-codegen)
cmake_minimum_required(VERSION 2.8)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

# Stage 0 - Find dependencies
# Boost
find_package(Boost REQUIRED )

# TagLib
find_package(Taglib REQUIRED)

# Thread library ( pthreads or other )
find_package( Threads REQUIRED )

# FFMPEG
find_package( FFMPEG REQUIRED )

# ZLib
find_package( ZLIB REQUIRED )

# Add library
set ( codegen_SOURCES
    src/AudioBufferInput.cxx
    src/AudioStreamInput.cxx
    src/Base64.cxx
    src/Codegen.cxx
    src/Fingerprint.cxx
    src/MatrixUtility.cxx
    src/SubbandAnalysis.cxx
    src/Whitening.cxx
    )

set ( codegen_HEADERS
    src/AudioBufferInput.h
    src/AudioStreamInput.h
    src/Base64.h
    src/Codegen.h
    src/Fingerprint.h 
    src/MatrixUtility.h 
    src/SubbandAnalysis.h 
    src/Whitening.h
    )

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${TAGLIB_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    )

add_library( codegen SHARED ${codegen_SOURCES} )

target_link_libraries( codegen
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    )


# Add executable
set( echoprint_codegen_SOURCES
    src/Metadata.cxx
    src/main.cxx
    )

add_executable( echoprint-codegen
    ${echoprint_codegen_SOURCES}
    )
target_link_libraries(
    echoprint-codegen
    ${TAGLIB_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    codegen
    )

# Install library and executable
install( TARGETS echoprint-codegen
    RUNTIME DESTINATION bin
    )

install( TARGETS codegen
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

