# -*- coding: utf-8 -*-
import sys
import struct
import string
import html

LONG_SIZE = 4
FLOAT_SIZE = 4
SHORT_SIZE = 2
DEBUG = 0

def perror(s):
	if DEBUG:
		sys.stderr.write(s + "\n")

def readLong(file):
	return struct.unpack('l', file.read(LONG_SIZE))[0]

def readFloat(file):
	return struct.unpack('f', file.read(FLOAT_SIZE))[0]

def readByte(file):
	return struct.unpack('B', file.read(1))[0]

def readShort(file):
	return struct.unpack('h', file.read(2))[0]

def readBounds(file, obj):
	obj.BoundsRadius = readFloat(file)
	obj.MinExtx = readFloat(file)
	obj.MinExty = readFloat(file)
	obj.MinExtz = readFloat(file)
	obj.MaxExtx = readFloat(file)
	obj.MaxExty = readFloat(file)
	obj.MaxExtz = readFloat(file)

def getBoundsSize():
	return 7 * FLOAT_SIZE

def boundsToStr(obj):
	s = "BoundsRadius: " + str(obj.BoundsRadius) + "\n"
	s += "MinExtx: " + str(obj.MinExtx) + "\n"
	s += "MinExty: " + str(obj.MinExty) + "\n"
	s += "MinExtz: " + str(obj.MinExtz) + "\n"
	s += "MaxExtx: " + str(obj.MaxExtx) + "\n"
	s += "MaxExty: " + str(obj.MaxExty) + "\n"
	s += "MaxExtz: " + str(obj.MaxExtz) + "\n"
	return s

def readVertices(file, obj):
	obj.nvrts = readLong(file)
	perror(".nvrts: " + str(obj.nvrts) + "\n")
	obj.vertices = []
	for i in range(obj.nvrts):
		vrt = Struct()
		vrt.x = readFloat(file)
		vrt.y = readFloat(file)
		vrt.z = readFloat(file)
		obj.vertices.append(vrt)

def getVerticesSize(obj):
	return LONG_SIZE + (obj.nvrts * 3 * FLOAT_SIZE)

# Removes 0 chars.
def readString(file, length):
	value = str(file.read(length))
	#value = value.strip("PDR")
	index = value.find("\x00")
	print "Value is " + value
	print "Index of read string is " + str(index)
	print "Length of read string is " + str(len(value))

	if index != -1:
		value = value[0:index]

	return value

def writeString(file, string, length):
	file.write(string + ((length - string.len) * '\x00')) # fill empty space with \x00 chars

def deepDictListAdd(dict, location, item, ListAdd=0):
	#sys.stderr.write("deepDictListAdd: " + str(location) + ": " + str(item) + "\n")
	if len(location) == 0:
		return 0
	place = dict
	while len(location) > 1:
		key = location.pop(0)
		if not place.has_key(key):
			place[key] = {}
		place = place[key]
	key = location.pop(0)
	if ListAdd:
		if place.has_key(key):
			if item not in place[key]:
				place[key].append(item)
		else:
			place[key] = [item]
	else:
		place[key] = item

class MDXFileFormatError(Exception):
	def __init__(self, value, file):
		self.value = value
		self.filename = file.name
		self.fileoffset = file.tell()

	def __str__(self):
		return self.filename + " offset " + str(self.fileoffset) + ": " + self.value

class MDXWriteError(Exception):
	def __init__(self, value):
		self.value = value

	def __str__(self):
		return self.value

class Struct:
	""" Empty class for a C-like struct """
	def __str__(self):
		attrs = dir(self)
		attrs.pop(0)
		s = ''
		for attr in attrs:
			if (attr[0] == '_' and attr[1] == '_') or attr == '':
				continue
			s += ".%s = %s\n" % (attr, str(eval('self.' + attr)))
		return s

	def id(self):
		return "struct%d" % (id(self))

class MDXBlock:
	def __init__(self, name, optional=0):
		self.blockname = name
		self.optional = optional
		self.present = 0

	def __str__(self):
		if self.present:
			return "%s\n%s" % (self.blockname, self.toString())
		else:
			return ''

	def readFrom(self, file, model):
		header = file.read(4)
		if len(header) < 4:
			if len(header) == 0:
				if self.optional:
					return
				else:
					raise MDXFileFormatError("unexpected end of file, expected string " + repr(self.blockname), file)
			else:
				raise MDXFileFormatError("unexpected end of file, expected string " + repr(self.blockname) + " (read " + repr(header) + ")", file)
		if header != self.blockname:
			if self.optional:
				self.present = 0
				file.seek(-4, 1)
				#perror(self.blockname + " is optional and absent")
			else:
				raise MDXFileFormatError("expected string " + repr(self.blockname) + " (found " + repr(header) + ")", file)
		else:
			self.present = 1
			self.loadBlock(file, model)

	def loadBlock(self, file, model):
		pass

	def getSize(self):
		if self.present:
			return self.getBlockSize()
		else:
			return 0

	def id(self):
		return "block%d" % id(self)

	def toHTML(self, parent, root):
		if self.present:
			return html.startBlock(self) + self.blockToHTML(parent, root) + html.endBlock()
		else:
			return ''

class Model:
	def __init__(self):
		self.present = 1
		self.filename = None
		self.objects = {}

	def __str__(self):
		s = str(self.MDLX) + "\n"
		s += str(self.VERS) + "\n"
		s += str(self.MODL) + "\n"
		s += str(self.SEQS) + "\n"
		s += str(self.GLBS) + "\n"
		s += str(self.MTLS) + "\n"
		s += str(self.GEOS) + "\n"
		s += str(self.GEOA) + "\n"
		s += str(self.BONE) + "\n"
		s += str(self.LITE) + "\n"
		s += str(self.HELP) + "\n"
		s += str(self.ATCH) + "\n"
		s += str(self.PIVT) + "\n"
		s += str(self.PREM) + "\n"
		s += str(self.PRE2) + "\n"
		s += str(self.RIBB) + "\n"
		s += str(self.CAMS) + "\n"
		s += str(self.EVTS) + "\n"
		s += str(self.CLID) + "\n"
		return s

	def id(self):
		return "block%d" % id(self)

	def putObject(self, key, obj):
		self.objects[key] = obj

	def toHTML(self):
		s = "<html><head>\n"
		s += "<title>MDX Model Output for %s</title>\n" % self.MODL.Name
		s += "<link rel=\"stylesheet\" type=\"text/css\" href=\"mdx.css\">\n"
		s += "<script src=\"mdx.js\"></script>\n"
		s += "</head><body>\n"
		s += "<div class=\"links\">\n"
		s += html.makeBlockLink(self.MDLX, 0)
		s += html.makeBlockLink(self.VERS)
		s += html.makeBlockLink(self.MODL)
		s += html.makeBlockLink(self.SEQS)
		s += html.makeBlockLink(self.GLBS)
		s += html.makeBlockLink(self.MTLS)
		s += html.makeBlockLink(self.TEXS)
		s += html.makeBlockLink(self.TXAN)
		s += html.makeBlockLink(self.GEOS)
		s += html.makeBlockLink(self.GEOA)
		s += html.makeBlockLink(self.BONE)
		s += html.makeBlockLink(self.LITE)
		s += html.makeBlockLink(self.HELP)
		s += html.makeBlockLink(self.ATCH)
		s += html.makeBlockLink(self.PIVT)
		s += html.makeBlockLink(self.PREM)
		s += html.makeBlockLink(self.PRE2)
		s += html.makeBlockLink(self.RIBB)
		s += html.makeBlockLink(self.CAMS)
		s += html.makeBlockLink(self.EVTS)
		s += html.makeBlockLink(self.CLID)
		s += "</div>\n"
		s += self.MDLX.toHTML(self, self)
		s += self.VERS.toHTML(self, self)
		s += self.MODL.toHTML(self, self)
		s += self.SEQS.toHTML(self, self)
		s += self.GLBS.toHTML(self, self)
		s += self.MTLS.toHTML(self, self)
		s += self.TEXS.toHTML(self, self)
		s += self.TXAN.toHTML(self, self)
		s += self.GEOS.toHTML(self, self)
		s += self.GEOA.toHTML(self, self)
		s += self.BONE.toHTML(self, self)
		s += self.LITE.toHTML(self, self)
		s += self.HELP.toHTML(self, self)
		s += self.ATCH.toHTML(self, self)
		s += self.PIVT.toHTML(self, self)
		s += self.PREM.toHTML(self, self)
		s += self.PRE2.toHTML(self, self)
		s += self.RIBB.toHTML(self, self)
		s += self.CAMS.toHTML(self, self)
		s += self.EVTS.toHTML(self, self)
		s += self.CLID.toHTML(self, self)
		s += "</body></html>\n"
		return s

class MDLX(MDXBlock):
	""" MDLX File Format """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'MDLX', optional)
		self.desc = {}
		self.desc['_block'] = "MDLX signature block"

	def loadBlock(self, file, model):
		pass

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		return ''

class VERS(MDXBlock):
	""" Version """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'VERS', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		perror("VERS.nbytes: " + str(self.nbytes))
		self.version = readLong(file)

	def toString(self):
		s = "Version: " + str(self.version) + "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property("Version", '%d', self.version)
		return s


class MODL(MDXBlock):
	""" Model """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'MODL', optional)

	def loadBlock(self, file, model):
		NAMELEN = 0x150
		self.nbytes = readLong(file)
		self.Name = readString(file, NAMELEN)
		self.UNKNOWN_long_0 = readLong(file)
		readBounds(file, self)
		self.BlendTime = readFloat(file)

	def toString(self):
		s = "Name: " + self.Name + "\n"
		s += "UNKNOWN_long_0: " + str(self.UNKNOWN_long_0) + "\n"
		s += boundsToStr(self)
		return s

	def blockToHTML(self, parent, root):
		s = html.property("Name", '%s', self.Name)
		s += html.property("UNKNOWN_long_0", '%d', self.UNKNOWN_long_0)
		s += html.bounds(self)
		s += html.property("BlendTime", '%f', self.BlendTime)
		return s

