CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Echoprint)

SET(INSTALL_LIB_DIR     "lib"             CACHE PATH "Installation directory for libraries")
SET(INSTALL_BIN_DIR     "bin"             CACHE PATH "Installation directory for executables")
SET(INSTALL_INCLUDE_DIR "include/codegen" CACHE PATH "Installation directory for header files")

ADD_SUBDIRECTORY(src)
