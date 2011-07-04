WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LANG_VJASS_H)
WC3LIB_GET_SRC(wc3lib_LANG_VJASS_SRC)

# execute_process(COMMAND flex vjass.l WORKING_DIRECTORY src/lang/vjass)
# execute_process(COMMAND bison -d vjass.y WORKING_DIRECTORY src/lang/vjass)

# install
install(FILES ${wc3lib_LANG_VJASS_H} PUBLIC_HEADER DESTINATION include/wc3lib/lang/vjass)