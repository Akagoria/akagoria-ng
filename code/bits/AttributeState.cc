#include "AttributeState.h"

#include <cassert>

namespace akgr {

  void AttributeValue::increase(Value gain, AttributeValue& anti1, AttributeValue& anti2) {
    Value loss1 = gain / 2;
    Value loss2 = gain / 2;

    if (loss1 > anti1.value) {
      loss1 = anti1.value;
    }

    if (loss2 > anti2.value) {
      loss2 = anti2.value;
    }

    gain = loss1 + loss2;

    value += gain;
    anti1.value -= loss1;
    anti2.value -= loss2;
  }

  void AttributeState::increase(Attribute kind, Value gain) {
    switch (kind) {
      case Attribute::Strength:
        strength.increase(gain, intelligence, wisdom);
        break;
      case Attribute::Dexterity:
        dexterity.increase(gain, wisdom, knowledge);
        break;
      case Attribute::Intelligence:
        intelligence.increase(gain, knowledge, strength);
        break;
      case Attribute::Wisdom:
        wisdom.increase(gain, strength, dexterity);
        break;
      case Attribute::Knowledge:
        knowledge.increase(gain, dexterity, intelligence);
        break;
    }
  }

  AttributeValue& AttributeState::operator[](Attribute attribute) {
    switch (attribute) {
      case Attribute::Strength:
        return strength;
      case Attribute::Dexterity:
        return dexterity;
      case Attribute::Intelligence:
        return intelligence;
      case Attribute::Wisdom:
        return wisdom;
      case Attribute::Knowledge:
        return knowledge;
    }

    assert(false);
    return strength;
  }

}
