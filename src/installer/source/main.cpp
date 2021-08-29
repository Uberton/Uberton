#include "installer.h"
#include <plugin_defs.h>

namespace Uberton {
namespace Installer {


static Init gAppDelegate(std::make_unique<Installer>(UBERTON_PLUGIN_NAME " Installer (" UBERTON_PLUGIN_VERSION ")", UBERTON_PLUGIN_VERSION));
}
}
