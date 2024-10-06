#ifndef AKGR_CHARACTER_STATE_H
#define AKGR_CHARACTER_STATE_H

#include <cstdint>

#include <variant>

#include <gf2/core/TypeTraits.h>

#include "CharacterData.h"
#include "DataReference.h"
#include "DialogData.h"
#include "Spot.h"

namespace akgr {

  enum class CharacterBehavior : uint8_t {
    None,
    Stay,
  };

  enum class CharacterAction : uint8_t {
    Static,
    Forward,
    Backward,
  };

  struct CharacterStayState {
    gf::Vec2F location;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CharacterStayState, Archive>& state) {
    return ar | state.location;
  }

  struct CharacterState {
    std::string name;
    DataReference<CharacterData> data;
    DataReference<DialogData> dialog;
    Spot spot;
    float rotation = 0.0f;
    CharacterAction action = CharacterAction::Static;

    std::variant<std::monostate, CharacterStayState> behavior;

    CharacterBehavior behavior_type() const { return static_cast<CharacterBehavior>(behavior.index()); }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CharacterState, Archive>& state) {
    return ar | state.name | state.data | state.dialog | state.spot | state.rotation | state.behavior;
  }

}

#endif // AKGR_CHARACTER_STATE_H
