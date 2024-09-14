#ifndef AKGR_ATTRIBUTE_STATE_H
#define AKGR_ATTRIBUTE_STATE_H

#include <gf2/core/TypeTraits.h>

#include "Attribute.h"
#include "Value.h"

namespace akgr {

  struct AttributeValue {
    Value value = 50;

    void increase(Value gain, AttributeValue& anti1, AttributeValue& anti2);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AttributeValue, Archive>& attribute) {
    return ar | attribute.value;
  }

  struct AttributeState {
    AttributeValue strength;
    AttributeValue dexterity;
    AttributeValue intelligence;
    AttributeValue wisdom;
    AttributeValue knowledge;

    void increase(Attribute kind, Value gain);
    AttributeValue& operator[](Attribute attribute);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AttributeState, Archive>& state) {
    return ar | state.strength | state.dexterity | state.intelligence | state.wisdom | state.knowledge;
  }

}

#endif // AKGR_ATTRIBUTE_STATE_H
