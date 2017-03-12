########################################
# SOURCES
########################################

set(CLANG_EXPAND_SOURCES
  common/assignee-data.cpp
  common/call-data.cpp
  common/canonical-location.cpp
  common/definition-data.cpp
  common/definition-rewriter.cpp
  common/location.cpp
  common/offset.cpp
  common/range.cpp
  common/routines.cpp
  definition-search/action.cpp
  definition-search/consumer.cpp
  definition-search/match-handler.cpp
  definition-search/tool-factory.cpp
  result.cpp
  search.cpp
  symbol-search/action.cpp
  symbol-search/consumer.cpp
  symbol-search/macro-search.cpp
  symbol-search/match-handler.cpp
  symbol-search/tool-factory.cpp
)

########################################
# TARGET
########################################

add_library(clang-expand-library STATIC ${CLANG_EXPAND_SOURCES})
