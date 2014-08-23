
find_package(Boost COMPONENTS unit_test_framework chrono REQUIRED)

add_executable(customobjectstest customobjects.cpp)
target_link_libraries(customobjectstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomObjectsTest COMMAND customobjectstest)

add_executable(txttest txt.cpp)
target_link_libraries(txttest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TxtTest COMMAND txttest)

add_executable(slktest slk.cpp)
target_link_libraries(slktest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SlkTest COMMAND slktest)

add_executable(triggerdatatest triggerdata.cpp)
target_link_libraries(triggerdatatest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TriggerDataTest COMMAND triggerdatatest)

add_executable(triggerstest triggers.cpp)
target_link_libraries(triggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TriggersTest COMMAND triggerstest)

add_executable(customtexttriggerstest customtexttriggers.cpp)
target_link_libraries(customtexttriggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomTextTriggersTest COMMAND customtexttriggerstest)

add_executable(triggerstringstest triggerstrings.cpp)
target_link_libraries(triggerstringstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TriggerStringTest COMMAND triggerstringtest)

add_executable(rectstest rects.cpp)
target_link_libraries(rectstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME RectsTest COMMAND rectstest)

add_executable(soundstest sounds.cpp)
target_link_libraries(soundstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SoundsTest COMMAND soundstest)

add_executable(wc3maptest wc3map.cpp)
target_link_libraries(wc3maptest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME Wc3MapTest COMMAND wc3maptest)

add_executable(infotest info.cpp)
target_link_libraries(infotest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME InfoTest COMMAND infotest)

add_executable(warchaserstest warchasers.cpp)
target_link_libraries(warchaserstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME WarChasersTest COMMAND warchaserstest)

configure_file(customobjects.w3o customobjects.w3o COPYONLY) # The Frozen Throne
configure_file(TriggerData.txt TriggerData.txt COPYONLY) # Warcraft III
configure_file(TriggerStrings.txt TriggerStrings.txt COPYONLY) # Warcraft III
configure_file(WorldEditStrings.txt WorldEditStrings.txt COPYONLY) # The Frozen Throne "War3xlocal.mpq"
configure_file(UnitAbilities.slk UnitAbilities.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitBalance.slk UnitBalance.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitData.slk UnitData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitMetaData.slk UnitMetaData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitUI.slk UnitUI.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UpgradeData.slk UpgradeData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(war3map.wtg war3map.wtg COPYONLY) # War Chasers
configure_file(war3map.wct war3map.wct COPYONLY) # War Chasers
configure_file(TestMap.w3m TestMap.w3m COPYONLY)
configure_file(war3map.w3r war3map.w3r COPYONLY) # Reign of Chaos rects
configure_file(war3map.w3s war3map.w3s COPYONLY) # Reign of Chaos sounds
configure_file(war3map.w3i war3map.w3i COPYONLY) # Reign of Chaos War Chasers

configure_file(\(4\)WarChasers.w3m \(4\)WarChasers.w3m COPYONLY)