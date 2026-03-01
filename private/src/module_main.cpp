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
            OSFileSystemArchiveManager os_file_system_archive_manager_instance;
            Base::Interop::SharedRef<Interface::Archive::IArchiveManager> archive_manager = Base::Interop::makePersistentShared<Interface::Archive::IArchiveManager>(os_file_system_archive_manager_instance);

            DllLoader()
            {
                os_file_system_archive_manager_instance.initialize();
                Core::ModuleManager::registerInterface<Interface::Archive::IArchiveManager>(
                    "os_filesystem_archive",
                    archive_manager
                );
            }

            ~DllLoader()
            {
                Core::ModuleManager::unregisterInterface<Interface::Archive::IArchiveManager>(
                    archive_manager
                );
                os_file_system_archive_manager_instance.finalize();
            }
        } dll_loader;
    }
}




