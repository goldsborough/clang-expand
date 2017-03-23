################################################################################
# Finds LLVM and clang libraries, headers and required flags using llvm-config.
# Sets:
# LLVM_ROOT:
#      The root LLVM directory.
# LLVM_BUILD:
#     The LLVM build directory.
# LLVM_BIN:
#     The LLVM binaries directories.
# LLVM_INCLUDE_DIRS:
#     The LLVM includes directories.
# LLVM_LIBRARY_DIRS:
#     The LLVM libraries directory.
# LLVM_CXX_FLAGS:
#     The compiler flags required to compile LLVM and clang libraries.
# LLVM_LD_FLAGS:
#     The linker flags required to compile LLVM and clang libraries.
# LLVM_LD_FLAGS_STRING:
#     Like LLVM_LD_FLAGS, but as a string instead of list.
# LLVM_LIBS:
#     The list of all LLVM libraries to link.
# LIBCXX_INCLUDE_DIRS:
#     The include directory for libc++, if it exists.
################################################################################

option(FIND_LLVM_VERBOSE_CONFIG off)

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
      if (FIND_LLVM_VERBOSE_CONFIG)
        message(STATUS "Found LLVM_${variable}: ${output}")
      endif()
      set(LLVM_${variable} "${output}" PARENT_SCOPE)
  endif()
endfunction()

message(STATUS "LLVM_PATH is ${LLVM_PATH}")

find_program(LLVM_CONFIG
    NAMES llvm-config
    HINTS "${LLVM_PATH}/build/bin" "${LLVM_PATH}"
    DOC "Path to llvm-config tool")
if (LLVM_CONFIG)
  message(STATUS "Found llvm-config at ${LLVM_CONFIG}")
else()
  message(FATAL_ERROR "Could not find llvm-config")
endif()

set_llvm_variable(ROOT "--src-root")
set_llvm_variable(BUILD "--obj-root")
set_llvm_variable(BIN "--bindir")
set_llvm_variable(INCLUDE_DIRS "--includedir")
set_llvm_variable(LIBRARY_DIRS "--libdir")
set_llvm_variable(CXX_FLAGS "--cxxflags")
set_llvm_variable(LD_FLAGS "--ldflags")
set_llvm_variable(LIBS "--libs;--system-libs")

set(CLANG_ROOT "${LLVM_ROOT}/tools/clang")
set(CLANG_INCLUDE_DIRS
    ${CLANG_ROOT}/include
    ${LLVM_BUILD}/tools/clang/include)
set(LLVM_INCLUDE_DIRS
    ${LLVM_INCLUDE_DIRS}
    ${LLVM_BUILD}/include)

if (EXISTS "${LLVM_ROOT}/projects/libcxx")
  set(LLVM_LIBCXX_INCLUDE_DIRS "${LLVM_ROOT}/projects/libcxx/include")
  message(STATUS "Found LLVM_LIBCXX_INCLUDE_DIRS: ${LLVM_LIBCXX_INCLUDE_DIRS}")
endif()

string(REPLACE ";" " " LLVM_LD_FLAGS_STRING "${LLVM_LD_FLAGS}")
