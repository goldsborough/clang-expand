###########################################################
## CMAKE SETUP
###########################################################

cmake_minimum_required(VERSION 3.7)
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
  clangStaticAnalyzerFrontend
  clangStaticAnalyzerCheckers
  clangStaticAnalyzerCore
  clangSerialization
  clangToolingCore
  clangTooling
  clangFormat
)

find_package(LLVM REQUIRED)
set(CMAKE_EXE_LINKER_FLAGS ${LLVM_LD_FLAGS_STRING})

include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})

# Also contains clang libraries
link_directories(${LLVM_LIBRARY_DIRS})

# Third Party
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

###########################################################
## COMPILER FLAGS
###########################################################

set(WARNINGS
    -Wall
    -Wextra
    -Wunused
    -Wdeprecated
    -Wunreachable-code)

list(REMOVE_ITEM LLVM_CXX_FLAGS "-std=c++11" "-std=c++14" "-std=c++1z")
list(REMOVE_ITEM LLVM_CXX_FLAGS "-DNDEBUG")

set(EXTRA_FLAGS -g -Os -O2 -std=c++1z -DJSON_NOEXCEPTION)
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
