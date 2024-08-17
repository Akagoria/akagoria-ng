#ifndef AKGR_DATA_LABEL_H
#define AKGR_DATA_LABEL_H

#include <string>

#include <gf2/core/Id.h>
#include <gf2/core/Serialization.h>

namespace akgr {

  struct DataLabel {
    std::string tag;
    gf::Id id;

    DataLabel& operator=(const std::string& other) {
      tag = other;
      id = gf::hash_string(tag);
      return *this;
    }
  };

  gf::Serializer& operator|(gf::Serializer& ar, const DataLabel& label);
  gf::Deserializer& operator|(gf::Deserializer& ar, DataLabel& label);

}

#endif // AKGR_DATA_LABEL_H
