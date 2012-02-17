WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_EDITOR_KIO_MPQ_H)
WC3LIB_GET_SRC(wc3lib_EDITOR_KIO_MPQ_SRC)
set(wc3lib_EDITOR_KIO_MPQ_SRC ${wc3lib_EDITOR_KIO_MPQ_SRC} PARENT_SCOPE)

include_directories(${KDE4_INCLUDE_DIR} ${QT_INCLUDE_DIR})
include(${QT_USE_FILE})

# MPQ KDE plugin
kde4_add_plugin(kio_mpq mpqprotocol.cpp mpqarchive.cpp)
target_link_libraries(kio_mpq wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS})
set_property(TARGET kio_mpq PROPERTY LINK_INTERFACE_LIBRARIES "")

# install
install(TARGETS kio_mpq  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${wc3lib_EDITOR_KIO_MPQ_H} PUBLIC_HEADER DESTINATION include/wc3lib/editor/kio_mpq)