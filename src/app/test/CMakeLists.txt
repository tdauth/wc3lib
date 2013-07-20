find_package(Boost COMPONENTS system filesystem timer REQUIRED)

add_executable(extraction extraction.cpp extract.cpp)
target_link_libraries(extraction ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES} StormLib)
set_property(TARGET extraction PROPERTY LINK_INTERFACE_LIBRARIES "")