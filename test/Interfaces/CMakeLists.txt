add_library(TestScriberShared SHARED)
target_link_libraries(TestScriberShared
		PUBLIC ScriberBase BaseTest)

add_library(ScriberShared SHARED
		libScriber.cpp)
target_link_libraries(ScriberShared
		PUBLIC TestScriberShared)

add_executable(test_Scriber test_Scriber.cpp)
target_link_libraries(test_Scriber
		PUBLIC TestScriberShared ScriberShared)
configure_file(test_Scriber.json ${CMAKE_CURRENT_BINARY_DIR}/test_Scriber.json COPYONLY)