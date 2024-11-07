if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost COMPONENTS unit_test_framework chrono REQUIRED)

add_executable(customobjectstest customobjects.cpp)
target_link_libraries(customobjectstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomObjectsTest COMMAND customobjectstest)

add_executable(customobjectstesttpof customobjectstpof.cpp)
target_link_libraries(customobjectstesttpof wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomObjectsTestTPoF COMMAND customobjectstesttpof)

add_executable(customobjectstestwowr customobjectswowr.cpp)
target_link_libraries(customobjectstestwowr wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomObjectsTestWoWR COMMAND customobjectstestwowr)

add_executable(txttest txt.cpp)
target_link_libraries(txttest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TxtTest COMMAND txttest)

add_executable(slktest slk.cpp)
target_link_libraries(slktest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SlkTest COMMAND slktest)

add_executable(triggerstest triggers.cpp)
target_link_libraries(triggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TriggersTest COMMAND triggerstest)

add_executable(customtexttriggerstest customtexttriggers.cpp)
target_link_libraries(customtexttriggerstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CustomTextTriggersTest COMMAND customtexttriggerstest)

add_executable(triggerstringstest triggerstrings.cpp)
target_link_libraries(triggerstringstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TriggerStringsTest COMMAND triggerstringstest)

add_executable(rectstest rects.cpp)
target_link_libraries(rectstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME RectsTest COMMAND rectstest)

add_executable(soundstest sounds.cpp)
target_link_libraries(soundstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SoundsTest COMMAND soundstest)

configure_file(SkibisCastleTD/war3map.wts SkibisCastleTD/war3map.wts COPYONLY)
add_executable(mapstringstest mapstrings.cpp)
target_link_libraries(mapstringstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME MapStringsTest COMMAND mapstringstest)
# The Power of Fire
configure_file(war3map_Talras_de.wts war3map_Talras_de.wts COPYONLY)
configure_file(war3map_Talras_en.wts war3map_Talras_en.wts COPYONLY)

if (MPQ)
add_executable(wc3maptest wc3map.cpp)
target_link_libraries(wc3maptest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME Wc3MapTest COMMAND wc3maptest)
endif ()

add_executable(infotest info.cpp)
target_link_libraries(infotest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME InfoTest COMMAND infotest)

if (MPQ)
add_executable(warchaserstest warchasers.cpp)
target_link_libraries(warchaserstest wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME WarChasersTest COMMAND warchaserstest)
endif ()

configure_file(customobjects.w3o customobjects.w3o COPYONLY) # The Frozen Throne
configure_file(customobjectstpof.w3o customobjectstpof.w3o COPYONLY) # The Frozen Throne: The Power of Fire
configure_file(reforged/war3map.w3u reforged/war3map.w3u COPYONLY) # Reforged: World of Warcraft Reforged
configure_file(reforged/war3map.w3a reforged/war3map.w3a COPYONLY) # Reforged: World of Warcraft Reforged
configure_file(TriggerData.txt TriggerData.txt COPYONLY) # Warcraft III
configure_file(TriggerDataEx.txt TriggerDataEx.txt COPYONLY) # Warcraft III: The Frozen Throne
configure_file(TriggerStrings.txt TriggerStrings.txt COPYONLY) # Warcraft III
configure_file(WorldEditStrings.txt WorldEditStrings.txt COPYONLY) # The Frozen Throne "War3xlocal.mpq"
configure_file(WorldEditData.txt WorldEditData.txt COPYONLY) # The Frozen Throne "War3Patch.mpq"
configure_file(UnitAbilities.slk UnitAbilities.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitBalance.slk UnitBalance.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitData.slk UnitData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitMetaData.slk UnitMetaData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitMetaDataEx.slk UnitMetaDataEx.slk COPYONLY) # Frozen Throne "war3x.mpq"
configure_file(UnitMetaDataExportedByLibreOffice.slk UnitMetaDataExportedByLibreOffice.slk COPYONLY) # Reign of Chaos "war3.mpq" exported with LibreOffice
configure_file(UnitUI.slk UnitUI.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UnitWeapons.slk UnitWeapons.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(UpgradeData.slk UpgradeData.slk COPYONLY) # Reign of Chaos "war3.mpq"
configure_file(ItemFunc.txt ItemFunc.txt COPYONLY) # Frozen Throne "War3Patch.mpq"
configure_file(ItemStrings.txt ItemStrings.txt COPYONLY) # Frozen Throne "War3Patch.mpq"
configure_file(war3map.wtg war3map.wtg COPYONLY) # War Chasers
configure_file(war3map.wct war3map.wct COPYONLY) # War Chasers
configure_file(TestMap.w3m TestMap.w3m COPYONLY)
configure_file(war3map.w3r war3map.w3r COPYONLY) # Reign of Chaos rects
configure_file(war3map.w3s war3map.w3s COPYONLY) # Reign of Chaos sounds
configure_file(war3map.w3i war3map.w3i COPYONLY) # Reign of Chaos War Chasers
configure_file(war3map.wts war3map.wts COPYONLY) # Reign of Chaos War Chasers
configure_file(UnitEditorData.txt UnitEditorData.txt COPYONLY) # Reign of Chaos (war3.mpq)
configure_file(NeutralUnitFunc.txt NeutralUnitFunc.txt COPYONLY) # Reign of Chaos (war3.mpq)

configure_file(\(4\)WarChasers.w3m \(4\)WarChasers.w3m COPYONLY)
