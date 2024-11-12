#ifndef AKGR_ITEM_DATA_H
#define AKGR_ITEM_DATA_H

#include <cstdint>

#include <gf2/core/SpriteData.h>
#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  enum class ItemType : uint8_t {
    None,
    HeadArmor,
    TorsoArmor,
    ArmsArmor,
    LegsArmor,
    CamouflageCloth,
    MeleeWeapon,
    RangedWeapon,
    Explosive,
    Potion,
    Ingredient,
    Recipes,
    Book,
    Parchment,
    Quest,
  };

  struct ItemData {
    DataLabel label;
    ItemType type;
    std::string description;
    gf::SpriteResource sprite;
    float scale = 1.0f;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemData, Archive>& data) {
    return ar | data.label | data.type |  data.description | data.sprite | data.scale;
  }

}

#endif // AKGR_ITEM_DATA_H
