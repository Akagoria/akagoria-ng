#include "bits/Akagoria.h"

#include "config.h"

int main()
{
  akgr::Akagoria akagoria(AKAGORIA_DATADIR);
  return akagoria.run();
}
