#ifndef AKGR_ASPECT_STATE_H
#define AKGR_ASPECT_STATE_H

#include <gf2/core/Time.h>
#include <gf2/core/TypeTraits.h>

#include "Aspect.h"
#include "Value.h"

namespace akgr {

  struct AspectValue {
    Value value = 75;
    Value max = 100;
    gf::Time period;

    bool increase(gf::Time time);
    void update(gf::Time time, gf::Time max_period);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AspectValue, Archive>& aspect) {
    return ar | aspect.value | aspect.max | aspect.period;
  }

  struct AspectState {
    AspectValue hp;
    AspectValue mp;
    AspectValue vp;

    AspectValue& operator[](Aspect aspect);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AspectState, Archive>& state) {
    return ar | state.hp | state.mp | state.vp;
  }

}

#endif // AKGR_ASPECT_STATE_H
