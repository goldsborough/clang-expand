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

option(VERBOSE_CONFIG off)

function(set_llvm_variable variable flags)
  execute_process(
      COMMAND ${LLVM_CONFIG} ${flags}
      RESULT_VARIABLE result_code
      OUTPUT_VARIABLE output
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(result_code)
      message(FATAL_ERROR "Failed to execute llvm-config ${flags}, result code: ${result_code}")
  else()
      string(REPLACE "\n" ";" output ${output})
      separate_arguments(output)
      if (VERBOSE_CONFIG)
        message(STATUS "Found LLVM_${variable}: ${output}")
      endif()
      set(LLVM_${variable} "${output}" PARENT_SCOPE)
  endif()
endfunction()

find_program(LLVM_CONFIG
    NAMES llvm-config
    HINTS $ENV{LLVM_PATH} $ENV{LLVM_PATH}/build/bin
    DOC "Path to llvm-config tool")
if (LLVM_CONFIG)
  message(STATUS "Found llvm-config at ${LLVM_CONFIG}")
else()
  message(FATAL_ERROR "Could not find llvm-config")
endif()

set_llvm_variable(ROOT "--src-root")
set_llvm_variable(VERSION_STRING "--version")
set_llvm_variable(BUILD "--obj-root")
set_llvm_variable(BIN "--bindir")
set_llvm_variable(HOST_TARGET "--host-target")
set_llvm_variable(INCLUDE_DIRS "--includedir")
set_llvm_variable(LIBRARY_DIRS "--libdir")
set_llvm_variable(CXX_FLAGS "--cxxflags")
set_llvm_variable(LD_FLAGS "--ldflags")
set_llvm_variable(LIBS "--libs;--system-libs")

set(CLANG_ROOT "${LLVM_ROOT}/tools/clang")
set(CLANG_INCLUDE_DIRS
    ${CLANG_ROOT}/include
    ${LLVM_BUILD}/tools/clang/include)

set(CLANG_CXX "${LLVM_BIN}/clang++")
set(CMAKE_CXX_COMPILER ${CLANG_CXX})

option(SYSTEM_LIBCXX OFF)
if (EXISTS "${LLVM_ROOT}/projects/libcxx" AND NOT SYSTEM_LIBCXX)
  message(STATUS "Using libcxx headers from ${LLVM_ROOT}/projects/libcxx")
  include_directories(SYSTEM "${LLVM_ROOT}/projects/libcxx/include")
endif()

string(REPLACE ";" " " LLVM_LD_FLAGS_STRING "${LLVM_LD_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS ${LLVM_LD_FLAGS_STRING})

include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})

# Also contains clang libraries
link_directories(${LLVM_LIBRARY_DIRS})

###########################################################
## COMPILER FLAGS
###########################################################

set(WARNINGS
  -Wall
  -Wextra
  -Wunused
  -Wdeprecated
  -Wunreachable-code)

# Want our own standard
list(REMOVE_ITEM LLVM_CXX_FLAGS "-std=c++11" "-std=c++14" "-std=c++1z")

set(EXTRA_FLAGS -g -Os -O2 -flto -std=c++1z)
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

add_executable(clang-expand main.cpp)
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
