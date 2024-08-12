#ifndef AKGR_KICKOFF_MENU_SCENE_H
#define AKGR_KICKOFF_MENU_SCENE_H

#include <gf2/graphics/ActionGroup.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>
#include <gf2/graphics/ShapeEntity.h>
#include <gf2/graphics/SpriteEntity.h>
#include <gf2/graphics/TextEntity.h>

#include "MenuData.h"
#include "ui/State.h"
#include "ui/Theme.h"
#include "ui/Widgets.h"

namespace akgr {

  class Akagoria;

  class KickoffMenuScene : public gf::Scene {
  public:
    KickoffMenuScene(Akagoria* game, const MenuData& data);

  private:
    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;

    gf::ActionGroup m_action_group;

    gf::Text m_icon_arrow_text;
    gf::Text m_icon_left_text;
    gf::Text m_icon_right_text;

    gf::Text m_start_text;
    gf::Text m_load_text;
    gf::Text m_quit_text;
    gf::Text m_back_text;

    ui::FrameTheme m_frame_theme;
    ui::FrameWidget m_frame_widget;

    ui::IndexState m_menu_index;
    ui::MenuTheme m_menu_theme;

    ui::LabelTheme m_label_theme;

  };

}

#endif // AKGR_KICKOFF_MENU_SCENE_H
