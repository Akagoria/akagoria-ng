#include "ItemRenderer.h"

#include "Akagoria.h"

namespace akgr {

  namespace {

    void compute_actual_resource(Akagoria* game, gf::SpriteGroupResource& resource, int32_t floor)
    {
      resource.data.sprites.clear();

      for (const ItemState& item : game->world_state()->items) {
        if (item.spot.floor != floor) {
          continue;
        }

        gf::Transform transform;
        transform.location = item.spot.location;
        transform.origin = { 0.5f, 0.5f };
        transform.rotation = item.rotation;
        transform.scale = { item.data->scale, item.data->scale };

        resource.add_sprite(item.data->sprite, transform);
      }
    }

    gf::SpriteGroupResource compute_initial_resource(Akagoria* game)
    {
      const int32_t floor = game->world_state()->hero.spot.floor;
      gf::SpriteGroupResource resource;

      for (auto& item : game->world_data()->items) {
        resource.add_texture(item.sprite.texture);
      }

      compute_actual_resource(game, resource, floor);

      return resource;
    }

  }

  ItemRenderer::ItemRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_sprite_group_resource(compute_initial_resource(game))
  , m_sprite_group(m_sprite_group_resource, game->render_manager(), game->resource_manager())
  {
  }

  void ItemRenderer::update([[maybe_unused]] gf::Time time, int32_t floor)
  {
    compute_actual_resource(m_game, m_sprite_group_resource, floor);
    m_sprite_group.update(m_sprite_group_resource.data, m_game->render_manager());
  }

  void ItemRenderer::render_floor(gf::RenderRecorder& recorder, [[maybe_unused]] int32_t floor)
  {
    auto geometries = m_sprite_group.geometry();

    for (auto& geometry : geometries) {
      if (geometry.size == 0) {
        continue;
      }

      gf::RenderObject object;
      object.geometry = geometry;
      recorder.record(object);
    }
  }

}
