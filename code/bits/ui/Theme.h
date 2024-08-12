#ifndef AKGR_UI_THEME_H
#define AKGR_UI_THEME_H

#include <gf2/core/Color.h>
#include <gf2/core/Vec2.h>

namespace akgr::ui {

  struct Space {
    Space()
    : ne(0.0f, 0.0f), sw(0.0f, 0.0f)
    {
    }

    Space(float single)
    : ne(single, single), sw(single, single)
    {
    }

    Space(float vertical, float horizontal)
    : ne(horizontal, vertical), sw(horizontal, vertical)
    {
    }

    Space(float top, float horizontal, float bottom)
    : ne(horizontal, top), sw(horizontal, bottom)
    {
    }

    Space(float top, float right, float bottom, float left)
    : ne(left, top), sw(right, bottom)
    {
    }

    gf::Vec2F ne;
    gf::Vec2F sw;
  };


  struct WidgetTheme {
    Space padding;
  };

  struct BoxTheme : WidgetTheme {
    float spacing = 0.0f;
  };

  struct StackTheme : BoxTheme {
  };

  struct FrameTheme : BoxTheme {
    gf::Color color = gf::Black * gf::opaque();
    float radius = 0.0f;
    float outline_thickness = 0.0f;
    gf::Color outline_color = gf::White;
  };

  struct MenuTheme : BoxTheme {
  };

  struct LabelTheme : WidgetTheme {
  };

}

#endif // AKGR_UI_THEME_H
