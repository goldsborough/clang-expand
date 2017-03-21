###########################################################
## CMAKE SETUP
###########################################################

cmake_minimum_required(VERSION 3.5)
project(clang-expand)

set(CMAKE_MODULE_PATH
   ${CMAKE_MODULE_PATH}
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

###########################################################
## DOCUMENTATION
###########################################################

# Add a target to generate documentation with Doxygen.
find_package(Doxygen)
if(DOXYGEN_FOUND)
  # Replace all variables of the form @<var>@ inside the Doxyfile
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
  )
  add_custom_target(
    docs
    ${DOXYGEN_EXECUTABLE}
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif()

###########################################################
## DEPENDENCIES
###########################################################

set(CLANG_LIBS
  clangAST
  clangASTMatchers
  clangAnalysis
  clangBasic
  clangDriver
  clangEdit
  clangFrontend
  clangFrontendTool
  clangLex
  clangParse
  clangSema
  clangEdit
  clangRewrite
  clangRewriteFrontend
  clangSerialization
  clangTooling
  clangToolingCore
)

if (NOT CMAKE_HOST_APPLE)
  set(CLANG_LIBS -Wl,--start-group ${CLANG_LIBS} -Wl,--end-group)
endif()

find_package(LLVM REQUIRED)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})

# Third Party
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

# Also contains clang libraries
link_directories(${LLVM_LIBRARY_DIRS})
set(CMAKE_EXE_LINKER_FLAGS ${LLVM_LD_FLAGS_STRING})

###########################################################
## COMPILER FLAGS
###########################################################

set(CXX_STANDARD_REQUIRED ON)
set(WARNINGS
    -Wall
    -Wextra
    -Wunused
    -Wdeprecated
    -Wunreachable-code)

list(REMOVE_ITEM LLVM_CXX_FLAGS
                 ${LLVM_INCLUDE_DIRS} # Already included
                 "-DNDEBUG"
                 "-std=c++11"         # Want our own standard
                 "-std=c++1y"
                 "-std=c++14"
                 "-stdlib=libc++")

if (NOT ${CMAKE_CXX_COMPILER} MATCHES "clang.*")
  # These seem to be clang-specific
  list(REMOVE_ITEM LLVM_CXX_FLAGS
                   "-Wcovered-switch-default"
                   "-Wstring-conversion")
endif()

set(EXTRA_FLAGS -Os -std=c++1z -DJSON_NOEXCEPTION)
set(ALL_FLAGS ${WARNINGS} ${LLVM_CXX_FLAGS} ${EXTRA_FLAGS})

add_compile_options(${ALL_FLAGS})

###########################################################
## INCLUDES
###########################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################################
## SOURCES
###########################################################

add_subdirectory(source)

###########################################################
## TARGETS
###########################################################

add_executable(clang-expand clang-expand.cpp)
target_link_libraries(clang-expand
                      clang-expand-library
                      ${CLANG_LIBS}
                      ${LLVM_LIBS})

###########################################################
## TOOLS <3
###########################################################

# IWYU
option(CLANG_EXPAND_IWYU OFF)

if(${CLANG_EXPAND_IWYU})
  find_program(iwyu_path NAMES include-what-you-use iwyu)
  if(iwyu_path)
    message(STATUS "Found include-what-you-use, integrating with target")
    set_property(TARGET clang-expand
                 PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
  else()
    message(WARNING "Could not find include-what-you-use, skipping")
  endif()
endif()
