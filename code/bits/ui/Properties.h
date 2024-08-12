#ifndef AKGR_UI_PROPERTIES_H
#define AKGR_UI_PROPERTIES_H

#include <cstdint>

namespace akgr::ui {

  enum class Aspect : uint8_t {
    Horizontal,
    Vertical,
  };

  enum class Positioning : uint8_t {
    Minimum,
    Middle,
    Maximum,
    Fill,
  };

}

#endif // AKGR_UI_PROPERTIES_H
