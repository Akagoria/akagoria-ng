#include "PhysicsUtils.h"

#include <cassert>

#include <gf2/core/Log.h>

namespace akgr {

  namespace {

    constexpr unsigned bits_from_floor(int32_t floor) {
      if (floor < -6 || floor > 7) {
        gf::Log::debug("Unknown floor: {}", floor);
        return 0;
      }

      assert(-6 <= floor && floor <= 7);
      return static_cast<unsigned>(1 << (floor + 8));
    }

  }

  gf::PhysicsShapeFilter filter_from_floor(int32_t floor)
  {
    gf::PhysicsShapeFilter filter = {};
    filter.categories = filter.mask = bits_from_floor(floor);
    return filter;
  }

}
