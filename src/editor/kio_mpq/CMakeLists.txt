find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

# Find KDE modules
find_package(KF5 REQUIRED COMPONENTS
	KIO
)

set(wc3lib_EDITOR_KIO_MPQ_H
	kio_mpq.hpp
)
set(wc3lib_EDITOR_KIO_MPQ_SRC
	kio_mpq.cpp
)

# MPQ KDE plugin
add_library(kio_mpq SHARED ${wc3lib_EDITOR_KIO_MPQ_SRC})
target_link_libraries(kio_mpq wc3libmpq wc3libcore wc3libeditor ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core KF5::KIOCore)

if (DEBUG)
	add_subdirectory(test)
endif ()

# install
install(TARGETS kio_mpq  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${wc3lib_EDITOR_KIO_MPQ_H} DESTINATION include/wc3lib/editor/kio_mpq)