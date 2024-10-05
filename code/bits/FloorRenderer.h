#ifndef AKGR_FLOOR_RENDERER_H
#define AKGR_FLOOR_RENDERER_H

#include <cstdint>

#include <vector>

#include <gf2/core/Time.h>
#include <gf2/graphics/RenderRecorder.h>

namespace akgr {

  class FloorRenderer {
  public:
    FloorRenderer(int priority = 0)
    : m_priority(priority)
    {
    }

    FloorRenderer(const FloorRenderer&) = delete;
    FloorRenderer(FloorRenderer&&) noexcept = default;
    virtual ~FloorRenderer();

    FloorRenderer& operator=(const FloorRenderer&) = delete;
    FloorRenderer& operator=(FloorRenderer&&) noexcept = default;

    int priority() const noexcept {
      return m_priority;
    }

    virtual void update(gf::Time time, int32_t floor);
    virtual void render_floor(gf::RenderRecorder& recorder, int32_t floor) = 0;

  private:
    int m_priority = 0;
  };

  class FloorRendererContainer {
  public:
    void add_renderer(FloorRenderer* renderer);
    void finish();

    void update(gf::Time time, int32_t floor);
    void render_floor(gf::RenderRecorder& recorder, int32_t floor);

  private:
    std::vector<FloorRenderer*> m_renderers;
  };

}

#endif // AKGR_FLOOR_RENDERER_H
