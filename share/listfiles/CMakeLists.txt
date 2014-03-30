if (MPQ)
	set(wc3lib_LISTFILES
		Diablo\ I.txt
		Diablo\ II\ LOD.txt
		Hellfire.txt
		Listfile.txt
		Lords\ of\ Magic.txt
		Starcraft\ BW.txt
		Starcraft\ II.txt
		Warcraft\ II\ BNE.txt
		Warcraft\ III.txt
		World\ of\ Warcraft.txt
	)

	install(FILES ${wc3lib_LISTFILES} DESTINATION share/wc3lib/listfiles)
endif ()