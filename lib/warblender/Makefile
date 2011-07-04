BLENDERSCRIPTSDIR ?= ~/.blender/scripts

all:

install:
	install warblender_import.py $(BLENDERSCRIPTSDIR)
	install warblender_export.py $(BLENDERSCRIPTSDIR)
	install -d $(BLENDERSCRIPTSDIR)/warblender
	install __init__.py $(BLENDERSCRIPTSDIR)/warblender
	install converter.py $(BLENDERSCRIPTSDIR)/warblender
	install importer.py $(BLENDERSCRIPTSDIR)/warblender
	install exporter.py $(BLENDERSCRIPTSDIR)/warblender
	install -d $(BLENDERSCRIPTSDIR)/warblender/mdx
	install mdx/__init__.py $(BLENDERSCRIPTSDIR)/warblender/mdx
	install mdx/mdx.py $(BLENDERSCRIPTSDIR)/warblender/mdx
	install mdx/html.py $(BLENDERSCRIPTSDIR)/warblender/mdx

uninstall:
	rm -fr $(BLENDERSCRIPTSDIR)/{warblender_import.py,warblender_export.py,warblender}

clean:
	find -name \*.pyc -exec rm -f {} \;

.PHONY: all install clean
