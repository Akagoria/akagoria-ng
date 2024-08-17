#include "bits/Akagoria.h"

#include "config.h"

int main()
{
  const std::filesystem::path assets_directory = akgr::AkagoriaDataDirectory;
  akgr::Akagoria akagoria(assets_directory / "akagoria");
  return akagoria.run();
}
