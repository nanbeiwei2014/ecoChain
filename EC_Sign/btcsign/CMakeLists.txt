CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT( btcsign )
set(bitcoinsign_VERSION_MAJOR 1)
set(bitcoinsign_VERSION_MINOR 0)

INCLUDE_DIRECTORIES(. DIR_HEADS)

aux_source_directory( . DIR_SRCS )

#link_directories(/usr/lib/)
link_directories(/usr/lib64/)
link_libraries( ssl boost_system boost_thread gmp )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -g" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -g " )

set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )

add_library(btcsign SHARED ${DIR_SRCS} ${DIR_HEADS} ErrorCode.h)

add_executable(test_sign test_sign.cpp)

target_link_libraries(test_sign btcsign )

