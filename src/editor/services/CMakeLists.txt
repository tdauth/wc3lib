find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

# Find KDE modules
find_package(KF5 REQUIRED)

# install
install(FILES mpq.protocol DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES blp.desktop DESTINATION ${SERVICES_INSTALL_DIR}/qimageioplugins/)