class SEQS(MDXBlock):
	""" Sequences """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'SEQS', optional)
		self.nseqs = 0

	def loadBlock(self, file, model):
		SEQ_SIZE = 80 + 4 * LONG_SIZE + 9 * FLOAT_SIZE
		NAME_LEN = 80
		self.nbytes = readLong(file)
		self.nseqs = self.nbytes / SEQ_SIZE
		self.seqs = []
		for i in range(self.nseqs):
			seq = Struct()
			seq.Name = readString(file, NAME_LEN)
			seq.IntStart = readLong(file)
			seq.IntEnd = readLong(file)
			seq.MoveSpeed = readFloat(file)
			seq.NoLooping = readLong(file)
			seq.Rarity = readFloat(file)
			seq.UNKNOWN_float_0 = readFloat(file)
			readBounds(file, seq)
			self.seqs.append(seq)

	def toString(self):
		s = "nbytes: %d\n" % (self.nbytes)
		s += "nseqs: %d\n" % (self.nseqs)
		for seq in self.seqs:
			s += "Name: " + repr(seq.Name) + "\n"
			s += "IntStart: " + str(seq.IntStart) + "\n"
			s += "IntEnd: " + str(seq.IntEnd) + "\n"
			s += "MoveSpeed: " + str(seq.MoveSpeed) + "\n"
			s += "NoLooping: " + str(seq.NoLooping) + "\n"
			s += "Rarity: " + str(seq.Rarity) + "\n"
			s += "UNKNOWN_float_0: " + str(seq.UNKNOWN_float_0) + "\n"
			s += boundsToStr(seq)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nseqs', '%d', self.nseqs)
		s1 = html.startLoop()
		for i in range(self.nseqs):
			seq = self.seqs[i]
			loop = html.startLoop()
			loop += html.property('Name', '%s', seq.Name)
			loop += html.property('IntStart', '%d', seq.IntStart)
			loop += html.property('IntEnd', '%d', seq.IntEnd)
			loop += html.property('MoveSpeed', '%f', seq.MoveSpeed)
			loop += html.property('NoLooping', '%d', seq.NoLooping)
			loop += html.property('Rarity', '%f', seq.Rarity)
			loop += html.property('UNKNOWN_float_0', '%f', seq.UNKNOWN_float_0)
			loop += html.bounds(seq)
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('seqs', '%s', s1, block=self, hide=1)
		return s

class GLBS(MDXBlock):
	""" Globals """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'GLBS', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.ndurs = self.nbytes / 4
		perror("GLBS.ndurs = %d\n" % (self.ndurs))
		self.durations = []
		for i in range(self.ndurs):
			self.durations.append(readLong(file))

	def toString(self):
		s = ".nbytes = %d\n" % (self.nbytes)
		s += ".ndurs = %d\n" % (self.ndurs)
		s += ".durations = %s\n" % (repr(self.durations))
		return s

	def blockToHTML(self, parent, root):
		s = html.property('ndurs', '%d', self.ndurs)
		s1 = "<table><tbody>\n"
		for i in range(self.ndurs):
			s1 += html.property(i, '%d', self.durations[i])
		s1 += "</tbody></table>\n"
		s += html.property('durations', '%s', s1)
		return s

class MTLS(MDXBlock):
	""" Material """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'MTLS', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		perror("MTLS.nbytes: " + str(self.nbytes))
		self.materials = []
		self.nmtls = 0
		layersize = 0
		while layersize < self.nbytes:
			material = Struct()
			material.nbytesi = readLong(file)
			layersize += material.nbytesi
			perror("material.nbytesi: " + str(material.nbytesi))
			material.PriorityPlane = readLong(file)
			material.RenderMode = readLong(file)
			material.LAYS = LAYS()
			material.LAYS.readFrom(file, model)
			self.materials.append(material)
			self.nmtls += 1
		perror("MTLS layersize: " + str(layersize))

	def toString(self):
		s = "nbytes: " + str(self.nbytes) + "\n"
		s += "nmtls: " + str(self.nmtls) + "\n"
		for i in range(self.nmtls):
			s += "material[" + str(i) + "].nbytesi: " + str(self.materials[i].nbytesi) + "\n"
			s += "material[" + str(i) + "].PriorityPlane: " + str(self.materials[i].PriorityPlane) + "\n"
			s += "material[" + str(i) + "].RenderMode: " + str(self.materials[i].RenderMode) + "\n"
			s += str(self.materials[i].LAYS)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nmtls', '%d', self.nmtls)
		s1 = html.startLoop()
		for i in range(self.nmtls):
			mat = self.materials[i]
			loop = html.startLoop()
			loop += html.property('PriorityPlane', '%d', mat.PriorityPlane)
			loop += html.property('RenderMode', '%d', mat.RenderMode)
			loop += html.makeRow(2, mat.LAYS.toHTML(mat, root))
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('materials', '%s', s1, block=self, hide=1)
		return s

class LAYS(MDXBlock):
	""" Layers """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'LAYS', optional)

	def loadBlock(self, file, model):
		self.nlays = readLong(file)
		perror("LAYS.nlays: " + str(self.nlays))
		self.layers = []
		for i in range(self.nlays):
			layer = Struct()
			layer.nbytesi = readLong(file)
			perror("LAYS.layer[%d].nbytesi: %d\n" % (i, layer.nbytesi))
			layer.FilterMode = readLong(file)
			layer.Shading = readLong(file)
			layer.TextureID = readLong(file)
			layer.TVertexAnimId = readLong(file)
			layer.CoordId = readLong(file)
			layer.Alpha = readFloat(file)
			layer.KMTA = KMTA(1)
			layer.KMTF = KMTF(1)
			if 6 * LONG_SIZE + FLOAT_SIZE < layer.nbytesi:
				layer.KMTA.readFrom(file, model)
				if 6 * LONG_SIZE + FLOAT_SIZE + layer.KMTA.getSize() < layer.nbytesi:
					layer.KMTF.readFrom(file, model)
			self.layers.append(layer)

	def toString(self):
		s = ".nlays = %d\n" % self.nlays
		for i in range(self.nlays):
			layer = self.layers[i]
			s += ".layer[%d].nbytesi = %d\n" % (i, layer.nbytesi)
			s += ".layer[%d].FilterMode = %d\n" % (i, layer.FilterMode)
			s += ".layer[%d].Shading = %d\n" % (i, layer.Shading)
			s += ".layer[%d].TextureID = %d\n" % (i, layer.TextureID)
			s += ".layer[%d].TVertexAnimId = %d\n" % (i, layer.TVertexAnimId)
			s += ".layer[%d].CoordId = %d\n" % (i, layer.CoordId)
			s += ".layer[%d].Alpha = %f\n" % (i, layer.Alpha)
			s += str(layer.KMTA)
			s += str(layer.KMTF)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nlays', '%d', self.nlays)
		s1 = html.startLoop()
		for i in range(self.nlays):
			layer = self.layers[i]
			loop = html.startLoop()
			loop += html.property('FilterMode', '%d', layer.FilterMode)
			loop += html.property('Shading', '%d', layer.Shading)
			loop += html.property('TextureID', '%d', layer.TextureID)
			loop += html.property('TVertexAnimId', '%d', layer.TVertexAnimId)
			loop += html.property('CoordId', '%d', layer.CoordId)
			loop += html.property('Alpha', '%f', layer.Alpha)
			if layer.KMTA.present:
				loop += html.makeRow(2, layer.KMTA.toHTML(layer, root))
			if layer.KMTF.present:
				loop += html.makeRow(2, layer.KMTF.toHTML(layer, root))
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('layers', '%s', s1, block=self, hide=1)
		return s

class KMTA_Base(MDXBlock):
	def __init__(self, blockname, optional=0):
		MDXBlock.__init__(self, blockname, optional)

	def loadBlock(self, file, model):
		self.size = 4 # we already read the 4-letter header, stoopid
		self.nunks = readLong(file)
		perror("XXXXXX.nunks: " + str(self.nunks))
		self.LineType = readLong(file)
		perror(".LineType: " + str(self.LineType))
		self.GlobalSeqId = readLong(file)
		self.size += 4 + 3 * LONG_SIZE
		self.alpha = []
		for i in range(self.nunks):
			a = Struct()
			a.Frame = readLong(file)
			a.State = readFloat(file)
			self.size += LONG_SIZE + FLOAT_SIZE
			if self.LineType > 1:
				a.InTan = readFloat(file)
				a.OutTan = readFloat(file)
				self.size += 2 * FLOAT_SIZE
			self.alpha.append(a)
		perror("KMTA_BASE size: " + str(self.size))

	def toString(self):
		s = ".nunks = %d\n" % self.nunks
		s += ".LineType = %d\n" % self.LineType
		s += ".GlobalSeqId = %d\n" % self.GlobalSeqId
		for i in range(self.nunks):
			a = self.alpha[i]
			s += ".alpha[%d].Frame = %d\n" % (i, a.Frame)
			s += ".alpha[%d].State = %d\n" % (i, a.State)
			if self.LineType > 1:
				s += ".alpha[%d].InTan = %f\n" % (i, a.InTan)
				s += ".alpha[%d].OutTan = %f\n" % (i, a.OutTan)
		return s

	def getBlockSize(self):
		s = 4 + 3 * LONG_SIZE + (self.nunks * (LONG_SIZE + FLOAT_SIZE))
		if self.LineType > 1:
			s += self.nunks * 2 * FLOAT_SIZE
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nunks', '%d', self.nunks)
		s += html.property('LineType', '%d', self.LineType)
		s += html.property('GlobalSeqId', '%d', self.GlobalSeqId)
		s1 = html.startLoop()
		for i in range(self.nunks):
			a = self.alpha[i]
			loop = html.startLoop()
			loop += html.property('Frame', '%d', a.Frame)
			loop += html.property('State', '%f', a.State)
			if self.LineType > 1:
				loop += html.property('InTan', '%f', a.InTan)
				loop += html.property('OutTan', '%f', a.OutTan)
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('alpha', '%s', s1, block=self, hide=1)
		return s

class KGSC_Base(MDXBlock):
	def __init__(self, blockname, optional=0):
		MDXBlock.__init__(self, blockname, optional)

	def loadBlock(self, file, model):
		nrItems = readLong(file)
		self.LineType = readLong(file)
		self.GlobalSeqId = readLong(file)
		self.array = []
		for i in range(nrItems):
			elem = Struct()
			elem.Frame = readLong(file)
			elem.x = readFloat(file)
			elem.y = readFloat(file)
			elem.z = readFloat(file)
			if self.LineType > 1:
				elem.InTanx = readFloat(file)
				elem.InTany = readFloat(file)
				elem.InTanz = readFloat(file)
				elem.OutTanx = readFloat(file)
				elem.OutTany = readFloat(file)
				elem.OutTanz = readFloat(file)

			self.array.append(elem)

	def toString(self):
		nrItems = len(self.array)
		s = ".nritems = %d\n" % nrItems
		s += ".LineType = %d\n" % self.LineType
		s += ".GlobalSeqId = %d\n" % self.GlobalSeqId
		for i in range(nrItems):
			elem = self.array[i]
			s += ".array[%d].Frame = %d\n" % (i, elem.Frame)
			s += ".array[%d].(x, y, z) = (%f, %f, %f)\n" % (i, elem.x, elem.y, elem.z)
			if self.LineType > 1:
				s += ".array[%d].InTax:(%f, %f, %f)\n" % (i, elem.InTanx, elem.InTaxy, elem.InTanz)
				s += ".array[%d].OutTan:(%f, %f, %f)\n" % (i, elem.OutTanx, elem.OutTaxy, elem.OutTanz)
		return s

	def getBlockSize(self):
		s = 3 * LONG_SIZE
		for elem in self.array:
			s += LONG_SIZE + 3 * FLOAT_SIZE
			if self.LineType > 1:
				s += 6 * FLOAT_SIZE
		return s

	def blockToHTML(self, parent, root):
		nrItems = len(self.array)
		s = html.property('nrItems', '%d', nrItems)
		s += html.property('LineType', '%d', self.LineType)
		s += html.property('GlobalSeqId', '%d', self.GlobalSeqId)
		s1 = html.startLoop()
		for elem_i in range(nrItems):
			elem = self.array[elem_i]
			loop = html.startLoop()
			loop += html.property('Frame', '%d', elem.Frame)
			loop += html.property('x', '%f', elem.x)
			loop += html.property('y', '%f', elem.y)
			loop += html.property('z', '%f', elem.z)
			if self.LineType > 1:
				loop += html.property('InTanx', '%f', elem.InTanx)
				loop += html.property('InTany', '%f', elem.InTany)
				loop += html.property('InTanz', '%f', elem.InTanz)
				loop += html.property('OutTanx', '%f', elem.OutTanx)
				loop += html.property('OutTany', '%f', elem.OutTany)
				loop += html.property('OutTanz', '%f', elem.OutTanz)
			loop += html.endLoop()
			s1 += html.indexProp(elem_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('array', '%s', s1, block=self, hide=1)
		return s

class KMTA(KMTA_Base):
	""" Alpha """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, "KMTA", optional)

