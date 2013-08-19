
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(txttest txt.cpp)
target_link_libraries(txttest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_executable(triggerdatatest triggerdata.cpp)
target_link_libraries(triggerdatatest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_executable(triggerstest triggers.cpp)
target_link_libraries(triggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

configure_file(TriggerData.txt TriggerData.txt COPYONLY) # Warcraft III
configure_file(war3map.wtg war3map.wtg COPYONLY) # War Chasers
