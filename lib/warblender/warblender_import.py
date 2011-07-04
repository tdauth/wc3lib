#!BPY

"""
Name: 'Blizzard Warcraft III model file (.mdx) ...'
Blender: 249a
Group: 'Import'
Tooltip: "Import a model file from Blizzard's Warcraft III"
"""

import Blender
from os import *
from warblender.mdx import mdx
from warblender.importer import Importer


def loadMDX(filename):
	r = mdx.Reader()
	mdxModel = None

	try:
		mdxModel = r.loadFile(filename)
	except mdx.MDXFileFormatError:
		Blender.Draw.Text("Could not read model file \"%1%\"." % filename)
		raise

	imp = Importer()
	imp.toBlender(mdxModel)

print "Warblender"
cfgdir = Blender.Get('udatadir');
print "Cfg dir does exist!"
cfgFilePath = str(cfgdir) + "/warblender.autoload"
print "Cfg file path is " + cfgFilePath

try:
	file = open(cfgFilePath, 'r') # There's a config file which can contain default file paths (separated by line breaks)
except:
	file = None

# Read each line contained in file and interpret as file path (trying to open it).
if file != None:
	print "opening file " % cfgFilePath
	loadAnyFile = False

	while (True):
		line = file.readline()
		line = line.strip()
		path = os.path(line)
		print "testing path %s" % path

		if os.path.isfile(path):
			print "path %s exists" % path
			loadAnyFile = True
			loadMDX(path)
		else:
			print "path %s does not exist" % path
			Blender.Draw.Text("Warning: Autload config file entry \"%s\" does is no valid file path." % path)

	# Did not load any file.
	if not loadAnyFile:
		print "Should draw that fucking text!"
		Blender.Draw.Text("Warning: Autload config file \"%s\" does not contain any file paths." % cfgFilePath)
		Blender.Window.FileSelector(loadMDX, "Import MDX")

else:
	Blender.Window.FileSelector(loadMDX, "Import MDX")