class KGSC(KGSC_Base):
	""" Scaling """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, "KGSC", optional)

class KMTF(MDXBlock):
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'KMTF', optional)

	def loadBlock(self, file, model):
		self.size = 4 # we already read the 4-letter header, stoopid
		self.nunks = readLong(file)
		perror("KMTA.nunks: " + str(self.nunks))
		self.LineType = readLong(file)
		perror("KMTA.LineType: " + str(self.LineType))
		self.GlobalSeqId = readLong(file)
		self.size += 4 + 3 * LONG_SIZE
		self.alpha = []
		for i in range(self.nunks):
			a = Struct()
			a.Frame = readLong(file)
			a.State = readLong(file)
			self.size += LONG_SIZE + FLOAT_SIZE
			if self.LineType > 1:
				a.InTan = readFloat(file)
				a.OutTan = readFloat(file)
				self.size += 2 * FLOAT_SIZE
			self.alpha.append(a)

	def blockToHTML(self, parent, root):
		s = html.property('nunks', '%d', self.nunks)
		s += html.property('LineType', '%d', self.LineType)
		s += html.property('GlobalSeqId', '%d', self.GlobalSeqId)
		s1 = html.startLoop()
		for i in range(self.nunks):
			a = self.alpha[i]
			loop = html.startLoop()
			loop += html.property('Frame', '%d', a.Frame)
			loop += html.property('State', '%d', a.State)
			if self.LineType > 1:
				loop += html.property('InTan', '%f', a.InTan)
				loop += html.property('OutTan', '%f', a.OutTan)
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('alpha', '%s', s1, block=self, hide=1)
		return s

class TEXS(MDXBlock):
	""" Textures """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'TEXS', optional)

	def loadBlock(self, file, model):
		TEXTURE_PATH_LEN = 0x100
		self.nbytes = readLong(file)
		self.ntexs = self.nbytes / (3 * LONG_SIZE + TEXTURE_PATH_LEN)
		self.textures = []
		for i in range(self.ntexs):
			tex = Struct()
			tex.ReplaceableID = readLong(file)
			perror("TEXS.tex[" + str(i) + "].ReplaceableID = '" + str(tex.ReplaceableID) + "'\n")
			perror("file offset: " + str(file.tell()) + "\n")
			tex.TexturePath = readString(file, TEXTURE_PATH_LEN)
			perror("file offset: " + str(file.tell()) + "\n")
			perror("TEXS.tex[" + str(i) + "].TexturePath = '" + tex.TexturePath + "'\n")
			tex.UNKNOWN_long_0 = readLong(file)
			tex.UNKNOWN_long_1 = readLong(file)
			self.textures.append(tex)

	def blockToHTML(self, parent, root):
		s = html.property('ntexs', '%s', self.ntexs)
		s1 = html.startLoop()
		for i in range(self.ntexs):
			tex = self.textures[i]
			loop = html.startLoop()
			loop += html.property('ReplaceableID', '%d', tex.ReplaceableID)
			loop += html.property('TexturePath', '%s', tex.TexturePath)
			loop += html.property('UNKNOWN_long_0', '%d', tex.UNKNOWN_long_0)
			loop += html.property('UNKNOWN_long_1', '%d', tex.UNKNOWN_long_1)
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('textures', '%s', s1, block=self, hide=1)
		return s

class TXAN(MDXBlock):
	""" Texture Animations """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'TXAN', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.txanims = []
		self.nanims = 0
		anims_size = 0
		while anims_size < self.nbytes:
			anim = Struct()
			anim.nbytesi = readLong(file)
			anim.KTAT = KTAT(1)
			anim.KTAT.readFrom(file, model)
			anim.KTAR = KTAR(1)
			anim.KTAR.readFrom(file, model)
			anim.KTAS = KTAS(1)
			anim.KTAS.readFrom(file, model)

			self.txanims.append(anim)
			self.nanims += 1
			anims_size += anim.nbytesi

	def blockToHTML(self, parent, root):
		s = html.property('nanims', '%d', self.nanims)
		s1 = html.startLoop()
		for i in range(self.nanims):
			anim = self.txanims[i]
			loop = html.startLoop()
			loop += html.makeRow(2, anim.KTAT.toHTML())
			loop += html.makeRow(2, anim.KTAR.toHTML())
			loop += html.makeRow(2, anim.KTAS.toHTML())
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('txanims', '%s', s1, block=self, hide=1)
		return s

class KTAT(KGSC_Base):
	""" Translation """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, file, 'KTAT', optional)

class KTAR(KGSC_Base):
	""" Rotation """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, file, 'KTAR', optional)

class KTAS(KGSC_Base):
	""" Scaling """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, file, 'KTAS', optional)

class GEOS(MDXBlock):
	""" Geoset """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'GEOS', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		perror("GEOS.nbytes: %d\n" % (self.nbytes))
		self.geosets = []
		self.ngeos = 0
		size_geosets = self.nbytes
		while size_geosets > 0:
			size_loop = 0
			obj = Struct()
			obj.nbytes = readLong(file)
			#size_loop += 4 # don't count this
			perror("GEOS.objects[%d].nbytes: %d\n" % (self.ngeos, obj.nbytes))
			perror("file offset: %d\n" % (file.tell()))
			obj.VRTX = VRTX()
			obj.VRTX.readFrom(file, model)
			size_loop += obj.VRTX.getSize()
			perror("GEOS.objects[%d].VRTX size: %d\n" % (self.ngeos, obj.VRTX.getSize()))
			obj.NRMS = NRMS()
			obj.NRMS.readFrom(file, model)
			size_loop += obj.NRMS.getSize()
			perror("GEOS.objects[%d].NRMS size: %d\n" % (self.ngeos, obj.NRMS.getSize()))
			obj.PTYP = PTYP()
			obj.PTYP.readFrom(file, model)
			size_loop += obj.PTYP.getSize()
			perror("GEOS.objects[%d].PTYP size: %d\n" % (self.ngeos, obj.PTYP.getSize()))
			obj.PCNT = PCNT()
			obj.PCNT.readFrom(file, model)
			size_loop += obj.PCNT.getSize()
			perror("GEOS.objects[%d].PCNT size: %d\n" % (self.ngeos, obj.PCNT.getSize()))
			perror("file offset: %d\n" % (file.tell()))
			obj.PVTX = PVTX()
			obj.PVTX.readFrom(file, model)
			size_loop += obj.PVTX.getSize()
			perror("GEOS.objects[%d].PVTX size: %d\n" % (self.ngeos, obj.PVTX.getSize()))
			perror("file offset: %d\n" % (file.tell()))
			obj.GNDX = GNDX()
			obj.GNDX.readFrom(file, model)
			size_loop += obj.GNDX.getSize()
			obj.MTGC = MTGC()
			obj.MTGC.readFrom(file, model)
			size_loop += obj.MTGC.getSize()
			perror("file offset: %d\n" % (file.tell()))
			obj.MATS = MATS()
			obj.MATS.readFrom(file, model)
			size_loop += obj.MATS.getSize()
			perror("GEOS.objects[%d].MATS size: %d\n" % (self.ngeos, obj.MATS.getSize()))
			obj.MaterialID = readLong(file)
			size_loop += LONG_SIZE
			perror("GEOS.objects[%d].MaterialID: %d\n" % (self.ngeos, obj.MaterialID))
			obj.SelectionGroup = readLong(file)
			size_loop += LONG_SIZE
			perror("GEOS.objects[%d].SelectionGroup: %d\n" % (self.ngeos, obj.SelectionGroup))
			obj.Selectable = readLong(file)
			size_loop += LONG_SIZE
			perror("GEOS.objects[%d].Selectable: %d\n" % (self.ngeos, obj.Selectable))

			perror("file offset: %d\n" % (file.tell()))
			readBounds(file, obj)
			size_loop += getBoundsSize()

			perror("file offset: %d\n" % (file.tell()))
			obj.nganim = readLong(file)
			size_loop += LONG_SIZE
			perror("GEOS.objects[%d].nganim: %d\n" % (self.ngeos, obj.nganim))
			obj.ganimations = []
			perror("file offset: %d\n" % (file.tell()))
			for j in range(obj.nganim):
				anim = Struct()
				readBounds(file, anim)
				size_loop += getBoundsSize()
				obj.ganimations.append(anim)
			obj.UVAS = UVAS()
			obj.UVAS.readFrom(file, model)
			size_loop += obj.UVAS.getSize()
			self.geosets.append(obj)
			size_geosets -= LONG_SIZE + obj.nbytes
			self.ngeos += 1
			perror("file offset: %d\n" % (file.tell()))
			#if size_loop != obj.nbytes:
				#raise MDXFileFormatError("loop size (%d) and nbytes (%d) don't match" % (size_loop, obj.nbytes), file)

	def toString(self):
		s = ".nbytes = %d\n" % self.nbytes
		s += ".ngeos = %d\n" % self.ngeos
		for i in range(self.ngeos):
			obj = self.geosets[i]
			s += ".geosets[%d].nbytes = %d\n" % (i, obj.nbytes)
			s += str(obj.VRTX)
			s += str(obj.NRMS)
			s += str(obj.PTYP)
			s += str(obj.PCNT)
			s += str(obj.PVTX)
			s += str(obj.GNDX)
			s += str(obj.MTGC)
			s += str(obj.MATS)
			s += ".geosets[%d].MaterialID = %d\n" % (i, obj.MaterialID)
			s += ".geosets[%d].SelectionGroup = %d\n" % (i, obj.SelectionGroup)
			s += ".geosets[%d].Selectable = %d\n" % (i, obj.Selectable)
			s += ".geosets[%d].BoundsRadius = %f\n" % (i, obj.BoundsRadius)
			s += ".geosets[%d].MinExtx = %f\n" % (i, obj.MinExtx)
			s += ".geosets[%d].MinExty = %f\n" % (i, obj.MinExty)
			s += ".geosets[%d].MinExtz = %f\n" % (i, obj.MinExtz)
			s += ".geosets[%d].MaxExtx = %f\n" % (i, obj.MaxExtx)
			s += ".geosets[%d].MaxExty = %f\n" % (i, obj.MaxExty)
			s += ".geosets[%d].MaxExtz = %f\n" % (i, obj.MaxExtz)
			s += ".geosets[%d].nganim = %d\n" % (i, obj.nganim)
			for j in range(obj.nganim):
				ganim = obj.ganimations[j]
				s += ".geosets[%d].ganimations[%d].BoundsRadius = %f\n" % (i, j, ganim.BoundsRadius)
				s += ".geosets[%d].ganimations[%d].MinExtx = %f\n" % (i, j, ganim.MinExtx)
				s += ".geosets[%d].ganimations[%d].MinExty = %f\n" % (i, j, ganim.MinExty)
				s += ".geosets[%d].ganimations[%d].MinExtz = %f\n" % (i, j, ganim.MinExtz)
				s += ".geosets[%d].ganimations[%d].MaxExtx = %f\n" % (i, j, ganim.MaxExtx)
				s += ".geosets[%d].ganimations[%d].MaxExty = %f\n" % (i, j, ganim.MaxExty)
				s += ".geosets[%d].ganimations[%d].MaxExtz = %f\n" % (i, j, ganim.MaxExtz)
			s += str(obj.UVAS)
		return s

	def getBlockSize(self):
		return 4 + self.nbytes

	def blockToHTML(self, parent, root):
		s = html.property('ngeos', '%d', self.ngeos)
		s1 = html.startLoop()
		for i in range(self.ngeos):
			obj = self.geosets[i]
			loop = html.startLoop()
			loop += html.makeRow(2, obj.VRTX.toHTML(obj, root))
			loop += html.makeRow(2, obj.NRMS.toHTML(obj, root))
			loop += html.makeRow(2, obj.PTYP.toHTML(obj, root))
			loop += html.makeRow(2, obj.PCNT.toHTML(obj, root))
			loop += html.makeRow(2, obj.PVTX.toHTML(obj, root))
			loop += html.makeRow(2, obj.GNDX.toHTML(obj, root))
			loop += html.makeRow(2, obj.MTGC.toHTML(obj, root))
			loop += html.makeRow(2, obj.MATS.toHTML(obj, root))
			loop += html.property('MaterialID', '%d', obj.MaterialID)
			loop += html.property('SelectionGroup', '%d', obj.SelectionGroup)
			loop += html.property('Selectable', '%d', obj.Selectable)
			loop += html.bounds(obj)
			loop += html.property('nganim', '%d', obj.nganim)
			s2 = html.startLoop()
			for j in range(obj.nganim):
				ganim = obj.ganimations[j]
				loop1 = html.startLoop()
				loop1 += html.bounds(ganim)
				loop1 += html.endLoop()
				s2 += html.indexProp(j, '%s', loop1)
			s2 += html.endLoop()
			loop += html.property('ganimations', '%s', s2, block=obj, hide=1)
			loop += html.makeRow(2, obj.UVAS.toHTML(obj, root))
			loop += html.endLoop()
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('geosets', '%s', s1, block=self, hide=1)
		return s


