#ifndef AKGR_LOCATION_DATA_H
#define AKGR_LOCATION_DATA_H

#include "DataLabel.h"
#include "Spot.h"

namespace akgr {

  struct LocationData {
    DataLabel label;
    Spot spot;
  };

}

#endif // AKGR_LOCATION_DATA_H
