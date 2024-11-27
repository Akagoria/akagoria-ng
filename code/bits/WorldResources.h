#ifndef AKGR_WORLD_RESOURCES_H
#define AKGR_WORLD_RESOURCES_H

#include <filesystem>
#include <vector>

#include <gf2/core/AnimationData.h>
#include <gf2/core/ResourceBundle.h>
#include <gf2/core/ShapeBuffer.h>
#include <gf2/core/TextData.h>

namespace akgr {
  class Akagoria;
  struct WorldData;

  gf::AnimationGroupResource compute_standard_character_animations(const std::filesystem::path& texture);

  struct WorldResources {
    WorldResources();

    gf::AnimationGroupResource hero_animations;

    gf::RichTextResource notification_text;
    gf::ShapeResource notification_background;

    gf::TextResource dialog_speaker_text;
    gf::ShapeResource dialog_speaker_background;

    gf::RichTextResource dialog_words_text;
    gf::ShapeResource dialog_words_background;

    gf::TextResource aspect_text;
    gf::TextResource area_text;

    gf::RichTextResource pinned_quest_text;

    gf::TextResource icon_left_arrow_text;
    gf::TextResource icon_right_arrow_text;

    gf::TextResource common_items_text;
    gf::TextResource quest_items_text;
    gf::TextResource back_text;

    gf::TextResource icon_arrow_text;
    gf::TextResource inventory_text;
    gf::TextResource back_to_adventure_text;
    gf::TextResource back_to_real_life_text;

    // bound from data

    std::vector<std::filesystem::path> map_textures;
    std::vector<std::filesystem::path> item_textures;
    std::vector<std::filesystem::path> animation_textures;

    void bind(const WorldData& data);
    gf::ResourceBundle bundle(Akagoria* game) const;
  };

}

#endif // AKGR_WORLD_RESOURCES_H
