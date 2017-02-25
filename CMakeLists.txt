###########################################################
## CMAKE SETUP
###########################################################

cmake_minimum_required(VERSION 3.7)
project(clang-expand)

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

# Uses clang-expand/cmake/modules/FindLLVM.cmake
find_package(LLVM REQUIRED)
find_package(clang REQUIRED)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})

# Also contains clang libraries
link_directories(${LLVM_LIBRARY_DIRS})

###########################################################
## INCLUDES
###########################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################################
## SOURCES
###########################################################

set(CLANG_EXPAND_SOURCES
  main.cpp
)

###########################################################
## TARGETS
###########################################################

add_executable(clang-expand ${CLANG_EXPAND_SOURCES})

target_include_directories(clang-expand PUBLIC ${LLVM_INCLUDE_DIR})
target_link_libraries(clang-expand
  ${CLANG_LIBS}
  ${LLVM_LIBRARIES}
  ${LLVM_LDFLAGS}
)

###########################################################
## COMPILER FLAGS
###########################################################

set(WARNINGS
  -Wall
  -Wextra
  -Wunused
  -Wdeprecated
  -Wc++11-compat
  -Wfuture-compat
  -Wunreachable-code
)

set(EXTRA_FLAGS -g -Os -O2 -flto -fno-rtti -std=c++1z -stdlib=libc++)

target_compile_options(clang-expand PUBLIC
  ${EXTRA_FLAGS}
  ${WARNINGS}
  ${LLVM_CXXFLAGS})
