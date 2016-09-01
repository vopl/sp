#pragma once
#include "sp/types.hpp"

namespace sp
{
    bool loadWav(const std::string &fName, TVReal &samples, uint32_t &sps);
}
