#pragma once
#include "interface/archive/archive.h"
#include <fstream>
namespace Arieo
{
    class OSFileSystemArchive
        : public Interface::Archive::IArchive
    {
    protected:
        std::filesystem::path m_root_path;
        std::unordered_map<std::filesystem::path, std::tuple<void*, size_t>> m_file_buffer_cache_map;
    public:
        OSFileSystemArchive(std::filesystem::path root_path)
            : m_root_path(root_path)
        {

        }

        ~OSFileSystemArchive()
        {
            clearCache();
        }

        std::tuple<void*, size_t> getFileBuffer(const std::filesystem::path& relative_path) override
        {
            auto found_cache_iter = m_file_buffer_cache_map.find(relative_path);
            if(found_cache_iter != m_file_buffer_cache_map.end())
            {
                return found_cache_iter->second;
            }
            else
            {
                std::filesystem::path full_path = m_root_path / relative_path;

                std::ifstream file(full_path, std::ios::binary | std::ios::ate);
                if(file.is_open() == false)
                {
                    Core::Logger::error("Cannot open file: {}", full_path.string());
                    return std::make_tuple(nullptr, 0);
                }

                size_t buffer_size = file.tellg();
                void* buffer = Base::Memory::malloc(buffer_size);
                
                file.seekg(0, std::ios::beg);
                if(!file.read((char*)buffer, buffer_size))
                {
                    Core::Logger::error("Cannot read file: {}", full_path.string());
                    Base::Memory::free(buffer);
                    return std::make_tuple(nullptr, 0);
                }

                m_file_buffer_cache_map.emplace(relative_path, std::make_tuple(buffer, buffer_size));
                return std::make_tuple(buffer, buffer_size);
            }
        }

        void clearCache()
        {
            for(auto iter : m_file_buffer_cache_map)
            {
                void* buffer = std::get<0>(iter.second);
                Base::Memory::free(buffer);
            }
            m_file_buffer_cache_map.clear();
        }
    };

    class OSFileSystemArchiveManager
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
        Base::Interface<Interface::Archive::IArchive> createArchive(const std::filesystem::path& root_path) override
        {
            if(std::filesystem::exists(root_path) == false || std::filesystem::is_directory(root_path) == false)
            {
                Core::Logger::error("Invalid archive root path: {}", root_path.string());
                return nullptr;
            }
            return Base::Interface<Interface::Archive::IArchive>::createAs<OSFileSystemArchive>(root_path);
        }

        void destroyArchive(Base::Interface<Interface::Archive::IArchive> archive) override
        {
            archive.destroyAs<OSFileSystemArchive>();
        }
    };
}