#ifndef AKGR_CHARACTER_STATE_H
#define AKGR_CHARACTER_STATE_H

#include <gf2/core/TypeTraits.h>

#include "CharacterData.h"
#include "DataReference.h"
#include "DialogData.h"
#include "Spot.h"

namespace akgr {

  struct CharacterState {
    std::string name;
    DataReference<CharacterData> data;
    DataReference<DialogData> dialog;
    Spot spot;
    float rotation = 0.0f;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CharacterState, Archive>& state) {
    return ar | state.name | state.data | state.dialog | state.spot;
  }

}

#endif // AKGR_CHARACTER_STATE_H
