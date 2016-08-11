// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"




// TODO: reference additional headers your program requires here
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <numeric>
#include <string>
#include <set>

//////////////////////////////////////////////////////////////////////////
#include <boost/thread.hpp>
#undef min
#undef max
#undef far
#undef near
#undef pascal

#include <boost/bind.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/filesystem.hpp>

#include <boost/math/special_functions.hpp>
