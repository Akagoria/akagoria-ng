#ifndef AKGR_NOTIFICATION_DATA_H
#define AKGR_NOTIFICATION_DATA_H

#include <string>

#include <gf2/core/Time.h>
#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  struct NotificationData {
    DataLabel label;
    std::string message;
    gf::Time duration;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<NotificationData, Archive>& data) {
    return ar | data.label | data.message | data.duration;
  }

}

#endif // AKGR_NOTIFICATION_DATA_H
