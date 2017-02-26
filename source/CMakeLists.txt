########################################
# SOURCES
########################################

set(CLANG_EXPAND_SOURCES
  action.cpp
)

########################################
# TARGET
########################################

add_library(clang-expand-library STATIC ${CLANG_EXPAND_SOURCES})
