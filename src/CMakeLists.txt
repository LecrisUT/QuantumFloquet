## Add sources to targets and declare other unexported targets
add_library(Registrar INTERFACE)
add_library(ScriberBase INTERFACE)
add_library(Interfaces INTERFACE)

target_link_libraries(Registrar INTERFACE Base ${CMAKE_DL_LIBS})
target_link_libraries(ScriberBase INTERFACE Registrar)
target_link_libraries(Interfaces INTERFACE ScriberBase)
target_link_libraries(Thing
		PUBLIC Interfaces)

add_subdirectory(Registrar)
add_subdirectory(interface)
add_subdirectory(Thing)