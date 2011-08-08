#xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/converter.cpp -o converter.pot
#xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/editor.cpp -o editor.pot
#xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/mpq.cpp -o mpq.pot
#xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu -D ../src/ exception.cpp format.cpp libraryloader.cpp utilities.cpp -o wc3lib.pot
#xgettext --boost -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu -D ../src/ exception.cpp format.cpp libraryloader.cpp utilities.cpp -o wc3lib.pot
#msginit -i editor.pot -o de_DE/LC_MESSAGES/editor.po --locale=de_DE
#msginit -i editor.pot -o en_GB/LC_MESSAGES/editor.po --locale=en_GB

# install
install(FILES de_DE/LC_MESSAGES/converter.mo de_DE/LC_MESSAGES/editor.mo de_DE/LC_MESSAGES/mpq.mo de_DE/LC_MESSAGES/wc3lib.mo DESTINATION usr/share/locale/de_DE/LC_MESSAGES)
#install(FILES en_GB/LC_MESSAGES/converter.mo en_GB/LC_MESSAGES/editor.mo en_GB/LC_MESSAGES/mpq.mo en_GB/LC_MESSAGES/wc3lib.mo DESTINATION usr/share/locale/en_GB/LC_MESSAGES)