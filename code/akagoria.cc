#include <gf2/graphics/Scene.h>
#include <gf2/graphics/SceneManager.h>

#include "config.h"

int main()
{
  gf::SingleSceneManager scene_manager("akagoria", gf::vec(1600, 900));
  gf::StandardScene scene;
  return scene_manager.run(&scene);
}
