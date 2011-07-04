def bounds(obj):
	s = property("BoundsRadius", '%f', obj.BoundsRadius)
	s += property("MinExtx", '%f', obj.MinExtx)
	s += property("MinExty", '%f', obj.MinExty)
	s += property("MinExtz", '%f', obj.MinExtz)
	s += property("MaxExtx", '%f', obj.MaxExtx)
	s += property("MaxExty", '%f', obj.MaxExty)
	s += property("MaxExtz", '%f', obj.MaxExtz)
	return s

def vertices(obj):
	s = property('nvrts', '%d', obj.nvrts)
	s1 = "<table class=\"vertexList\"><tbody>"
	s1 += "<tr><th>x</th><th>y</th><th>z</th></tr>\n"
	for vrt in obj.vertices:
		s1 += "<tr><td>%f</td><td>%f</td><td>%f</td></tr>\n" % (vrt.x, vrt.y, vrt.z)
	s1 += "</tbody></table>"
	s += property('vertices', '%s', s1, block=obj, hide=1)
	return s

def startBlockDiv(block):
	s = "<div class=\"%s\">\n" % (block.blockname)
	s += "<table><thead>\n"
	s += "<tr><th class=\"blockTitle\"><a href=\"javascript:toggleVis('%s')\">%s</a></th></tr>\n" % (block.id(), block.blockname)
	s += "</thead><tbody><tr><td>\n"
	return s

def startBlockTable(block):
	return "<table class=\"%s\" id=\"%s\"><tbody>\n" % (block.blockname, block.id())

def startBlock(block):
	s = startBlockDiv(block)
	s += startBlockTable(block)
	return s

def endBlock():
	s = "</tbody></table>\n"
	s += "</tbody></table></div>\n"
	return s

def startLoop():
	s = "<table><tbody>\n"
	return s

def endLoop():
	s = "</tbody></table>\n"
	return s

def blockProperty(block, propname, hide=0):
	propval = eval("block." + propname)
	#desc = block.desc[propname][propval]
	#if desc:
	#	desc = " (" + desc + ") "
	propid = "%s.%s" % (block.id(), propname)
	if hide:
		propvis = 'collapse'
	else:
		propvis = 'visible'
	s = "<tr>\n"
	s += "<th><a href=\"javascript:toggleVis('%s')\">%s</a>:</th>\n" % (propid, propname)
	s += "<td id=\"%s\" style=\"visibility: %s\">%s</td>\n" % (propid, propvis, propval)
	s += "</tr>\n"
	return s

def property(name, type, value, desc='', block=None, hide=0):
	if desc:
		desc = " (" + desc + ")"
	headerCell = ''
	if block != None:
		propid = "%s.%s" % (block.id(), name)
		display = ''
		if hide:
			display = "style=\"display: none\""
		s = "<tr><th><a href=\"javascript:toggleVis('%s')\">%s</a>:</th>" % (propid, name)
		s += "<td " + display + " id=\"" + propid + "\">" + type + desc + "</td></tr>\n"
	else:
		s = "<th>%s:</th>" % (name)
		s += "<td>" + type + desc + "</td></tr>\n"
	return s % (value)

def indexProp(i, type, value, desc=''):
	if desc:
		desc = " (" + desc + ")"
	s = "<tr><th class=\"index\">%d:</th><td>" + type + desc + "</td></tr>\n"
	return s % (i, value)

def makeRow(colspan, content):
	return "<tr><td colspan=\"%d\">%s</td></tr>\n" % (colspan, content)

def makeBlockLink(block, separator=1):
	s = ''
	if block.present:
		if separator:
			s += ' | '
		s += "<a href=\"#%s\">%s</a>\n" % (block.id(), block.blockname)
	return s
