#ifndef AKGR_TILESET_GUI_H
#define AKGR_TILESET_GUI_H

#include <filesystem>

#include <gf2/core/Random.h>
#include <gf2/graphics/Entity.h>
#include <gf2/graphics/Texture.h>

#include "TilesetData.h"

namespace akgr {

  class TilesetGui : public gf::Entity {
  public:
    TilesetGui(std::filesystem::path datafile, gf::RenderManager* render_manager);

    void update(gf::Time time) override;

  private:
    gf::RenderManager* m_render_manager = nullptr;

    std::filesystem::path m_datafile;
    TilesetData m_data;
    gf::Random m_random;

    bool m_modified = false;

    // for settings
    gf::Vec2I m_size;

    // edit for atoms
    Atom m_editedAtom;
    static constexpr std::size_t NameBufferSize = 256;
    char m_nameBuffer[NameBufferSize];
    int m_pigmentChoice = 0;
    gf::Texture m_pigmentPreview;
    bool m_newAtom = false;

    // edit for wang2
    Wang2 m_editedWang2;
    int m_borderEffectChoices[2] = { 0, 0 };
    gf::Texture m_wang2Preview;
    bool m_newWang2 = false;

    // edit for wang3
    Wang3 m_editedWang3;
    gf::Id m_idsChoice[3];
    gf::Texture m_wang3Preview;
    bool m_newWang3 = false;
  };

}

#endif // AKGR_TILESET_GUI_H
