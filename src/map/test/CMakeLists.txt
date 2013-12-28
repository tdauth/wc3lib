
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(txttest txt.cpp)
target_link_libraries(txttest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TxtTest COMMAND txttest)
add_executable(triggerdatatest triggerdata.cpp)
target_link_libraries(triggerdatatest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_executable(triggerstest triggers.cpp)
target_link_libraries(triggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_executable(customtexttriggerstest customtexttriggers.cpp)
target_link_libraries(customtexttriggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_executable(triggerstringstest triggerstrings.cpp)
target_link_libraries(triggerstringstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

configure_file(TriggerData.txt TriggerData.txt COPYONLY) # Warcraft III
configure_file(TriggerStrings.txt TriggerStrings.txt COPYONLY) # Warcraft III
configure_file(war3map.wtg war3map.wtg COPYONLY) # War Chasers
configure_file(war3map.wct war3map.wct COPYONLY) # War Chasers