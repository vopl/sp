#pragma once
#include "sp/math.hpp"

namespace sp
{
    bool loadWav(const std::string &fName, TVReal &samples, uint32_t &sps);
}
