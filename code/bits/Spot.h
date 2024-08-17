#ifndef AKGR_SPOT_H
#define AKGR_SPOT_H

#include <cstdint>

#include <gf2/core/TypeTraits.h>
#include <gf2/core/Vec2.h>

namespace akgr {

  struct Spot {
    gf::Vec2F location = { 0.0f, 0.0f };
    int32_t floor = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<Spot, Archive>& spot) {
    return ar | spot.location | spot.floor;
  }

}

#endif // AKGR_LOCATION_H
