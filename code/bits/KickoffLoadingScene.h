#ifndef AKGR_KICKOFF_LOADING_SCENE_H
#define AKGR_KICKOFF_LOADING_SCENE_H

#include <gf2/graphics/ActionGroup.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>
#include <gf2/graphics/TextEntity.h>

#include "KickoffResources.h"

namespace akgr {
  class Akagoria;

  class KickoffLoadingScene : public gf::Scene {
  public:
    KickoffLoadingScene(Akagoria* game, const KickoffResources& resources);

  private:
    void do_update(gf::Time time) override;

    Akagoria* m_game = nullptr;

    gf::FontAtlas m_atlas;

    gf::ActionGroup m_action_group;

    gf::TextEntity m_loading_text;
  };

}

#endif // AKGR_KICKOFF_LOADING_SCENE_H
