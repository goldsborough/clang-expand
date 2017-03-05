########################################
# SOURCES
########################################

set(CLANG_EXPAND_SOURCES
  symbol-search/action.cpp
  symbol-search/consumer.cpp
  symbol-search/match-handler.cpp
  symbol-search/macro-search.cpp
  symbol-search/tool-factory.cpp
  definition-search/tool-factory.cpp
  definition-search/action.cpp
  definition-search/consumer.cpp
  definition-search/match-handler.cpp
  common/routines.cpp
  common/structures.cpp
  common/parameter-rewriter.cpp
  search.cpp
)

########################################
# TARGET
########################################

add_library(clang-expand-library STATIC ${CLANG_EXPAND_SOURCES})
