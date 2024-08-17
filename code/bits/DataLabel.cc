#include "DataLabel.h"

#include <gf2/core/SerializationOps.h>

namespace akgr {

  gf::Serializer& operator|(gf::Serializer& ar, const DataLabel& label)
  {
    return ar | label.tag;
  }

  gf::Deserializer& operator|(gf::Deserializer& ar, DataLabel& label)
  {
    ar | label.tag;

    if (label.tag.empty()) {
      label.id = gf::InvalidId;
    } else {
      label.id = gf::hash_string(label.tag);
    }

    return ar;
  }

}
