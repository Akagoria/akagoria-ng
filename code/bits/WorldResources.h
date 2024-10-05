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

    // bound from data

    std::vector<std::filesystem::path> map_textures;
    std::vector<std::filesystem::path> animation_textures;

    void bind(const WorldData& data);
    gf::ResourceBundle bundle(Akagoria* game);
  };

}

#endif // AKGR_WORLD_RESOURCES_H
