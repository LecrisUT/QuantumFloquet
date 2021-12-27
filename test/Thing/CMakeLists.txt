#add_library(test_ThingMod MODULE
#		libThing.cpp)
add_library(test_ThingMod SHARED
		libThing.cpp)
target_link_libraries(test_ThingMod
		PUBLIC Thing)

add_executable(test_Thing test_Thing.cpp)
target_link_libraries(test_Thing
		PUBLIC Thing BaseTest test_ThingMod)
#target_link_libraries(test_Thing
#		PUBLIC Thing BaseTest)
configure_file(test_Thing.json ${CMAKE_CURRENT_BINARY_DIR}/test_Thing.json COPYONLY)
configure_file(test_Defs.json ${CMAKE_CURRENT_BINARY_DIR}/test_Defs.json COPYONLY)
add_dependencies(test_Thing test_ThingMod)