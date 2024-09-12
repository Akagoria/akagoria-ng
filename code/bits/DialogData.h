#ifndef AKGR_DIALOG_DATA_H
#define AKGR_DIALOG_DATA_H

#include <string>
#include <vector>

#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  struct DialogLine {
    std::string speaker;
    std::string words;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DialogLine, Archive>& data) {
    return ar | data.speaker | data.words;
  }

  enum class DialogType : uint8_t {
    Simple  = 1,
    Quest   = 2,
    Story   = 3,
  };

  struct DialogData {
    DataLabel label;
    DialogType type;
    std::vector<DialogLine> content;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DialogData, Archive>& data) {
    return ar | data.label | data.type | data.content;
  }

}

#endif // AKGR_DIALOG_DATA_H
