add_library(libTestRegistrars SHARED)
target_link_libraries(libTestRegistrars
		PUBLIC Registrar BaseTest)
add_executable(test_Concepts test_Concepts.cpp)
target_link_libraries(test_Concepts
		PUBLIC libTestRegistrars)
add_executable(test_Registrars test_Registrars.cpp)
target_link_libraries(test_Registrars
		PUBLIC libTestRegistrars)