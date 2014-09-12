if (EDITOR OR PLUGINS)
	set (WC3LIB_APPLICATIONS
		wc3editor.desktop
		wc3model.desktop
		wc3mpq.desktop
		wc3object.desktop
		wc3trigger.desktop
	)
	install(FILES ${WC3LIB_APPLICATIONS} DESTINATION share/applications)
endif ()