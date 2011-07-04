WC3LIB_ENABLE_EXCEPTIONS()

set(wc3lib_VM_SRC
	vm.cpp
)

set(wc3lib_VM_H
	blizzardj.hpp
	commonai.hpp
	commonj.hpp
	function.hpp
	type.hpp
	variable.hpp
	vm.hpp
)

add_library(wc3libvm SHARED ${wc3lib_VM_SRC})
target_link_libraries(wc3libvm ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

#install

install(TARGETS wc3libvm DESTINATION lib/wc3lib/)
install(FILES ${wc3lib_VM_H} PUBLIC_HEADER DESTINATION include/wc3lib/vm)
