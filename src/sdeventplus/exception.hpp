#pragma once

#include <system_error>

namespace sdeventplus
{

class SdEventError final : public std::system_error
{
  public:
    SdEventError(int r, const char* prefix);
};

} // namespace sdeventplus
