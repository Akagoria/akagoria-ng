#include "WorldResources.h"

#include "Akagoria.h"
#include "Colors.h"

namespace akgr {

  gf::AnimationGroupResource compute_standard_character_animations(const std::filesystem::path& texture)
  {
    using namespace gf::literals;

    gf::AnimationGroupResource animations;
    animations.textures = { texture };

    auto compute_animation = [](gf::Time duration, int32_t frame_count, int32_t frame_offset, int32_t frame_direction = 1) {
      gf::AnimationData animation;
      animation.properties = gf::AnimationProperties::Loop;
      animation.color = gf::White;
      animation.add_tileset(0, gf::vec(16, 16), duration, frame_count, frame_offset, frame_direction);
      return animation;
    };

    animations.data.animations.emplace("static"_id, compute_animation(1000_milliseconds, 1, 0));
    animations.data.animations.emplace("forward"_id, compute_animation(25_milliseconds, 16, 0));
    animations.data.animations.emplace("backward"_id, compute_animation(50_milliseconds, 16, 15, -1));

    return animations;
  }

  WorldResources::WorldResources()
  {
    hero_animations = compute_standard_character_animations("sprites/character_default.png");

    notification_text.data.content = "404 Not Found";
    notification_text.data.character_size = 30.0f;
    notification_text.data.color = gf::White;
    notification_text.data.alignment = gf::Alignment::Center;
    notification_text.data.paragraph_width = 750.0f;
    notification_text.default_font = "fonts/DejaVuSans.ttf";
    notification_text.bold_font = "fonts/DejaVuSans-Bold.ttf";
    notification_text.italic_font = "fonts/DejaVuSans-Oblique.ttf";
    notification_text.bold_italic_font = "fonts/DejaVuSans-BoldOblique.ttf";

    notification_background.buffer = gf::ShapeBuffer::make_rounded_rectangle({ 800.0f, 100.0f }, 10.0f);
    notification_background.buffer.color = RpgBlue * gf::opaque(0.9f);
    notification_background.buffer.outline_color = gf::White;
    notification_background.buffer.outline_thickness = 1.0f;

    dialog_speaker_text.data.content = "?";
    dialog_speaker_text.data.character_size = 20.0f;
    dialog_speaker_text.data.color = gf::White;
    dialog_speaker_text.data.alignment = gf::Alignment::Left;
    dialog_speaker_text.data.paragraph_width = 200.0f;
    dialog_speaker_text.font = "fonts/DejaVuSans.ttf";

    dialog_speaker_background.buffer = gf::ShapeBuffer::make_rounded_rectangle({ 250.0f, 40.0f }, 10.0f);
    dialog_speaker_background.buffer.color = RpgBlue;
    dialog_speaker_background.buffer.outline_color = gf::White;
    dialog_speaker_background.buffer.outline_thickness = 1.0f;

    dialog_words_text.data.content = "?";
    dialog_words_text.data.character_size = 24.0f;
    dialog_words_text.data.color = gf::White;
    dialog_words_text.data.alignment = gf::Alignment::Left;
    dialog_words_text.data.paragraph_width = 750.0f;
    dialog_words_text.default_font = "fonts/DejaVuSans.ttf";
    dialog_words_text.bold_font = "fonts/DejaVuSans-Bold.ttf";
    dialog_words_text.italic_font = "fonts/DejaVuSans-Oblique.ttf";
    dialog_words_text.bold_italic_font = "fonts/DejaVuSans-BoldOblique.ttf";

    dialog_words_background.buffer = gf::ShapeBuffer::make_rounded_rectangle({ 800.0f, 120.0f }, 10.0f);
    dialog_words_background.buffer.color = RpgBlue * gf::opaque(0.9f);
    dialog_words_background.buffer.outline_color = gf::White;
    dialog_words_background.buffer.outline_thickness = 1.0f;

    aspect_text.data.content = "?";
    aspect_text.data.character_size = 15.0f;
    aspect_text.data.color = gf::White;
    aspect_text.font = "fonts/DejaVuSans.ttf";

    area_text.data.content = "?";
    area_text.data.character_size = 20.0f;
    area_text.data.color = gf::White;
    area_text.data.paragraph_width = 340.0f;
    area_text.data.alignment = gf::Alignment::Center;
    area_text.font = "fonts/DejaVuSans.ttf";

    pinned_quest_text.data.content = "?";
    pinned_quest_text.data.character_size = 15.0f;
    pinned_quest_text.data.color = gf::White;
    pinned_quest_text.data.alignment = gf::Alignment::Justify;
    pinned_quest_text.data.paragraph_width = 300.0f;
    pinned_quest_text.default_font = "fonts/DejaVuSans.ttf";
    pinned_quest_text.bold_font = "fonts/DejaVuSans-Bold.ttf";
    pinned_quest_text.italic_font = "fonts/DejaVuSans-Oblique.ttf";
    pinned_quest_text.bold_italic_font = "fonts/DejaVuSans-BoldOblique.ttf";
  }

  void WorldResources::bind(const WorldData& data)
  {
    // map_textures

    map_textures.clear();
    map_textures.reserve(data.map.textures.size());

    for (const std::filesystem::path& texture : data.map.textures) {
      map_textures.push_back(texture);
    }

    // animation_textures

    for (const auto& character : data.characters) {
      animation_textures.push_back(character_animation_path(character));
    }

  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resource_manager, gf::ResourceAction action) {
      for (const std::filesystem::path& texture : map_textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const std::filesystem::path& texture : animation_textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const std::filesystem::path& texture : hero_animations.textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const gf::TextResource& resource : { dialog_speaker_text, aspect_text, area_text }) {
        bundle->handle<gf::FontFace>(resource.font, game->font_manager(), resource_manager, action);
      }

      for (const gf::RichTextResource& resource : { notification_text, dialog_words_text, pinned_quest_text }) {
        bundle->handle<gf::FontFace>(resource.default_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(resource.bold_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(resource.italic_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(resource.bold_italic_font, game->font_manager(), resource_manager, action);
      }
    });

    return bundle;
  }


}
