set(wc3lib_EDITOR_KIO_MPQ_H
	kio_mpq.hpp
)
set(wc3lib_EDITOR_KIO_MPQ_SRC
	kio_mpq.cpp
)

include_directories(${KDE4_INCLUDE_DIR} ${QT_INCLUDE_DIR})
include(${QT_USE_FILE})

# MPQ KDE plugin
kde4_add_plugin(kio_mpq ${wc3lib_EDITOR_KIO_MPQ_SRC})
target_link_libraries(kio_mpq wc3libmpq wc3libcore wc3libeditor ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS})

if (DEBUG)
	add_subdirectory(test)
endif ()

# install
install(TARGETS kio_mpq  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${wc3lib_EDITOR_KIO_MPQ_H} DESTINATION include/wc3lib/editor/kio_mpq)