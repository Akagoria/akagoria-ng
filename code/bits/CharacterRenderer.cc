#include "CharacterRenderer.h"

#include <gf2/core/ShapeBuffer.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    constexpr float CharacterDialogRadius = 25.0f;
    constexpr float CharacterDialogThichness = 5.0f;

  }

  CharacterRenderer::CharacterRenderer(Akagoria* game, [[maybe_unused]] const WorldResources& resources)
  : m_game(game)
  {
    // load all the animations, not really optimal but easy for now

    for (const auto& character_data : game->world_data()->characters) {
      const gf::Id animation_id = gf::hash_string(character_data.animations.string());

      if (m_animation_graphics.find(animation_id) != m_animation_graphics.end()) {
        continue;
      }

      const gf::AnimationGroupResource resource = compute_standard_character_animations(character_animation_path(character_data));
      gf::AnimationGroupGraphics graphics(resource, game->render_manager(), game->resource_manager());
      m_animation_graphics.emplace(animation_id, std::move(graphics));
    }

    // create the animation states

    for (auto& character_state : m_game->world_state()->characters) {
      const gf::Id character_name_id = gf::hash_string(character_state.name);

      gf::AnimationGroupResource resource = compute_standard_character_animations(character_animation_path(character_state.data()));
      gf::AnimationGroupState state(resource.data);

      [[maybe_unused]] auto [ iterator, inserted ] = m_animation_states.emplace(character_name_id, std::move(state));
      assert(inserted);
    }

  }

  void CharacterRenderer::update(gf::Time time, int32_t floor)
  {
    using namespace gf::literals;

    // update dialog shapes

    gf::ShapeGroupBuffer shape_group_buffer;

    for (auto& character : m_game->world_state()->characters) {
      if (character.spot.floor != floor) {
        continue;
      }

      if (character.dialog) {
        gf::Color dialog_color = gf::opaque();

        switch (character.dialog->type) {
          case DialogType::Simple:
            dialog_color *= gf::Amber;
            break;
          case DialogType::Quest:
            dialog_color *= gf::Aquamarine;
            break;
          case DialogType::Story:
            dialog_color *= gf::Purple;
            break;
        }

        auto shape_buffer = gf::ShapeBuffer::make_circle(gf::CircF::from_center_radius(character.spot.location, CharacterDialogRadius));
        shape_buffer.color = gf::Transparent;
        shape_buffer.outline_color = dialog_color;
        shape_buffer.outline_thickness = CharacterDialogThichness;
        shape_group_buffer.shapes.push_back(std::move(shape_buffer));
      }

    }

    m_dialog_shape_group.update(shape_group_buffer, m_game->render_manager());

    // update animations

    for (auto& character_state : m_game->world_state()->characters) {
      if (character_state.spot.floor != floor) {
        continue;
      }

      const gf::Id character_name_id = gf::hash_string(character_state.name);

      auto iterator = m_animation_states.find(character_name_id);

      if (iterator == m_animation_states.end()) {
        gf::AnimationGroupResource resource = compute_standard_character_animations(character_animation_path(*character_state.data.origin));
        gf::AnimationGroupState state(resource.data);

        auto [ new_iterator, inserted ] = m_animation_states.emplace(character_name_id, std::move(state));
        assert(inserted);
        iterator = new_iterator;
      }

      auto& [ _, animation_state ] = *iterator;

      switch (character_state.action) {
        case CharacterAction::Static:
          animation_state.select("static"_id);
          break;
        case CharacterAction::Forward:
          animation_state.select("forward"_id);
          break;
        case CharacterAction::Backward:
          animation_state.select("backward"_id);
          break;
      }

      animation_state.update(time);
    }
  }

  void CharacterRenderer::render_floor(gf::RenderRecorder& recorder, [[maybe_unused]] int32_t floor)
  {
    gf::RenderObject dialog_shape_object;
    dialog_shape_object.geometry = m_dialog_shape_group.geometry();
    dialog_shape_object.priority = 0;

    if (dialog_shape_object.geometry.size > 0) {
      recorder.record(dialog_shape_object);
    }

    for (auto& character_state : m_game->world_state()->characters) {
      if (character_state.spot.floor != floor) {
        continue;
      }

      const gf::Id character_name_id = gf::hash_string(character_state.name);

      auto state_iterator = m_animation_states.find(character_name_id);
      assert(state_iterator != m_animation_states.end());

      const auto& [ actual_character_id, animation_state ] = *state_iterator;

      const gf::Id animation_id = gf::hash_string(character_state.data->animations.string());

      auto graphics_iterator = m_animation_graphics.find(animation_id);
      assert(graphics_iterator != m_animation_graphics.end());

      const auto& [ actual_animation_id, animation_graphics ] = *graphics_iterator;

      gf::Transform transform;
      transform.location = character_state.spot.location;
      transform.rotation = character_state.rotation + gf::Pi2;
      transform.origin = { 0.5f, 0.5f };
      transform.scale = { 0.8f, 0.8f };

      gf::RenderObject character_object;
      character_object.geometry = animation_graphics.geometry(animation_state.current_animation_id(), animation_state.current_frame());
      character_object.priority = 0;
      character_object.transform = transform.compute_matrix(animation_graphics.bounds());
      assert(character_object.geometry.size > 0);
      recorder.record(character_object);
    }

  }

}
