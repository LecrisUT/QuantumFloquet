target_sources(ScriberBase INTERFACE
		Scriber.cpp ScribeDriver.cpp)

add_library(JSONScriber INTERFACE)
target_sources(JSONScriber INTERFACE
		JSONDriver.cpp)
target_link_libraries(JSONScriber INTERFACE
		fmt::fmt nlohmann_json::nlohmann_json scn::scn-header-only)

target_link_libraries(ScriberBase INTERFACE
		JSONScriber)