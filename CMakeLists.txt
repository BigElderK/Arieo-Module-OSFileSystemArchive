cmake_minimum_required (VERSION 3.31)

include($ENV{ARIEO_BUILDENV_PACKAGE_INSTALL_FOLDER}/cmake/build_environment.cmake)
project(ArieoEngine)

arieo_engine_project(
    arieo_os_filesystem_archive_module
    PROJECT_TYPE module

    PACKAGES
        Arieo-Interface-Archive
        Arieo-Core
    INTERFACES
        Arieo-Interface-Archive::arieo_archive_interface
    PRIVATE_LIBS
        Arieo-Core::arieo_core

    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
)