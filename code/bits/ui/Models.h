#ifndef AKGR_UI_MODELS_H
#define AKGR_UI_MODELS_H

#include <cstddef>

namespace akgr::ui {
  class Widget;

  class ListModel {
  public:
    ListModel(const ListModel&) = delete;
    ListModel(ListModel&&) noexcept = default;
    virtual ~ListModel();

    ListModel& operator=(const ListModel&) = delete;
    ListModel& operator=(ListModel&&) noexcept = default;

    virtual void update(Widget* parent) = 0;
    virtual std::size_t widget_count() = 0;
    virtual Widget* widget(std::size_t i) = 0;

  };

}



#endif // AKGR_UI_MODELS_H
