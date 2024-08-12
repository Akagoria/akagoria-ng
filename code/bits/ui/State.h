#ifndef AKGR_UI_STATE_H
#define AKGR_UI_STATE_H

#include <cstdint>

namespace akgr::ui {

  struct IndexState {
    uint32_t choice = 0;
    uint32_t count = 1;

    void compute_next_choice() {
      choice = (choice + 1) % count;
    }

    void compute_prev_choice() {
      choice = (choice + count - 1) % count;
    }
  };

}

#endif // AKGR_UI_STATE_H
