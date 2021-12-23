add_library(TestRegistrarShared SHARED)
target_link_libraries(TestRegistrarShared
		PUBLIC Registrar BaseTest)

add_executable(test_Concepts test_Concepts.cpp)
target_link_libraries(test_Concepts
		PUBLIC TestRegistrarShared)

add_executable(test_Registrars test_Registrars.cpp)
target_link_libraries(test_Registrars
		PUBLIC TestRegistrarShared)

add_library(RegistrationShared SHARED libRegistration.cpp)
target_link_libraries(RegistrationShared
		PUBLIC Registrar)
add_executable(test_Registration test_Registration.cpp)
target_link_libraries(test_Registration
		PUBLIC TestRegistrarShared RegistrationShared)

add_library(test_TypeInfoMod MODULE libTypeInfo.cpp)
target_link_libraries(test_TypeInfoMod
		PUBLIC Registrar)
add_executable(test_TypeInfo test_TypeInfo.cpp)
target_link_libraries(test_TypeInfo
		PUBLIC TestRegistrarShared)
add_dependencies(test_TypeInfo test_TypeInfoMod)

add_library(FactoryShared SHARED libFactory.cpp)
target_link_libraries(FactoryShared
		PUBLIC Registrar)
add_executable(test_Factory test_Factory.cpp)
target_link_libraries(test_Factory
		PUBLIC TestRegistrarShared FactoryShared)