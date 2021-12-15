cmake_minimum_required(VERSION 3.19)

project(QuanFloq VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

## Setup CPM (CMake Package Manager)
## TODO: Change to get_cpm to download the latest/specific version
include(cmake/CPM.cmake)
set(CPM_USE_LOCAL_PACKAGES true)


CPMAddPackage(NAME fmt
		VERSION 8.0.1
		GIT_TAG 8.0.1
		GITHUB_REPOSITORY fmtlib/fmt
		OPTIONS "BUILD_SHARED_LIBS TRUE")

# At the bare minimum a JSON interface is needed. Other interfaces are added via option.
CPMAddPackage(NAME json
		VERSION 3.10.4
		GITHUB_REPOSITORY nlohmann/json)
CPMAddPackage(NAME scn
		VERSION 0.4
		GITHUB_REPOSITORY eliaskosunen/scnlib)

## Base target for include and other basic target
add_library(Base INTERFACE)
target_include_directories(Base
		INTERFACE include)

## Public targets


add_subdirectory(src)
add_subdirectory(include)
## TODO: Add an exclude from all for test, docs, etc.
add_subdirectory(test)
add_subdirectory(docs)