class VRTX(MDXBlock):
	""" Vertices """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'VRTX', optional=0)
		self.desc = {}
		self.desc['vertices'] = {}

	def loadBlock(self, file, model):
		readVertices(file, self)

	def getBlockSize(self):
		return 4 + getVerticesSize(self)

	def toString(self):
		s = ".nvrts = %d\n" % self.nvrts
		for i in range(self.nvrts):
			vrt = self.vertices[i]
			s += "(%f, %f, %f) " % (vrt.x, vrt.y, vrt.z)
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.vertices(self)
		return s

class NRMS(MDXBlock):
	""" Normals """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'NRMS', optional)

	def loadBlock(self, file, model):
		readVertices(file, self)

	def getBlockSize(self):
		return 4 + getVerticesSize(self)

	def toString(self):
		s = ".nvrts = %d\n" % self.nvrts
		for i in range(self.nvrts):
			vrt = self.vertices[i]
			s += "(%f, %f, %f) " % (vrt.x, vrt.y, vrt.z)
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.vertices(self)
		return s

class PTYP(MDXBlock):
	""" Primitive Type """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PTYP', optional)

	def loadBlock(self, file, model):
		self.nptyps = readLong(file)
		self.primType = []
		for i in range(self.nptyps):
			self.primType.append(readLong(file))

	def getBlockSize(self):
		return 4 + LONG_SIZE + self.nptyps * LONG_SIZE

	def toString(self):
		s = ".nptyps = %d\n" % self.nptyps
		for i in range(self.nptyps):
			s += "%d " % self.primType[i]
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nptyps', '%d', self.nptyps)
		s1 = html.startLoop()
		for i in range(self.nptyps):
			s1 += html.indexProp(i, '%d', self.primType[i])
		s1 += html.endLoop()
		s += html.property('primType', '%s', s1, block=self, hide=1)
		return s

class PCNT(MDXBlock):
	""" Primitive Sizes"""
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PCNT', optional)

	def loadBlock(self, file, model):
		self.npcnts = readLong(file)
		self.primCount = []
		for i in range(self.npcnts):
			self.primCount.append(readLong(file))

	def getBlockSize(self):
		return 4 + LONG_SIZE + self.npcnts * LONG_SIZE

	def toString(self):
		s = ".npcnts = %d\n" % self.npcnts
		for i in range(self.npcnts):
			s += "(%d) " % self.primCount[i]
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property('npcnts', '%d', self.npcnts)
		s1 = html.startLoop()
		for i in range(self.npcnts):
			s1 += html.indexProp(i, '%d', self.primCount[i])
		s1 += html.endLoop()
		s += html.property('primCount', '%s', s1)
		return s

class PVTX(MDXBlock):
	""" Primitive Vertices """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PVTX', optional)
		self.desc = {}
		self.desc['_block'] = 'Primitive vertex list';

	def loadBlock(self, file, model):
		self.nvrts = readLong(file)
		perror("PVTX.nvrts: %d\n" % (self.nvrts))
		self.vertices = []
		for i in range(self.nvrts):
			self.vertices.append(readShort(file))

	def toString(self):
		s = ".nvrts: %d\n" % (self.nvrts)
		for i in range(self.nvrts / 3):
			s += ("[%d, %d, %d] " % (self.vertices[3 * i], self.vertices[3 * i + 1], self.vertices[3 * i + 2]))
		s += "\n"
		return s

	def getBlockSize(self):
		return 4 + LONG_SIZE + (self.nvrts * SHORT_SIZE)

	def blockToHTML(self, parent, root):
		s = html.property('nvrts', '%d', self.nvrts)
		s1 = html.startLoop()
		for i in range(self.nvrts / 9):
			loop = "[%d, %d, %d]" % (self.vertices[9 * i], self.vertices[9 * i + 1], self.vertices[9 * i + 2])
			loop += " ==&gt; [%d, %d, %d]" % (self.vertices[9 * i + 3], self.vertices[9 * i + 4], self.vertices[9 * i + 5])
			loop += " ==&gt; [%d, %d, %d]" % (self.vertices[9 * i + 6], self.vertices[9 * i + 7], self.vertices[9 * i + 8])
			s1 += html.indexProp(i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('vertices', '%s', s1, block=self, hide=1)
		return s

class GNDX(MDXBlock):
	""" Group Vertices """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'GNDX', optional)

	def loadBlock(self, file, model):
		self.nvgrps = readLong(file)
		perror("GNDX.nvgrps: %d\n" % (self.nvgrps))
		self.vertexGroups = []
		for i in range(self.nvgrps):
			self.vertexGroups.append(readByte(file))

	def getBlockSize(self):
		return 4 + LONG_SIZE + self.nvgrps

	def toString(self):
		s = ".nvgrps = %d\n" % self.nvgrps
		for i in range(self.nvgrps):
			s += "%d " % self.vertexGroups[i]

		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property('nvgrps', '%d', self.nvgrps)
		s1 = html.startLoop()
		for i in range(self.nvgrps):
			s1 += html.indexProp(i, '%d', self.vertexGroups[i])
		s1 += html.endLoop()
		s += html.property('vertexGroups', '%s', s1, block=self, hide=1)
		return s

class MTGC(MDXBlock):
	""" lengths for each group for MATS """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'MTGC', optional)

	def loadBlock(self, file, model):
		self.nmtrcs = readLong(file)
		self.groupCount = []
		for i in range(self.nmtrcs):
			self.groupCount.append(readLong(file))

	def getBlockSize(self):
		return 4 + LONG_SIZE + self.nmtrcs * LONG_SIZE

	def toString(self):
		s = ".nmtrcs = %d\n" % self.nmtrcs
		for i in range(self.nmtrcs):
			s += "%d " % self.groupCount[i]
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property('number of matrices (nmtrcs)', '%d', self.nmtrcs)
		s1 = html.startLoop()
		for i in range(self.nmtrcs):
			s1 += html.indexProp(i, '%d', self.groupCount[i])
		s1 += html.endLoop()
		s += html.property('groupCount', '%s', s1, block=self, hide=1)
		return s

class MATS(MDXBlock):
	""" Matrices """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'MATS', optional)

	def loadBlock(self, file, model):
		self.nmats = readLong(file)
		perror("MATS.nmtrs: %d\n" % (self.nmats))
		self.matrices = []
		for i in range(self.nmats):
			self.matrices.append(readLong(file))

	def getGroup(self, parent, i):
		group = []
		mats_i = 0
		for matsize_i in range(parent.MTGC.nmtrcs):
			for group_i in range(parent.MTGC.groupCount[matsize_i]):
				if matsize_i == i:
					group.append(self.matrices[mats_i])
				mats_i += 1
		return group

	def getBlockSize(self):
		return 4 + LONG_SIZE + self.nmats * LONG_SIZE

	def toString(self):
		s = ".nmats = %d\n" % self.nmats
		for i in range(self.nmats):
			s += "%d " % self.matrices[i]
		s += "\n"
		return s

	def blockToHTML(self, parent, model):
		s = html.property('number of matrix elements (nmats)', '%d', self.nmats)
		mats_i = 0
		s1 = html.startLoop()
		for matsize_i in range(parent.MTGC.nmtrcs):
			loop = '['
			for i in range(parent.MTGC.groupCount[matsize_i]):
				loop += " %d" % self.matrices[mats_i]
				mats_i += 1
			loop += " ]\n"
			s1 += html.indexProp(matsize_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('matrices', '%s', s1, block=self, hide=1)
		return s

class UVAS(MDXBlock):
	""" Texture Patches? """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'UVAS', optional)

	def loadBlock(self, file, model):
		self.ntvrts = readLong(file)
		perror("UVAS.ntvrts = %d" % (self.ntvrts))
		self.UVBS = []
		perror("range = " + str(range(self.ntvrts)))
		for i in range(self.ntvrts):
			perror("About to read UVBS")
			uvbs = UVBS()
			uvbs.readFrom(file, model)
			self.UVBS.append(uvbs)

	def getBlockSize(self):
		s = 4 + LONG_SIZE
		for obj in self.UVBS:
			s += obj.getSize()
		return s

	def toString(self):
		s = ".ntvrts = %d\n" % self.ntvrts;
		for i in range(self.ntvrts):
			s += str(self.UVBS[i])
		return s

	def blockToHTML(self, parent, root):
		s = html.property('ntvrts', '%d', self.ntvrts)
		s1 = html.startLoop()
		for uvbs_i in range(self.ntvrts):
			s1 += html.indexProp(uvbs_i, '%s', self.UVBS[uvbs_i].toHTML(self, root))
		s1 += html.endLoop()
		s += html.property('uvbs', '%s', s1)
		return s

