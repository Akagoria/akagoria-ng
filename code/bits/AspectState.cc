#include "AspectState.h"

#include <cassert>

namespace akgr {

  bool AspectValue::increase(gf::Time time) {
    value += (max / 20) * time.as_seconds();

    if (value > max) {
      value = max;
      return false;
    }

    return true;
  }

  void AspectValue::update(gf::Time time, gf::Time max_period) {
    period += time;

    while (period > max_period) {
      if (value < max) {
        value += 1;
      } else {
        value = max;
      }

      period -= max_period;
    }
  }

  AspectValue& AspectState::operator[](Aspect aspect) {
    switch (aspect) {
      case Aspect::Health:
        return hp;
      case Aspect::Magic:
        return mp;
      case Aspect::Vitality:
        return vp;
    }

    assert(false);
    return hp;
  }

}
