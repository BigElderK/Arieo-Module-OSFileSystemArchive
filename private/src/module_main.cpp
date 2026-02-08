#include "base/prerequisites.h"
#include "core/core.h"
#include "os_filesystem_archive.h"
namespace Arieo
{
    GENERATOR_MODULE_ENTRY_FUN()
    ARIEO_DLLEXPORT void ModuleMain()
    {
        Core::Logger::setDefaultLogger("os_filesystem_archive");

        static struct DllLoader
        {
            OSFileSystemArchiveManager os_filesystem_archive_manager;

            DllLoader()
            {
                os_filesystem_archive_manager.initialize();
                Core::ModuleManager::registerInterface<Interface::Archive::IArchiveManager>(
                    "os_filesystem_archive", 
                    &os_filesystem_archive_manager
                );
            }

            ~DllLoader()
            {
                Core::ModuleManager::unregisterInterface<Interface::Archive::IArchiveManager>(
                    &os_filesystem_archive_manager
                );
                os_filesystem_archive_manager.finalize();
            }
        } dll_loader;
    }
}