class UVBS(MDXBlock):
	""" Texture Coords """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'UVBS', optional)

	def loadBlock(self, file, model):
		self.nvrts = readLong(file)
		perror("UVBS.nvrts = %d" % self.nvrts)
		self.vertices = []
		for i in range(self.nvrts):
			vrt = Struct()
			vrt.x = readFloat(file)
			vrt.y = readFloat(file)
			self.vertices.append(vrt)

	def getBlockSize(self):
		return 4 + LONG_SIZE + (self.nvrts * 2 * FLOAT_SIZE)

	def toString(self):
		s = ".nvrts = %d\n" % self.nvrts
		for i in range(self.nvrts):
			s += "(%f, %f) " % (self.vertices[i].x, self.vertices[i].y)
		s += "\n"
		return s

	def blockToHTML(self, parent, root):
		s = html.property('number or vertices (nvrts)', '%d', self.nvrts)
		s1 = html.startLoop()
		for vrt_i in range(self.nvrts):
			vrt = self.vertices[vrt_i]
			loop = "[%f, %f]" % (vrt.x, vrt.y)
			s1 += html.indexProp(vrt_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('vertices', '%s', s1, block=self, hide=1)
		return s

class GEOA(MDXBlock):
	""" Geoset Anim """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'GEOA', optional)
		self.desc = {}
		self.desc['ColorAnimation'] = {0: 'none', 1: 'DropShadow', 2: 'Color', 3: 'Both'}

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.ngsan = 0
		self.geosanims = []
		size_geosanims = 0
		while size_geosanims < self.nbytes:
			anim = Struct()
			anim.nbytesi = readLong(file)
			anim.staticAlpha = readFloat(file)
			anim.ColorAnimation = readLong(file)
			anim.ColorR = readFloat(file)
			anim.ColorG = readFloat(file)
			anim.ColorB = readFloat(file)
			anim.GeosetID = readLong(file)
			anim.KGAO = KGAO()
			anim.KGAC = KGAC()
			if (3 * LONG_SIZE + 4 * FLOAT_SIZE) < anim.nbytesi:
				anim.KGAO.readFrom(file, model)
				if (3 * LONG_SIZE + 4 * FLOAT_SIZE + anim.KGAO.getSize()) < anim.nbytesi:
					anim.KGAC.readFrom(file, model)
			self.geosanims.append(anim)
			self.ngsan += 1
			size_geosanims += anim.nbytesi

	def toString(self):
		s = "nbytes = %d\n" % self.nbytes
		s += "ngsan = %d\n" % self.ngsan
		for anim in self.geosanims:
			s += str(anim) + "\n"
			s += "nbytesi = %d\n" % (anim.nbytesi)
			s += "staticAlpha = %f\n" % (anim.staticAlpha)
			s += "ColorAnimation = %d\n" % (anim.ColorAnimation)
			s += "ColorR, ColorG, ColorB = %f, %f, %f\n" % (anim.ColorR, anim.ColorG, anim.ColorB)
			s += "GeosetID = %d\n" % (anim.GeosetID)
			if anim.KGAO.present:
				s += str(anim.KGAO)
			if anim.KGAC.present:
				s += str(anim.KGAC)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('ngsan', '%d', self.ngsan)
		s1 = html.startLoop()
		for anim_i in range(self.ngsan):
			anim = self.geosanims[anim_i]
			loop = html.startLoop()
			loop += html.property('staticAlpha', '%f', anim.staticAlpha)
			loop += html.property('ColorAnimation', '%d', anim.ColorAnimation, self.desc['ColorAnimation'][anim.ColorAnimation])
			loop += html.property('ColorR, ColorG, ColorB', '%s', ('%f, %f, %f' % (anim.ColorR, anim.ColorG, anim.ColorB)))
			loop += html.property('GeosetID', '%d', anim.GeosetID)
			loop += html.makeRow(2, anim.KGAO.toHTML(parent, root))
			loop += html.makeRow(2, anim.KGAC.toHTML(parent, root))
			loop += html.endLoop()
			s1 += html.indexProp(anim_i, '%s', loop)
		s1 += html.endLoop()
		s = html.property('geosanims', '%s', s1, block=self, hide=1)
		return s

class KGAO(KMTA_Base):
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, "KGAO", optional)

class KGAC(KGSC_Base):
	""" GEOA Color """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KGAC', optional)

