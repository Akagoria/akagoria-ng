#ifndef AKGR_WORLD_RESOURCES_H
#define AKGR_WORLD_RESOURCES_H

#include <map>
#include <vector>

#include <gf2/core/AnimationData.h>
#include <gf2/core/ResourceBundle.h>
#include <gf2/core/TextData.h>

namespace akgr {
  class Akagoria;
  struct WorldData;

  struct WorldResources {
    WorldResources();

    gf::AnimationGroupResource hero_animations;

    // bound from data

    std::vector<std::filesystem::path> map_textures;
    std::map<gf::Id, gf::RichTextResource> notifications;

    void bind(const WorldData& data);
    gf::ResourceBundle bundle(Akagoria* game);
  };

}

#endif // AKGR_WORLD_RESOURCES_H
