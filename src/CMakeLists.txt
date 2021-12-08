## Add sources to targets and declare other unexported targets
add_library(Registrar INTERFACE)

target_link_libraries(Registrar INTERFACE Base ${CMAKE_DL_LIBS})

add_subdirectory(Registrar)