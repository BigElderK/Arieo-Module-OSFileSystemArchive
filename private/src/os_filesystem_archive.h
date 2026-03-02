#pragma once
#include "interface/archive/archive.h"
#include <fstream>
namespace Arieo
{
    class OSFileSystemArchive final
        : public Interface::Archive::IArchive
    {
    protected:
        std::filesystem::path m_root_path;
        std::unordered_set<Base::Interop::SharedRef<Base::IBuffer>> m_file_buffers;
    public:
        OSFileSystemArchive(std::filesystem::path root_path)
            : m_root_path(root_path)
        {

        }

        ~OSFileSystemArchive()
        {
            clearCache();
        }

        Base::Interop::SharedRef<Base::IBuffer> aquireFileBuffer(const Base::Interop::StringView& related_path) override
        {
            std::filesystem::path full_path = m_root_path / related_path.getString();

            std::ifstream file(full_path, std::ios::binary | std::ios::ate);
            if(file.is_open() == false)
            {
                Core::Logger::error("Cannot open file: {}", full_path.string());
                return nullptr;
            }

            size_t buffer_size = file.tellg();
            void* buffer = Base::Memory::malloc(buffer_size);
            
            file.seekg(0, std::ios::beg);
            if(!file.read((char*)buffer, buffer_size))
            {
                Core::Logger::error("Cannot read file: {}", full_path.string());
                Base::Memory::free(buffer);
                return nullptr;
            }

            return Base::Interop::SharedRef<Base::IBuffer>::createInstance<Base::Buffer>(
                buffer, 
                buffer_size,
                [](const void* data, size_t size)
                {
                    Base::Memory::free(const_cast<void*>(data));
                }
            );
        }

        void clearCache()
        {
        }
    };

    class OSFileSystemArchiveManager final
        : public Interface::Archive::IArchiveManager
    {
    public:
        void initialize()
        {

        }

        void finalize()
        {

        }
    public:
        Base::Interop::SharedRef<Interface::Archive::IArchive> createArchive(const Base::Interop::StringView& root_path) override
        {
            std::filesystem::path root_path_fs(root_path.getString());
            if(std::filesystem::exists(root_path_fs) == false || std::filesystem::is_directory(root_path_fs) == false)
            {
                Core::Logger::error("Invalid archive root path: {}", root_path_fs.string());
                return nullptr;
            }
            return Base::Interop::SharedRef<Interface::Archive::IArchive>::createInstance<OSFileSystemArchive>(root_path_fs);
        }
    };
}




