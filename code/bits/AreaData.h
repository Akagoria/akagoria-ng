#ifndef AKGR_AREA_DATA_H
#define AKGR_AREA_DATA_H

#include <gf2/core/TypeTraits.h>
#include <gf2/core/Vec2.h>

#include "DataLabel.h"

namespace akgr {

  struct AreaData {
    DataLabel label;
    gf::Vec2F position;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AreaData, Archive>& data) {
    return ar | data.label | data.position;
  }

}

#endif // AKGR_AREA_DATA_H
