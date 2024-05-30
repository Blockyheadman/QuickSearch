cmake_minimum_required(VERSION 3.28)
project(
        QuickSearch
        VERSION 1.0.0.0
        DESCRIPTION "Searches files that contain a given string."
        HOMEPAGE_URL "https://github.com/Blockyheadman/QuickSearch"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(QuickSearch main.cpp)
