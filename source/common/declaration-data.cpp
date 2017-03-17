// Project includes
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/location.hpp"

// Third party includes
#include <third-party/json.hpp>

// Standard includes
#include <string>
#include <utility>

namespace ClangExpand {
DeclarationData::DeclarationData(std::string name_, Location location_)
: name(std::move(name_)), location(std::move(location_)) {
}

nlohmann::json DeclarationData::toJson() const {
  // clang-format off
    return {
      {"location", location.toJson()},
      {"name", name},
      {"text", text}
    };
  // clang-format on
}
}  // namespace ClangExpand
