#!/usr/bin/env python

import sys
from optparse import OptionParser
from warblender.mdx import mdx

usage = "%prog [options] <filename>"
opt = OptionParser(usage=usage)
opt.add_option('-t', '--txt', dest=None, action="store_true", default=False, help="text dump of the MDX file (default)")
opt.add_option('-m', '--html', dest=None, action="store_true", default=False, help="HTML dump of the MDX file")

(options, args) = opt.parse_args()

if len(args) != 1:
	opt.print_help()
	sys.exit(1)

reader = mdx.Reader()
model = reader.loadFile(args[0])
if options.html:
	print model.toHTML()
else:
	print str(model)
