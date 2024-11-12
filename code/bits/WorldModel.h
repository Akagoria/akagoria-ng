#ifndef AKGR_WORLD_MODEL_H
#define AKGR_WORLD_MODEL_H

#include <gf2/core/Model.h>

#include "WorldData.h"
#include "WorldRuntime.h"
#include "WorldState.h"

namespace akgr {
  class Akagoria;

  struct WorldModel : public gf::Model {
    WorldModel(Akagoria* game);

    void update(gf::Time time) override;

    Akagoria* game = nullptr;

    WorldData data;
    WorldRuntime runtime;
    WorldState state;

  private:
    void update_hero(gf::Time time);
    void update_dialog(gf::Time time);
    void update_exploration(gf::Time time);
    void update_characters(gf::Time time);
    void update_items(gf::Time time);
    void update_physics(gf::Time time);
    void update_quests(gf::Time time);
    void update_notifications(gf::Time time);

    void check_quest_dialog(const std::string& label);
    void check_quest_explore(const std::string& label);
    void check_quest_farm(const std::string& label);

    template<typename Predicate>
    void check_quest(QuestType type, Predicate predicate);

    static bool advance_in_quest(QuestState& quest);
  };

}

#endif // AKGR_WORLD_MODEL_H
