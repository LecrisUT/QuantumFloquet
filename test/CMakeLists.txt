CPMAddPackage(NAME Catch2
		VERSION 2.13.7
		GITHUB_REPOSITORY catchorg/Catch2)

add_library(BaseTest INTERFACE)
target_link_libraries(BaseTest INTERFACE
		fmt::fmt Catch2::Catch2)

add_subdirectory(Registrar)
add_subdirectory(Interfaces)
