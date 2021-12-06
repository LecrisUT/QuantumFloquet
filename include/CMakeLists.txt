## Public interface and metadata of the project go here

configure_file(QuanFloq.h.in QuanFloq.h)
target_include_directories(Base INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(QuanFloq)