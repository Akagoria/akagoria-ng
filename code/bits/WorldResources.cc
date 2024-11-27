#include "WorldResources.h"

#include <gf2/framework/BundleBuilder.h>

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

    icon_left_arrow_text.data.content = "↩"; // "«";
    icon_left_arrow_text.data.character_size = 20.0f;
    icon_left_arrow_text.data.color = gf::gray(0.7f);
    icon_left_arrow_text.font = "fonts/DejaVuSans-Bold.ttf";

    icon_right_arrow_text.data.content = "↪"; // "»";
    icon_right_arrow_text.data.character_size = 20.0f;
    icon_right_arrow_text.data.color = gf::gray(0.7f);
    icon_right_arrow_text.font = "fonts/DejaVuSans-Bold.ttf";

    common_items_text.data.content = "Common Items";
    common_items_text.data.character_size = 30.0f;
    common_items_text.data.color = gf::White;
    common_items_text.font = "fonts/DejaVuSans.ttf";

    quest_items_text.data.content = "Quest Items";
    quest_items_text.data.character_size = 30.0f;
    quest_items_text.data.color = gf::White;
    quest_items_text.font = "fonts/DejaVuSans.ttf";

    back_text.data.content = "Back"; // TODO: i18n
    back_text.data.character_size = 30.0f;
    back_text.data.color = gf::White;
    back_text.font = "fonts/DejaVuSans.ttf";

    icon_arrow_text.data.content = "→";
    icon_arrow_text.data.character_size = 30.0f;
    icon_arrow_text.data.color = gf::White;
    icon_arrow_text.font = "fonts/DejaVuSans.ttf";

    inventory_text.data.content = "Inventory"; // TODO: i18n
    inventory_text.data.character_size = 30.0f;
    inventory_text.data.color = gf::White;
    inventory_text.font = "fonts/DejaVuSans.ttf";

    back_to_adventure_text.data.content = "Back To Adventure"; // TODO: i18n
    back_to_adventure_text.data.character_size = 30.0f;
    back_to_adventure_text.data.color = gf::White;
    back_to_adventure_text.font = "fonts/DejaVuSans.ttf";

    back_to_real_life_text.data.content = "Back To Real Life"; // TODO: i18n
    back_to_real_life_text.data.character_size = 30.0f;
    back_to_real_life_text.data.color = gf::White;
    back_to_real_life_text.font = "fonts/DejaVuSans.ttf";
  }

  void WorldResources::bind(const WorldData& data)
  {
    // map_textures

    map_textures.clear();
    map_textures.reserve(data.map.textures.size());

    for (const std::filesystem::path& texture : data.map.textures) {
      map_textures.push_back(texture);
    }

    // item_textures

    for (const auto& item : data.items) {
      item_textures.push_back(item.sprite.texture);
    }

    // animation_textures

    for (const auto& character : data.characters) {
      animation_textures.push_back(character_animation_path(character));
    }

  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game) const
  {
    gf::BundleBuilder builder(game);

    builder.add_in_bundle(hero_animations);

    builder.add_in_bundle(notification_text);
    builder.add_in_bundle(dialog_speaker_text);
    builder.add_in_bundle(dialog_words_text);

    builder.add_in_bundle(aspect_text);
    builder.add_in_bundle(area_text);

    builder.add_in_bundle(pinned_quest_text);

    builder.add_in_bundle(icon_left_arrow_text);
    builder.add_in_bundle(icon_right_arrow_text);

    builder.add_in_bundle(common_items_text);
    builder.add_in_bundle(quest_items_text);
    builder.add_in_bundle(back_text);

    builder.add_in_bundle(icon_arrow_text);
    builder.add_in_bundle(inventory_text);
    builder.add_in_bundle(back_to_adventure_text);
    builder.add_in_bundle(back_to_real_life_text);

    for (const std::filesystem::path& texture : map_textures) {
      builder.add_raw_texture(texture);
    }

    for (const std::filesystem::path& texture : item_textures) {
      builder.add_raw_texture(texture);
    }

    for (const std::filesystem::path& texture : animation_textures) {
      builder.add_raw_texture(texture);
    }

    return builder.make_bundle();
  }


}
