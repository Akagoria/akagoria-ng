#ifndef AKGR_ASPECT_H
#define AKGR_ASPECT_H

#include <cstdint>

#include <gf2/core/Color.h>

namespace akgr {

  enum class Aspect : uint8_t {
    Health,
    Magic,
    Vitality,
  };

  gf::Color aspect_color(Aspect aspect);

}

#endif // AKGR_ASPECT_H
