########################################
# SOURCES
########################################

set(CLANG_EXPAND_SOURCES
  symbol-search/action.cpp
  symbol-search/consumer.cpp
  symbol-search/match-handler.cpp
  symbol-search/preprocessor-hooks.cpp
  symbol-search/tool-factory.cpp
  common/routines.cpp
  common/structures.cpp
)

########################################
# TARGET
########################################

add_library(clang-expand-library STATIC ${CLANG_EXPAND_SOURCES})
