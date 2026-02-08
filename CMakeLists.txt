cmake_minimum_required (VERSION 3.31)
    arieo_engine_project(
        arieo_os_filesystem_archive_module
        ALIAS arieo::modules::archive::os_filesystem
        PROJECT_TYPE module
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
        INTERFACES
            arieo::interface::archive
        LIBS
            arieo::base
            arieo::core
    )