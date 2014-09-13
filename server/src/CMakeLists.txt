CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

CMAKE_POLICY( SET CMP0015 NEW )

PROJECT( Server )

# add definitions
IF(WIN32)
    ADD_DEFINITIONS("-DWIN32")
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y -DWIN32" )
ENDIF()
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y -pthread -DSERVER" )
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Bstatic -static-libgcc -static-libstdc++" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}" )

# search sources
FILE(
    GLOB_RECURSE
    SOURCE_FILES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    *.cpp *.c
)

# create executable
ADD_EXECUTABLE(
    server
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp"
)

# generate cross platform enums for client and server
SET( PARSER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../shared/parser" )
ADD_CUSTOM_COMMAND(
    DEPENDS "${PARSER_DIR}/../lists/messages.txt"
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp"
    COMMAND java
    ARGS -cp "${PARSER_DIR}/" EnumParser "C++" "${PARSER_DIR}/../lists/messages.txt" "${CMAKE_CURRENT_SOURCE_DIR}/constants.hpp" )

