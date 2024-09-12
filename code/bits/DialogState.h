#ifndef AKGR_DIALOG_STATE_H
#define AKGR_DIALOG_STATE_H

#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "DialogData.h"

namespace akgr {

  struct DialogState {
    DataReference<DialogData> data;
    std::size_t current_line = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DialogState, Archive>& state) {
    return ar | state.data | state.current_line;
  }

}

#endif // AKGR_DIALOG_STATE_H
