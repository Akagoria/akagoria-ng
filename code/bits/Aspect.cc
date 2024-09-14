#include "Aspect.h"

#include <cassert>

#include "Colors.h"

namespace akgr {

  gf::Color aspect_color(Aspect aspect) {
    switch (aspect) {
      case Aspect::Health:
        return HealthColor;
      case Aspect::Magic:
        return MagicColor;
      case Aspect::Vitality:
        return VitalityColor;
    }

    assert(false);
    return gf::Black;
  }

}

