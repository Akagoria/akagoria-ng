#include "WorldModel.h"

#include <ctime>

#include <optional>

#include <gf2/core/Math.h>
#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    constexpr float LinearVelocity = 200.0f;
    constexpr float AngularVelocity = 3.0f;

    constexpr float ExplorationDistance = 50.0f;

  }

  WorldModel::WorldModel(Akagoria* game)
  : game(game)
  , runtime(game)
  {
  }

  void WorldModel::update(gf::Time time)
  {
    update_hero(time);
    update_dialog(time);
    update_exploration(time);
    update_characters(time);
    update_items(time);
    update_physics(time);
    update_quests(time);
    update_notifications(time);
  }

  void WorldModel::update_hero(gf::Time time)
  {
    const float dt = time.as_seconds();

    float rotation = state.hero.rotation;
    float velocity = 0.0f;

    if (state.hero.move.method == HeroMoveMethod::Relative) {
      switch (state.hero.move.angular) {
        case gf::AngularMove::None:
          break;
        case gf::AngularMove::Left:
          rotation -= AngularVelocity * dt;
          break;
        case gf::AngularMove::Right:
          rotation += AngularVelocity * dt;
          break;
      }

      switch (state.hero.move.linear) {
        case gf::LinearMove::None:
          break;
        case gf::LinearMove::Forward:
          velocity = LinearVelocity;
          break;
        case gf::LinearMove::Backward:
          velocity = - 0.5f * LinearVelocity;
          break;
      }

    } else {
      if (state.hero.move.linear == gf::LinearMove::Forward) {
        velocity = LinearVelocity;
      }
    }

    runtime.physics.hero.body.set_rotation(rotation);
    runtime.physics.hero.controller.set_rotation(rotation);
    runtime.physics.hero.controller.set_velocity(velocity * gf::unit(rotation));
  }

  void WorldModel::update_dialog([[maybe_unused]] gf::Time time)
  {
    auto& dialog = state.hero.dialog;

    if (!dialog.data) {
      return;
    }

    if (dialog.current_line >= dialog.data->content.size()) {
      // end of the dialog
      const std::string& label = dialog.data->label.tag;
      dialog.data.reset();
      dialog.current_line = 0;

      check_quest_dialog(label);
      game->replace_scene(&game->world_act()->travel_scene);

      // the script can start another dialog so put it at the end
      runtime.script.on_dialog(label);
    }
  }

  void WorldModel::update_exploration([[maybe_unused]] gf::Time time)
  {
    for (auto& location : data.locations) {
      if (location.spot.floor != state.hero.spot.floor) {
        continue;
      }

      if (gf::square_distance(state.hero.spot.location, location.spot.location) < gf::square(ExplorationDistance)) {
        check_quest_explore(location.label.tag);
      }
    }
  }

  void WorldModel::update_characters([[maybe_unused]] gf::Time time)
  {
    // const float dt = time.as_seconds();

    for (auto& character : state.characters) {
      auto physics_iterator = runtime.physics.characters.find(gf::hash_string(character.name));
      assert(physics_iterator != runtime.physics.characters.end());

      auto& [ character_name_id, character_physics ] = *physics_iterator;

      switch (character.behavior_type()) {
        case CharacterBehavior::None:
          break;
        case CharacterBehavior::Stay:
          {
            auto& stay = std::get<CharacterStayState>(character.behavior);
            auto move = stay.location - character.spot.location;

            if (gf::square_length(move) > gf::square(20.0f)) {
              const gf::Vec2F current_direction = gf::unit(character.rotation);

              if (gf::dot(current_direction, move) >= 0.0f) {
                character.rotation = gf::angle(move);
                character_physics.body.set_rotation(character.rotation);
                character_physics.controller.set_rotation(character.rotation);
                character_physics.controller.set_velocity(LinearVelocity * gf::unit(character.rotation));
                character.action = CharacterAction::Forward;
              } else {
                character.rotation = gf::angle(move) + gf::Pi;
                character_physics.body.set_rotation(character.rotation);
                character_physics.controller.set_rotation(character.rotation);
                character_physics.controller.set_velocity(- 0.5f * LinearVelocity * gf::unit(character.rotation));
                character.action = CharacterAction::Backward;
              }
            } else {
              character_physics.controller.set_velocity({ 0.0f, 0.0f });
              character.action = CharacterAction::Static;
            }
          }
          break;
      }
    }
  }

  void WorldModel::update_items([[maybe_unused]] gf::Time time)
  {
    bool need_update = false;

    for (auto& item : state.items) {
      if (item.picked) {
        need_update = true;
        check_quest_farm(item.data->label.tag);
      }
    }

    if (need_update) {
      state.items.erase(std::remove_if(state.items.begin(), state.items.end(), [](const ItemState& item) { return item.picked; }), state.items.end());
    }
  }

  void WorldModel::update_physics(gf::Time time)
  {
    runtime.physics.world.update(time);

    state.hero.rotation = runtime.physics.hero.body.rotation();
    state.hero.spot.location = runtime.physics.hero.body.location();

    for (auto& character : state.characters) {
      if (auto iterator = runtime.physics.characters.find(gf::hash_string(character.name)); iterator != runtime.physics.characters.end()) {
        auto& body = iterator->second.body;
        character.rotation = body.rotation();
        character.spot.location = body.location();
      }
    }

    runtime.script.handle_deferred_messages();
  }

  void WorldModel::update_quests([[maybe_unused]] gf::Time time)
  {
    for (auto& quest : state.hero.quests) {
      if (quest.current_step == quest.data->steps.size()) {
        quest.status = QuestStatus::Finished;
      }
    }
  }

  void WorldModel::update_notifications(gf::Time time)
  {
    if (state.notifications.empty()) {
      return;
    }

    auto& current = state.notifications.front();
    current.elapsed += time;

    if (current.elapsed > current.data->duration) {
      state.notifications.erase(state.notifications.begin());
    }
  }

  void WorldModel::check_quest_dialog(const std::string& label)
  {
    check_quest(QuestType::Talk, [&label]([[maybe_unused]] QuestState& quest, const QuestStepData& step) {
      const auto& data = std::get<TalkQuestData>(step.features);
      return data.dialog->label.tag == label;
    });
  }

  void WorldModel::check_quest_explore(const std::string& label)
  {
    check_quest(QuestType::Explore, [&label]([[maybe_unused]] QuestState& quest, const QuestStepData& step) {
      const auto& data = std::get<ExploreQuestData>(step.features);
      return data.location.origin->label.tag == label;
    });
  }

  void WorldModel::check_quest_farm(const std::string& label)
  {
    check_quest(QuestType::Farm, [&label](QuestState& quest, const QuestStepData& step) {
      const auto& data = std::get<FarmQuestData>(step.features);
      auto& state = std::get<FarmQuestState>(quest.features);

      if (data.item->label.tag == label) {
        ++state.amount;
        return state.amount == data.count;
      }

      return false;
    });
  }

  template<typename Predicate>
  void WorldModel::check_quest(QuestType type, Predicate predicate)
  {
    std::optional<std::string> finished_quest;

    for (auto& quest : state.hero.quests) {
      if (quest.status == QuestStatus::Finished) {
        continue;
      }

      if (quest.type() != type) {
        continue;
      }

      assert(quest.current_step < quest.data->steps.size());

      const QuestStepData& step = quest.data->steps[quest.current_step];
      assert(step.type() == type);

      if (predicate(quest, step)) {
        if (advance_in_quest(quest)) {
          finished_quest = quest.data->label.tag;
        }

        break;
      }
    }

    // get out of the loop because the script can add a new quest
    // and invalidate the current iterator

    if (finished_quest) {
      // TODO: automatic notification for the end of the quest?
      runtime.script.on_quest(*finished_quest);
    }
  }

  bool WorldModel::advance_in_quest(QuestState& quest)
  {
    ++quest.current_step;
    quest.last_update = static_cast<int64_t>(std::time(nullptr));

    if (quest.current_step == quest.data->steps.size()) {
      return true;
    }

    quest.reset_features();
    return false;
  }

}
