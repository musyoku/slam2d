#include "icp.h"

namespace slam {
namespace scan_matching {
    IterativeClosestPoints::IterativeClosestPoints(Associator* associator)
    {
        _associator = associator;
    }
}
}