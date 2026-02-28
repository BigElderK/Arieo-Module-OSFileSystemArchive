#pragma once
#include "interface/archive/archive.h"
#include <fstream>
namespace Arieo
{
    class FileBuffer
        : public Interface::Archive::IFileBuffer
    {
    protected:
        void* m_buffer = nullptr;
        size_t m_size = 0;
    public:
        FileBuffer(void* buffer, size_t size)
            : m_buffer(buffer), m_size(size)
        {
        }

        ~FileBuffer()
        {
            if(m_buffer)
            {
                Base::Memory::free(m_buffer);
                m_buffer = nullptr;
            }
        }

        void* getBuffer() override { return m_buffer; }
        size_t getBufferSize() override { return m_size; }
    };

    class OSFileSystemArchive
        : public Interface::Archive::IArchive
    {
    protected:
        std::filesystem::path m_root_path;
        std::unordered_set<Base::Interop<Interface::Archive::IFileBuffer>> m_file_buffers;
    public:
        OSFileSystemArchive(std::filesystem::path root_path)
            : m_root_path(root_path)
        {

        }

        ~OSFileSystemArchive()
        {
            clearCache();
        }

        Base::Interop<Interface::Archive::IFileBuffer> aquireFileBuffer(const Base::Interop<std::string_view>& relative_path) override
        {
            std::filesystem::path full_path = m_root_path / relative_path.getString();

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

            auto file_buffer = Base::Interop<Interface::Archive::IFileBuffer>::createAs<FileBuffer>(buffer, buffer_size);
            m_file_buffers.insert(file_buffer);
            return file_buffer;
        }

        void releaseFileBuffer(Base::Interop<Interface::Archive::IFileBuffer> file_buffer) override
        {
            m_file_buffers.erase(file_buffer);
            Base::Interop<Interface::Archive::IFileBuffer>::destroyAs<FileBuffer>(std::move(file_buffer));
        }

        void clearCache()
        {
            for(auto fb : m_file_buffers)
            {
                Base::Interop<Interface::Archive::IFileBuffer>::destroyAs<FileBuffer>(std::move(fb));
            }
            m_file_buffers.clear();
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
        Base::Interop<Interface::Archive::IArchive> createArchive(const Base::Interop<std::string_view>& root_path) override
        {
            std::filesystem::path root_path_fs(root_path.getString());
            if(std::filesystem::exists(root_path_fs) == false || std::filesystem::is_directory(root_path_fs) == false)
            {
                Core::Logger::error("Invalid archive root path: {}", root_path_fs.string());
                return nullptr;
            }
            return Base::Interop<Interface::Archive::IArchive>::createAs<OSFileSystemArchive>(root_path_fs.string());
        }

        void destroyArchive(Base::Interop<Interface::Archive::IArchive> archive) override
        {
            Base::Interop<Interface::Archive::IArchive>::destroyAs<OSFileSystemArchive>(std::move(archive));
        }
    };
}