class BONE(MDXBlock):
	""" Bones """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'BONE', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.bones = []
		self.nbons = 0
		bones_size = 0
		while bones_size < self.nbytes:
			bone = Struct()
			bone.OBJ = OBJ()
			bone.OBJ.readFrom(file, model)
			bone.GeosetID = readLong(file)
			perror(".bone.GeosetID = %d" % bone.GeosetID)
			bone.GeosetAnimID = readLong(file)

			self.bones.append(bone)
			self.nbons += 1
			bones_size += bone.OBJ.getSize() + 2 * LONG_SIZE

	def toString(self):
		s = ".nbytes = %s\n" % (self.nbytes)
		for i in range(self.nbons):
			bone = self.bones[i]
			s += str(bone.OBJ)
			s += ".GeosetID = %d\n" % (bone.GeosetID)
			s += ".GeosetAnimID = %d\n" % (bone.GeosetAnimID)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('number of bones (nbons)', '%d', self.nbons)
		s1 = html.startLoop()
		for bone_i in range(self.nbons):
			bone = self.bones[bone_i]
			loop = html.startLoop()
			loop += html.makeRow(2, bone.OBJ.toHTML(bone, root))
			loop += html.property('GeosetID', '%d', bone.GeosetID)
			loop += html.property('GeosetAnimID', '%d', bone.GeosetAnimID)
			loop += html.endLoop()
			s1 += html.indexProp(bone_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('bones', '%s', s1, block=self, hide=1)
		return s

class LITE(MDXBlock):
	""" Light """
	TYPE_OMNI = 0
	TYPE_DIR = 1
	TYPE_AMB = 2
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'LITE', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		perror(".nbytes = %d" % (self.nbytes))
		self.lights = []
		self.nlits = 0
		lights_size = 0
		while lights_size < self.nbytes:
			light = Struct()
			light.nbytesi = readLong(file)
			perror(".light.nbytesi = %d" % (light.nbytesi))
			light.OBJ = OBJ()
			light.OBJ.readFrom(file, model)
			light.Type = readLong(file)
			perror(".light.Type = %d" % (light.Type))
			light.AttStart = readFloat(file)
			perror(".light.AttStart = %f" % (light.AttStart))
			light.AttEnd = readFloat(file)
			perror(".light.AttEnd = %f" % (light.AttEnd))
			light.ColR = readFloat(file)
			perror(".light.ColR = %f" % (light.ColR))
			light.ColG = readFloat(file)
			perror(".light.ColG = %f" % (light.ColG))
			light.ColB = readFloat(file)
			perror(".light.ColB = %f" % (light.ColB))
			light.Intensity = readFloat(file)
			perror(".light.Intensity = %f" % (light.Intensity))
			light.AmbColR = readFloat(file)
			perror(".light.AmbColR = %f" % (light.AmbColR))
			light.AmbColG = readFloat(file)
			perror(".light.AmbColG = %f" % (light.AmbColG))
			light.AmbColB = readFloat(file)
			perror(".light.AmbColB = %f" % (light.AmbColB))
			light.AmbIntensity = readFloat(file)
			perror(".light.AmbIntensity = %f" % (light.AmbIntensity))
			light.KLAI = KLAI(1)
			light.KLAI.readFrom(file, model)
			light.KLAV = KLAV(1)
			light.KLAV.readFrom(file, model)
			light.KLAC = KLAC(1)
			light.KLAC.readFrom(file, model)
			light.KLBC = KLBC(1)
			light.KLBC.readFrom(file, model)
			light.KLBI = KLBI(1)
			light.KLBI.readFrom(file, model)

			self.lights.append(light)
			self.nlits += 1
			lights_size += 2 * LONG_SIZE + 10 * FLOAT_SIZE
			lights_size += light.OBJ.getSize()
			lights_size += light.KLAI.getSize()
			lights_size += light.KLAV.getSize()
			lights_size += light.KLAC.getSize()
			lights_size += light.KLBC.getSize()
			lights_size += light.KLBI.getSize()
			perror(" lights_size = %d" % (lights_size))

	def blockToHTML(self, parent, root):
		s = html.property('nlits', '%d', self.nlits)
		s1 = html.startLoop()
		for lite_i in range(self.nlits):
			lite = self.lights[lite_i]
			loop = html.startLoop()
			loop += html.makeRow(2, light.OBJ.toHTML(light, root))
			loop += html.property('Type', '%d', light.Type)
			loop += html.property('AttStart', '%f', light.AttStart)
			loop += html.property('AttEnd', '%f', light.AttEnd)
			loop += html.property('ColR', '%f', light.ColR)
			loop += html.property('ColG', '%f', light.ColG)
			loop += html.property('ColB', '%f', light.ColB)
			loop += html.property('Intensity', '%f', light.Intensity)
			loop += html.property('AmbColR', '%f', light.AmbColR)
			loop += html.property('AmbColG', '%f', light.AmbColG)
			loop += html.property('AmbColB', '%f', light.AmbColB)
			loop += html.property('AmbIntensity', '%f', light.AmbIntensity)
			loop += html.makeRow(2, light.KLAI.toHTML(light, root))
			loop += html.makeRow(2, light.KLAV.toHTML(light, root))
			loop += html.makeRow(2, light.KLAC.toHTML(light, root))
			loop += html.makeRow(2, light.KLBC.toHTML(light, root))
			loop += html.makeRow(2, light.KLBI.toHTML(light, root))
			loop += html.endLoop()
			s1 += html.indexProp(lite_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('lights', '%s', s1, block=self, hide=1)
		return s

class KLAI(KMTA_Base):
	""" Intensity """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KLAI', optional)

class KLAV(KMTA_Base):
	""" Visibility """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KLAV', optional)

class KLAC(KGSC_Base):
	""" Color """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KLAC', optional)

class KLBC(KGSC_Base):
	""" Ambient Color """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KLBC', optional)

class KLBI(KMTA_Base):
	""" Intensity """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KLBI', optional)

class HELP(MDXBlock):
	""" Helper """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'HELP', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.objs = []
		self.nhlprs = 0
		objs_size = 0
		while objs_size < self.nbytes:
			obj = OBJ()
			obj.readFrom(file, model)

			self.objs.append(obj)
			self.nhlprs += 1
			objs_size += obj.getSize()

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('nhlprs', '%d', self.nhlprs)
		s1 = html.startLoop()
		for obj_i in range(self.nhlprs):
			loop = html.startLoop()
			loop += html.makeRow(2, self.objs[obj_i].toHTML(parent, root))
			loop += html.endLoop()
			s1 += html.indexProp(obj_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('objs', '%s', s1, block=self, hide=1)
		return s

class ATCH(MDXBlock):
	""" Attachment """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'ATCH', optional)

	def loadBlock(self, file, model):
		PATH_LEN = 0x100
		self.nbytes = readLong(file)
		perror(".nbytes = %d" % (self.nbytes))
		self.attachments = []
		self.natts = 0
		atts_size = 0
		while atts_size < self.nbytes:
			att = Struct()
			att.nbytesi = readLong(file)
			perror(".att.nbytesi = %d" % (att.nbytesi))
			att.OBJ = OBJ()
			att.OBJ.readFrom(file, model)
			att.Path = readString(file, PATH_LEN)
			att.UNKNOWN_long_0 = readLong(file)
			att.AttachmentID = readLong(file)
			att.KATV = KATV(1)
			att.KATV.readFrom(file, model)

			self.attachments.append(att)
			self.natts += 1
			atts_size += att.nbytesi
			perror(" atts_size = %d" % (atts_size))

	def toString(self):
		return ''

	def getBlockSize(self):
		return 4 + self.nbytes

	def blockToHTML(self, parent, root):
		s = html.property('natts', '%d', self.natts)
		s1 = html.startLoop()
		for att_i in range(self.natts):
			att = self.attachments[att_i]
			loop = html.startLoop()
			loop += html.makeRow(2, att.OBJ.toHTML(att, root))
			loop += html.property('Path', '%s', att.Path)
			loop += html.property('UNKNOWN_long_0', '%d', att.UNKNOWN_long_0)
			loop += html.property('AttachmentID', '%d', att.AttachmentID)
			loop += html.makeRow(2, att.KATV.toHTML(att, root))
			loop += html.endLoop()
			s1 += html.indexProp(att_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('attachments', '%s', s1, block=self, hide=1)
		return s

class KATV(KMTA_Base):
	""" Visibility """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KATV', optional)

class PIVT(MDXBlock):
	""" PivotPoints """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PIVT', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.pivpts = []
		self.npvps = 0
		pivs_size = 0
		while pivs_size < self.nbytes:
			pivt = Struct()
			pivt.x = readFloat(file)
			pivt.y = readFloat(file)
			pivt.z = readFloat(file)
			self.pivpts.append(pivt)
			self.npvps += 1
			pivs_size += 3 * FLOAT_SIZE

	def getBlockSize(self):
		return 4 + self.nbytes

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('npvps', '%d', self.npvps)
		s1 = html.startLoop()
		for pivt_i in range(self.npvps):
			pivt = self.pivpts[pivt_i]
			loop = html.startLoop()
			loop += html.property('x, y, z', '%s', '%f, %f, %f' % (pivt.x, pivt.y, pivt.z))
			loop += html.endLoop()
			s1 += html.indexProp(pivt_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('pivpts', '%s', s1, block=self, hide=1)
		return s

class PREM(MDXBlock):
	usesTga = 0x80 # +bit8(EmitterUsesTGA)
	usesMdl = 0x800000 # +bit23(EmitterUsesMDL)

	def usesTga(prem):
		return prem.Flags & PREM.usesTga

	def usesMdl(prem):
		return prem.Flags & PREM.usesMdl

	""" ParticleEmitter """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PREM', optional)

	def loadBlock(self, file, model):
		NAME_LEN = 0x50
		MODELPATH_LEN = 0x100
		self.nbytes = readLong(file)
		self.particleemitters = []
		self.nprems = 0
		prems_size = 0
		while prems_size < self.nbytes:
			prem = Struct()
			prem.nbytesi = readLong(file)
			prem.nbytesikg = readLong(file)
			prem.Name = readString(file, NAME_LEN)
			prem.ObjectID = readLong(file)
			prem.Parent = readLong(file)
			prem.Flags = readLong(file)

			prem.KGTR = KGTR(1)
			prem.KGTR.readFrom(file, model)
			prem.KGRT = KGRT(1)
			prem.KGRT.readFrom(file, model)
			prem.KGSC = KGSC(1)
			prem.KGSC.readFrom(file, model)

			prem.EmissionRate = readFloat(file)
			prem.Gravity = readFloat(file)
			prem.Longitude = readFloat(file)
			prem.Latitude = readFloat(file)
			prem.ModelPath = readString(file, MODELPATH_LEN)
			prem.UNKNOWN_long_0 = readLong(file)
			prem.LifeSpan = readFloat(file)
			prem.InitVelocity = readFloat(file)

			prem.KPEV = KPEV(1)
			prem.KPEV.readFrom(file, model)

			self.particleemitters.append(prem)
			self.nprems += 1
			prems_size += prem.nbytesi + prem.KPEV.getSize()

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('nprems', '%d', self.nprems)
		s1 = html.startLoop()
		for prem_i in range(self.nprems):
			prem = self.particleemitters[prem_i]
			loop = html.startLoop()
			loop += html.property('Name', '%s', prem.Name)
			loop += html.property('ObjectID', '%d', prem.ObjectID)
			loop += html.property('Parent', '%d', prem.Parent)
			loop += html.property('Flags', '%d', prem.FLags)
			loop += html.makeRow(2, '%s', prem.KGTR.toHTML(prem, root))
			loop += html.makeRow(2, '%s', prem.KGRT.toHTML(prem, root))
			loop += html.makeRow(2, '%s', prem.KGSC.toHTML(prem, root))
			loop += html.property('EmissionRate', '%f', prem.EmissionRate)
			loop += html.property('Gravity', '%f', prem.Gravity)
			loop += html.property('Longitude', '%f', prem.Longitude)
			loop += html.property('Latitude', '%f', prem.Latitude)
			loop += html.property('ModelPath', '%s', prem.ModelPath)
			loop += html.property('UNKNOWN_long_0', '%d', prem.UNKNOWN_long_0)
			loop += html.property('LifeSpan', '%f', prem.LifeSpan)
			loop += html.property('InitVelocity', '%f', prem.InitVelocity)
			loop += html.makeRow(2, prem.KPEV.toHTML(prem, root))
			loop += html.endLoop()
			s1 += html.indexProp(prem_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('particleemitters', '%s', s1, block=self, hide=1)
		return s

class KGTR(KGSC_Base):
	""" Translation """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KGTR', optional)

	def __str__(self):
		return KGSC_Base.__str__(self)

class KGRT(MDXBlock):
	""" Rotation """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'KGRT', optional)

	def loadBlock(self, file, model):
		nrItems = readLong(file)
		perror(" nrItems = %d" % (nrItems))
		self.LineType = readLong(file)
		self.GlobalSeqId = readLong(file)
		self.array = []
		for i in range(nrItems):
			rot = Struct()
			rot.Frame = readLong(file)
			rot.a = readFloat(file)
			rot.b = readFloat(file)
			rot.c = readFloat(file)
			rot.d = readFloat(file)
			if self.LineType > 1:
				rot.InTana = readFloat(file)
				rot.InTanb = readFloat(file)
				rot.InTanc = readFloat(file)
				rot.InTand = readFloat(file)
				rot.OutTana = readFloat(file)
				rot.OutTanb = readFloat(file)
				rot.OutTanc = readFloat(file)
				rot.OutTand = readFloat(file)

			self.array.append(rot)

	def toString(self):
		return "Not implemented!\n"

	def blockToHTML(self, parent, root):
		nrItems = len(self.array)
		s = html.property('nrItems', '%d', nrItems)
		s += html.property('LineType', '%d', self.LineType)
		s += html.property('GlobalSeqId', '%d', self.GlobalSeqId)
		s1 = html.startLoop()
		for rot_i in range(nrItems):
			rot = self.array[rot_i]
			loop = html.startLoop()
			loop += html.property('Frame', '%d', rot.Frame)
			loop += html.property('a', '%f', rot.a)
			loop += html.property('b', '%f', rot.b)
			loop += html.property('c', '%f', rot.c)
			loop += html.property('d', '%f', rot.d)
			if self.LineType > 1:
				loop += html.property('InTana', '%f', rot.InTana)
				loop += html.property('InTand', '%f', rot.InTanb)
				loop += html.property('InTanc', '%f', rot.InTanc)
				loop += html.property('InTand', '%f', rot.InTand)
				loop += html.property('OutTana', '%f', rot.OutTana)
				loop += html.property('OutTanb', '%f', rot.OutTanb)
				loop += html.property('OutTanc', '%f', rot.OutTanc)
				loop += html.property('OutTand', '%f', rot.OutTand)
			loop += html.endLoop()
			s1 += html.indexProp(rot_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('array', '%s', s1, block=self, hide=1)
		return s

class KPEV(KMTA_Base):
	""" Visibility """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KPEV', optional)

class PRE2(MDXBlock):
	""" ParticleEmitter 2 """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'PRE2', optional)

	def loadBlock(self, file, model):
		NAME_LEN = 0x50
		self.nbytes = readLong(file)
		self.particleemitters = []
		self.npre2s = 0
		prems_size = 0
		while prems_size < self.nbytes:
			prem = Struct()
			prem.nbytesi = readLong(file)
			prem.nbytesikg = readLong(file)
			prem.Name = readString(file, NAME_LEN)
			prem.ObjectID = readLong(file)
			prem.Parent = readLong(file)
			prem.Flags = readLong(file)

			prem.KGTR = KGTR(1)
			prem.KGTR.readFrom(file, model)
			prem.KGRT = KGRT(1)
			prem.KGRT.readFrom(file, model)
			prem.KGSC = KGSC(1)
			prem.KGSC.readFrom(file, model)

			prem.Speed = readFloat(file)
			prem.Variation = readFloat(file)
			prem.Latitude = readFloat(file)
			prem.Gravity = readFloat(file)
			prem.Lifespan = readFloat(file)
			prem.EmissionRate = readFloat(file)
			prem.Length = readFloat(file)
			prem.Width = readFloat(file)
			prem.FilterMode = readLong(file)
			prem.Rows = readLong(file)
			prem.Columns = readLong(file)
			prem.Flag2 = readLong(file)
			prem.TailLength = readFloat(file)
			prem.Time = readFloat(file)
			prem.color = []
			prem.numsegments = 3
			for i in range(prem.numsegments):
				seg = Struct()
				seg.R = readFloat(file)
				seg.G = readFloat(file)
				seg.B = readFloat(file)
				prem.color.append(seg)
			prem.Alpha1 = readByte(file)
			prem.Alpha2 = readByte(file)
			prem.Alpha3 = readByte(file)
			prem.ScalingX = readFloat(file)
			prem.ScalingY = readFloat(file)
			prem.ScalingZ = readFloat(file)
			prem.LifeSpanUVAnim1 = readLong(file)
			prem.LifeSpanUVAnim2 = readLong(file)
			prem.LifeSpanUVAnim3 = readLong(file)
			prem.DecayUVAnim1 = readLong(file)
			prem.DecayUVAnim2 = readLong(file)
			prem.DecayUVAnim3 = readLong(file)
			prem.TailUVAnim1 = readLong(file)
			prem.TailUVAnim2 = readLong(file)
			prem.TailUVAnim3 = readLong(file)
			prem.TailDecayUVAnim1 = readLong(file)
			prem.TailDecayUVAnim2 = readLong(file)
			prem.TailDecayUVAnim3 = readLong(file)
			prem.TextureID = readLong(file)
			prem.Squirt = readLong(file)
			prem.PriorityPlane = readLong(file)
			prem.ReplaceableID = readLong(file)

			prem.KP2S = KP2S(1)
			prem.KP2S.readFrom(file, model)
			prem.KP2L = KP2L(1)
			prem.KP2L.readFrom(file, model)
			prem.KP2E = KP2E(1)
			prem.KP2E.readFrom(file, model)
			prem.KP2V = KP2V(1)
			prem.KP2V.readFrom(file, model)
			prem.KP2N = KP2N(1)
			prem.KP2N.readFrom(file, model)
			prem.KP2W = KP2W(1)
			prem.KP2W.readFrom(file, model)

			self.particleemitters.append(prem)
			self.npre2s += 1
			prems_size += prem.nbytesi

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('npre2s', '%d', self.npre2s)
		s1 = html.startLoop()
		for prem_i in range(self.npre2s):
			prem = self.particleemitters[prem_i]
			loop = html.property('Name', '%s', prem.Name)
			loop += html.property('ObjectID', '%d', prem.ObjectID)
			loop += html.property('Parent', '%d', prem.Parent)
			loop += html.property('Flags', '%d', prem.Flags)
			loop += html.makeRow(2, prem.KGTR.toHTML(prem, root))
			loop += html.makeRow(2, prem.KGRT.toHTML(prem, root))
			loop += html.makeRow(2, prem.KGSC.toHTML(prem, root))
			loop += html.property('Speed', '%f', prem.Speed)
			loop += html.property('Variation', '%f', prem.Variation)
			loop += html.property('Latitude', '%f', prem.Latitude)
			loop += html.property('Gravity', '%f', prem.Gravity)
			loop += html.property('Lifespan', '%f', prem.Lifespan)
			loop += html.property('EmissionRate', '%f', prem.EmissionRate)
			loop += html.property('Length', '%f', prem.Length)
			loop += html.property('Width', '%f', prem.Width)
			loop += html.property('FilterMode', '%d', prem.FilterMode)
			loop += html.property('Rows', '%d', prem.Rows)
			loop += html.property('Columns', '%d', prem.Columns)
			loop += html.property('Flag2', '%d', prem.Flag2)
			loop += html.property('TailLength', '%f', prem.TailLength)
			loop += html.property('Time', '%f', prem.Time)
			s2 = html.startLoop()
			for seg_i in range(3):
				seg = prem.color[seg_i]
				s2 += html.indexProp(prem_i, '%s', '(R, G, B) = (%f, %f, %f)' % (seg.R, seg.G, seg.B))
			s2 += html.endLoop()
			loop += html.property('color', '%s', s2, block=prem, hide=1)
			loop += html.property('Alpha (1, 2, 3)', '%s', '(%d, %d, %d)' % (prem.Alpha1, prem.Alpha2, prem.Alpha3))
			loop += html.property('Scaling (X, Y, Z)', '%s', '(%f, %f, %f)' % (prem.ScalingX, prem.ScalingY, prem.ScalingZ))
			loop += html.property('LifeSpanUVAnim (1, 2, 3)', '%s', '(%d, %d, %d)' % (prem.LifeSpanUVAnim1, prem.LifeSpanUVAnim2, prem.LifeSpanUVAnim3))
			loop += html.property('DecayUVAnim (1, 2, 3)', '%s', '(%d, %d, %d)' % (prem.DecayUVAnim1, prem.DecayUVAnim2, prem.DecayUVAnim3))
			loop += html.property('TailUVAnim (1, 2, 3)', '%s', '(%d, %d, %d)' % (prem.TailUVAnim1, prem.TailUVAnim2, prem.TailUVAnim3))
			loop += html.property('TailDecayUVAnim (1, 2, 3)', '%s', '(%d, %d, %d)' % (prem.TailDecayUVAnim1, prem.TailDecayUVAnim2, prem.TailDecayUVAnim3))
			loop += html.property('TextureID', '%d', prem.TextureID)
			loop += html.property('Squirt', '%d', prem.Squirt)
			loop += html.property('PriorityPlane', '%d', prem.PriorityPlane)
			loop += html.property('ReplaceableID', '%d', prem.ReplaceableID)
			loop += html.makeRow(2, prem.KP2S.toHTML(prem, root))
			loop += html.makeRow(2, prem.KP2L.toHTML(prem, root))
			loop += html.makeRow(2, prem.KP2E.toHTML(prem, root))
			loop += html.makeRow(2, prem.KP2V.toHTML(prem, root))
			loop += html.makeRow(2, prem.KP2N.toHTML(prem, root))
			loop += html.makeRow(2, prem.KP2W.toHTML(prem, root))
			loop += html.endLoop()
			s1 += html.indexProp(prem_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('particleemitters', '%s', s1, block=self, hide=1)
		return s

class KP2S(KMTA_Base):
	""" Speed """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2S', optional)

class KP2L(KMTA_Base):
	""" Latitude """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2L', optional)

class KP2E(KMTA_Base):
	""" Emission Rate """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2E', optional)

class KP2V(KMTA_Base):
	""" Visibility """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2V', optional)

class KP2N(KMTA_Base):
	""" Length """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2N', optional)

class KP2W(KMTA_Base):
	""" Width """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KP2W', optional)

class RIBB(MDXBlock):
	""" RibbonEmitter """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'RIBB', optional)

	def loadBlock(self, file, model):
		NAME_LEN = 0x50
		self.nbytes = readLong(file)
		self.ribbs = []
		self.nribbs = 0
		ribbs_size = 0
		while ribbs_size < self.nbytes:
			ribb = Struct()
			ribb.nbytesi = readLong(file)
			ribb.nbytesikg = readLong(file)
			ribb.Name = readString(file, NAME_LEN)
			ribb.ObjectID = readLong(file)
			ribb.Parent = readLong(file)
			ribb.Flags = readLong(file)

			ribb.KGTR = KGTR(1)
			ribb.KGTR.readFrom(file, model)
			ribb.KGRT = KGRT(1)
			ribb.KGRT.readFrom(file, model)
			ribb.KGSC = KGSC(1)
			ribb.KGSC.readFrom(file, model)

			ribb.HeightAbove = readFloat(file)
			ribb.HeightBelow = readFloat(file)
			ribb.Alpha = readFloat(file)
			ribb.ColorR = readFloat(file)
			ribb.ColorG = readFloat(file)
			ribb.ColorB = readFloat(file)
			ribb.LifeSpan = readFloat(file)
			ribb.UNKNOWN_long_0 = readLong(file)
			ribb.EmissionRate = readLong(file)
			ribb.Rows = readLong(file)
			ribb.Columns = readLong(file)
			ribb.MaterialID = readLong(file)
			ribb.Gravity = readFloat(file)

			ribb.KRVS = KRVS(1)
			ribb.KRVS.readFrom(file, model)
			ribb.KRHA = KRHA(1)
			ribb.KRHA.readFrom(file, model)
			ribb.KRHB = KRHB(1)
			ribb.KRHB.readFrom(file, model)

			self.ribbs.append(ribb)
			self.nribbs += 1
			ribbs_size += ribb.nbytesi

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('nribbs', '%d', self.nribbs)
		s1 = html.startLoop()
		for ribb_i in range(self.nribbs):
			ribb = self.ribbs[ribb_i]
			loop = html.startLoop()
			loop += html.property('Name', '%s', ribb.Name)
			loop += html.property('ObjectID', '%d', ribb.ObjectID)
			loop += html.property('Parent', '%d', ribb.Parent)
			loop += html.property('Flags', '%d', ribb.Flags)
			loop += html.makeRow(2, ribb.KGTR.toHTML(ribb, root))
			loop += html.makeRow(2, ribb.KGRT.toHTML(ribb, root))
			loop += html.makeRow(2, ribb.KGSC.toHTML(ribb, root))
			loop += html.property('HeightAbove', '%f', ribb.HeightAbove)
			loop += html.property('HeightBelow', '%f', ribb.HeightBelow)
			loop += html.property('Alpha', '%f', ribb.Alpha)
			loop += html.property('Color (R, G, B)', '%s', '(%f, %f, %f)' % (ribb.ColorR, ribb.ColorG, ribb.ColorB))
			loop += html.property('LifeSpan', '%f', ribb.LifeSpan)
			loop += html.property('UNKNOWN_long_0', '%d', ribb.UNKNOWN_long_0)
			loop += html.property('EmissionRate', '%f', ribb.EmissionRate)
			loop += html.property('Rows', '%d', ribb.Rows)
			loop += html.property('Columns', '%d', ribb.Columns)
			loop += html.property('MaterialID', '%d', ribb.MaterialID)
			loop += html.property('Gravity', '%f', ribb.Gravity)
			loop += html.makeRow(2, '%s', ribb.KRVS.toHTML(ribb, root))
			loop += html.makeRow(2, '%s', ribb.KRHA.toHTML(ribb, root))
			loop += html.makeRow(2, '%s', ribb.KRHB.toHTML(ribb, root))
			loop += html.endLoop()
			s1 += html.indexProp(ribb_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('ribbs', '%s', s1, block=self, hide=1)
		return s

class KRVS(KMTA_Base):
	""" Visibility """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KRVS', optional)

class KRHA(KMTA_Base):
	""" Height Above """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KRHA', optional)

class KRHB(KMTA_Base):
	""" Height Below """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KRHB', optional)

class CAMS(MDXBlock):
	""" Cameras """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'CAMS', optional)

	def loadBlock(self, file, model):
		NAME_LEN = 0x50
		self.nbytes = readLong(file)
		perror(".nbytes = %d" % (self.nbytes))
		perror("file offset: %d" % (file.tell()))
		self.cameras = []
		self.ncams = 0
		cams_size = 0
		while cams_size < self.nbytes:
			cam = Struct()
			cam.nbytesi = readLong(file)
			perror(".cam.nbytesi = %d" % (cam.nbytesi))
			cam.Name = readString(file, NAME_LEN)
			cam.PosX = readFloat(file)
			cam.PosY = readFloat(file)
			cam.PosZ = readFloat(file)
			cam.FieldOfView = readFloat(file)
			cam.FarClip = readFloat(file)
			cam.NearClip = readFloat(file)
			cam.Target = Struct()
			cam.Target.x = readFloat(file)
			cam.Target.y = readFloat(file)
			cam.Target.z = readFloat(file)
			cam.Target.KCTR = KCTR(1)
			cam.Target.KCTR.readFrom(file, model)

			cam.KCRL = KCRL(1)
			cam.KCRL.readFrom(file, model)
			cam.KTTR = KTTR(1)
			cam.KTTR.readFrom(file, model)

			self.cameras.append(cam)
			self.ncams += 1
			cams_size += cam.nbytesi
			perror(" cams_size = %d" % (cams_size))

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('ncams', '%d', self.ncams)
		s1 = html.startLoop()
		for cam_i in range(self.ncams):
			cam = self.cameras[cam_i]
			loop = html.startLoop()
			loop += html.property('Name', '%s', cam.Name)
			loop += html.property('Pos (X, Y, Z)', '%s', '(%f, %f, %f)' % (cam.PosX, cam.PosY, cam.PosZ))
			loop += html.property('FieldOfView', '%f', cam.FieldOfView)
			loop += html.property('FarClip', '%f', cam.FarClip)
			loop += html.property('NearClip', '%f', cam.NearClip)
			loop1 = html.startLoop()
			loop1 += html.property('(x, y, z)', '%s', '(%f, %f, %f)' % (cam.Target.x, cam.Target.y, cam.Target.z))
			loop1 += html.makeRow(2, cam.Target.KCTR.toHTML(cam.Target, root))
			loop1 += html.endLoop()
			loop += html.property('Target', '%s', loop1)
			loop += html.makeRow(2, cam.KCRL.toHTML(cam, root))
			loop += html.makeRow(2, cam.KTTR.toHTML(cam, root))
			loop += html.endLoop()
			s1 += html.indexProp(cam_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('cameras', '%s', s1, block=self, hide=1)
		return s

class KCTR(KGSC_Base):
	""" Translation """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KCTR', optional)

class KCRL(KMTA_Base):
	""" Rotation """
	def __init__(self, optional=0):
		KMTA_Base.__init__(self, 'KCRL', optional)

class KTTR(KGSC_Base):
	""" Translation """
	def __init__(self, optional=0):
		KGSC_Base.__init__(self, 'KTTR', optional)

class EVTS(MDXBlock):
	""" EventObject """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'EVTS', optional)

	def loadBlock(self, file, model):
		self.nbytes = readLong(file)
		self.events = []
		self.nevts = 0
		evts_size = 0
		while evts_size < self.nbytes:
			evt = Struct()
			evt.OBJ = OBJ()
			evt.OBJ.readFrom(file, model)
			data = readString(file, 4)
			if data != 'KEVT':
				raise MDXFileFormatError("expected string 'KEVT' (found " + repr(data) + ")", file)
			evt.ntrks = readLong(file)
			data = readString(file, 4)
			if data != "\xFF\xFF\xFF\xFF":
				raise MDXFileFormatError("expected string '\xFF\xFF\xFF\xFF' (found " + repr(data) + ")", file)
			evt.frames = []
			for i in range(evt.ntrks):
				evt.frames.append(readLong(file))

			self.events.append(evt)
			self.nevts += 1
			evts_size += evt.OBJ.getSize() + 8 + LONG_SIZE + evt.ntrks * LONG_SIZE

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('nevts', '%d', self.nevts)
		s1 = html.startLoop()
		for evt_i in range(self.nevts):
			evt = self.events[evt_i]
			loop = html.startLoop()
			loop += html.makeRow(2, evt.OBJ.toHTML(evt, root))
			loop += html.makeRow(2, 'KEVT')
			loop1 = html.startLoop()
			for frame_i in range(evt.ntrks):
				loop1 += html.indexProp(frame_i, '%d', evt.frames[frame_i])
			loop1 += html.endLoop()
			loop += html.property('frames', '%s', loop1, block=evt, hide=1)
			loop += html.endLoop()
			s1 += html.indexProp(evt_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('events', '%s', s1, block=self, hide=1)
		return s

# it seems we don't need this class
#class KEVT(MDXBlock):
#	""" ... """
#	def __init__(self, file):
#		MDXBlock(file, 'KEVT')

class CLID(MDXBlock):
	""" CollisionShape """
	def __init__(self, optional=0):
		MDXBlock.__init__(self, 'CLID', optional)
		self.desc = {}
		self.desc['Shape'] = {0: 'box', 2: 'sphere'}

	def loadBlock(self, file, model):
		perror(" file offset: %d" % (file.tell()))
		self.nbytes = readLong(file)
		perror(".nbytes = %d" % (self.nbytes))
		self.collisionshape = []
		self.nclds = 0
		cols_size = 0
		while cols_size < self.nbytes:
			col = Struct()
			col.OBJ = OBJ()
			col.OBJ.readFrom(file, model)
			perror(".OBJ size = %d" % (col.OBJ.getSize()))
			col.Shape = readLong(file)
			perror(".col.Shape = %d" % (col.Shape))
			col.x = readFloat(file)
			col.y = readFloat(file)
			col.z = readFloat(file)
			perror(" file offset: %d" % (file.tell()))
			if col.Shape == 0:
				col.x2 = readFloat(file)
				col.y2 = readFloat(file)
				col.z2 = readFloat(file)
				cols_size += 3 * FLOAT_SIZE
			else:
				col.BoundsRadius = readFloat(file)
				cols_size += FLOAT_SIZE
			perror(" file offset: %d" % (file.tell()))

			self.collisionshape.append(col)
			self.nclds += 1
			cols_size += col.OBJ.getSize() + LONG_SIZE + 3 * FLOAT_SIZE
			perror(" cols_size = %d" % (cols_size))

	def toString(self):
		return ''

	def blockToHTML(self, parent, root):
		s = html.property('nclds', '%d', self.nclds)
		s1 = html.startLoop()
		for col_i in range(self.nclds):
			col = self.collisionshape[col_i]
			loop = html.startLoop()
			loop += html.makeRow(2, col.OBJ.toHTML(col, root))
			loop += html.property('Shape', '%d', col.Shape, self.desc['Shape'][col.Shape])
			loop += html.property('(x, y, z)', '%s', '(%f, %f, %f)' % (col.x, col.y, col.z))
			if col.Shape == 0:
				loop += html.property('(x2, y2, z2)', '%s', '(%f, %f, %f)' % (col.x2, col.y2, col.z2))
			else:
				loop += html.property('BoundsRadius', '%f', col.BoundsRadius)
			loop += html.endLoop()
			s1 += html.indexProp(col_i, '%s', loop)
		s1 += html.endLoop()
		s += html.property('collisionshape', '%s', s1, block=self, hide=1)
		return s

class OBJ(MDXBlock):
	""" Object """
	def __init__(self, optional=0):
		# I don't think OBJ can be optional
		MDXBlock.__init__(self, 'OBJ', 0)

	def readFrom(self, file, model):
		self.loadBlock(file, model)

	def loadBlock(self, file, model):
		NAME_LEN = 0x50
		perror("OBJ")
		perror("file offset: %d" % (file.tell()))
		self.nbytesi = readLong(file)
		perror(".nbytesi = %d" % (self.nbytesi))
		self.Name = readString(file, NAME_LEN)
		self.ObjectID = readLong(file)
		perror(".ObjectID = %d" % self.ObjectID)
		self.Parent = readLong(file)
		self.Type = readLong(file)
		self.KGTR = KGTR(1)
		self.KGTR.readFrom(file, model)
		self.KGRT = KGRT(1)
		self.KGRT.readFrom(file, model)
		self.KGSC = KGSC(1)
		self.KGSC.readFrom(file, model)
		self.KATV = KATV(1)
		self.KATV.readFrom(file, model)
		self.present = 1

		model.putObject(self.ObjectID, self)
		model.putObject(self.Name, self)

	def __str__(self):
		return self.toString()

	def getSize(self):
		return self.nbytesi

	def toString(self):
		s = "OBJ\n"
		s += ".nbytes = %d\n" % (self.nbytesi)
		s += ".Name = %s\n" % (self.Name)
		s += ".ObjectID = %d\n" % (self.ObjectID)
		s += ".Parent = %d\n" % (self.Parent)
		s += ".Type = %d\n" % (self.Type)
		s += str(KGTR)
		s += str(KGRT)
		s += str(KGSC)
		s += str(KATV)
		return s

	def blockToHTML(self, parent, root):
		s = html.property('Name', '%s', self.Name)
		s += html.property('ObjectID', '%d', self.ObjectID)
		s += html.property('Parent', '%d', self.Parent)
		s += html.property('Type', '%d', self.Type)
		s += html.makeRow(2, self.KGTR.toHTML(self, root))
		s += html.makeRow(2, self.KGRT.toHTML(self, root))
		s += html.makeRow(2, self.KGSC.toHTML(self, root))
		s += html.makeRow(2, self.KATV.toHTML(self, root))
		return s

class Reader:
	def __init__(self):
		pass

	def loadFile(self, filename):
		file = open(filename, 'rb')
		model = self.load(file)
		model.filename = filename
		file.close()
		return model

	def load(self, file):
		model = Model()
		model.MDLX = MDLX()
		model.MDLX.readFrom(file, model)
		model.VERS = VERS()
		model.VERS.readFrom(file, model)
		model.MODL = MODL()
		model.MODL.readFrom(file, model)
		model.SEQS = SEQS()
		model.SEQS.readFrom(file, model)
		model.GLBS = GLBS(1)
		model.GLBS.readFrom(file, model)
		model.MTLS = MTLS()
		model.MTLS.readFrom(file, model)
		model.TEXS = TEXS()
		model.TEXS.readFrom(file, model)
		model.TXAN = TXAN(1)
		model.TXAN.readFrom(file, model)
		model.GEOS = GEOS()
		model.GEOS.readFrom(file, model)
		model.GEOA = GEOA(1)
		model.GEOA.readFrom(file, model)
		model.BONE = BONE()
		model.BONE.readFrom(file, model)
		model.LITE = LITE(1)
		model.LITE.readFrom(file, model)
		model.HELP = HELP(1)
		model.HELP.readFrom(file, model)
		model.ATCH = ATCH(1)
		model.ATCH.readFrom(file, model)
		model.PIVT = PIVT()
		model.PIVT.readFrom(file, model)
		model.PREM = PREM(1)
		model.PREM.readFrom(file, model)
		model.PRE2 = PRE2(1)
		model.PRE2.readFrom(file, model)
		model.RIBB = RIBB(1)
		model.RIBB.readFrom(file, model)
		model.CAMS = CAMS(1)
		model.CAMS.readFrom(file, model)
		model.EVTS = EVTS(1)
		model.EVTS.readFrom(file, model)
		model.CLID = CLID(1)
		model.CLID.readFrom(file, model)

		# post-processing
		model.bones = []
		if model.BONE.present:
			for bone in model.BONE.bones:
				obj = bone.OBJ
				obj.skinnable = 1
				perror("Appending BONE '" + obj.Name + "'\n")
				model.bones.append(obj)
		if model.HELP.present:
			for obj in model.HELP.objs:
				obj.skinnable = 0
				perror("Appending HELP '" + obj.Name + "'\n")
				model.bones.append(obj)

		# store pointers to bones in a dict keyed by frame time
		model.keyFrames = {}
		for obj in model.bones:
			# Translation keyframes
			k = obj.KGTR
			if k.present:
				k.keyFrames = {}
				for frame_i in range(len(k.array)):
					time = k.array[frame_i].Frame
					deepDictListAdd(k.keyFrames, [time], frame_i)
					deepDictListAdd(model.keyFrames, [time], obj, 1)

			# Rotation keyframes
			k = obj.KGRT
			if k.present:
				k.keyFrames = {}
				for frame_i in range(len(k.array)):
					time = k.array[frame_i].Frame
					deepDictListAdd(k.keyFrames, [time], frame_i)
					deepDictListAdd(model.keyFrames, [time], obj, 1)

			# Scaling keyframes
			k = obj.KGSC
			if k.present:
				k.keyFrames = {}
				for frame_i in range(len(k.array)):
					time = k.array[frame_i].Frame
					deepDictListAdd(k.keyFrames, [time], frame_i)
					deepDictListAdd(model.keyFrames, [time], obj, 1)
		return model

class Writer:
	def __init__(self):
		pass
