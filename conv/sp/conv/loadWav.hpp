#pragma once
#include "sp/math.hpp"

namespace sp { namespace conv
{
    bool loadWav(const std::string &fName, TVReal &samples, uint32_t &sps);
}}
