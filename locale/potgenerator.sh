xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/wc3converter.cpp -o wc3converter.pot
xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/wc3editor.cpp -o wc3editor.pot
xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu ../src/app/mpq.cpp -o mpq.pot
xgettext --boost  -aFC --from-code UTF-8 --package-name=wc3lib --package-version=0.1 --msgid-bugs-address=tamino@cdauth.eu -D ../src/ exception.cpp format.cpp libraryloader.cpp utilities.cpp -o wc3lib.pot
msginit -i wc3editor.pot -o de_DE/LC_MESSAGES/wc3editor.po --locale=de_DE
msginit -i wc3converter.pot -o de_DE/LC_MESSAGES/wc3converter.po --locale=de_DE
#msginit -i editor.pot -o en_GB/LC_MESSAGES/editor.po --locale=en_GB