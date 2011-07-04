# -*- coding: utf-8 -*-
import sys
import os
import math

import Blender
#from Blender.Armature.Bone import SKINNABLE, UNSKINNABLE
from Blender.Mathutils import Quaternion
#from Blender.Armature.Bone import LOC, ROT, SIZE

from mdx import mdx
from converter import Converter


def add(x, y):
	return x+y

def sub(x, y):
	return x-y

def checkForOptionalBlock(block):
	if block.optional and not block.present:
		return True

	return False

def loadObjectTranslations(wbObject, bObject):
	# load translations
	if not checkForOptionalBlock(wbObject.KGTR):
		# TODO check line type and additional data
		for elem in wbObject.KGTR.array:
			bObject.loc = [bObject.loc.x + elem.x, bObject.loc.y + elem.y, bObject.loc.z +  elem.z] # add values (translate)

def loadObjectRotations(wbObject, bObject):
	# load rotations
	if not checkForOptionalBlock(wbObject.KGRT):
		# TODO check line type and additional data
		for elem in wbObject.KGRT.array:
			# TODO: Values a, b, c, d in MDLX?
			bObject.rot = [bObject.rot.x + elem.a, bObject.rot.y + elem.b, bObject.rot.z +  elem.c] # add values (rotate)

def loadObjectScalings(wbObject, bObject):
	# load scalings
	if not checkForOptionalBlock(wbObject.KGSC):
		# TODO check line type and additional data
		for elem in wbObject.KGSC.array:
			bObject.setSize([bObject.getSize().x + elem.x, bObject.getSize().y + elem.y, bObject.getSize().z +  elem.z]) # add values (scale)

def loadObjectVisibilities(wbObject, bObject):
	# load visibilities
	if not checkForOptionalBlock(wbObject.KATV):
		for alpha in wbObject.KATV.alpha:
			print "Unable to add alpha."
			# TODO: Values frame and state in MDLX?
			#bObject.rot = [bObject.rot.x + elem.a, bObject.rot.y + elem.b, bObject.rot.z +  elem.c] # add values (rotate)

# Call this function for every object which contains an OBJ instance.
def loadObject(wbObject, bObject):
	bObject.setName(wbObject.Name)

	# TODO properties for sequences only?
	loadObjectTranslations(wbObject, bObject)
	loadObjectRotations(wbObject, bObject)
	loadObjectScalings(wbObject, bObject)
	loadObjectVisibilities(wbObject, bObject)

# Each load MDX file has its own group.
# Sometimes files are being loaded which should be changed (like models of particle emitters).
# Those file groups should get some constraints.
class Importer(Converter):
	def __init__(self, scene=None, fps=None):
		Converter.__init__(self, scene, fps)
		self.model = None
		self.group = None
		self.textures = None
		self.materials = None
		self.meshes = None
		self.armature = None

	def getObjectByName(self, name):
		for obj in self.scene.objects:
			if obj.name() == name:
				return obj

		return None
	
	def loadObjectPivotPointLocation(wbObjectId, blObject):
		wbPivt = self.model.PIVT.pivpts[wbObjectId]
		blObject.loc = [ wbPivt.x, wbPivt.y, wbPivt.z ]

	# Call this function to load parent data and create parent relationships between objects.
	def loadObjectParent(self, wbObject, bObject):
		# FIXME: 0 is default parent, what's meant by default?
		if wbObject.Parent == 0 or wbObject.Parent == 0xFFFFFFFF:
			return

		wbParent = wbObject.objects[wbObject.Parent]
		bParent = self.getObjectByName(wbParent.model.MODL.Name)

		if bParent == None:
			sys.stderr.write("Parent object \"%s\" of object \"%s\" does not exist in scene \"%s\".\n" % wbParent.model.MODL.Name % bObject.name() % self.scene.getName())
			return

		sys.stderr.write("Object \"%s\" has parent \"%s\".\n" % bObject.getName() % bParent.getName())
		bObject.setParent(bParent)

		# TODO properties for sequences only?
		#if not wbObject.dontInheritTranslation():
		#	loadObjectTranslations(wbParent, bObject)

		#if not wbObject.dontInheritScaling():
		#	loadObjectScalings(wbParent, bObject)

		#if not wbObject.dontInheritRotation():
		#	loadObjectRotations(wbParent, bObject)


		# TODO Inherit parent's visibilities (no flag?)
		#loadObjectVisibilities(wbParent, bObject)

		# TODO What does billboarded mean?
		# if wbObject.billboarded():
		# if wbObject.billboardedLockX():
		# if wbObject.billboardedLockY():

		#if wbObject.cameraAnchored():
			# TODO inherit camera

	def loadAllObjectParents(self):
		for obj in self.scene.objects:
			obj

	def linkObject(self, datablock, name):
		print "Name: %sObject" % name
		obj = self.scene.objects.new(datablock) # name + 'Object'
		obj.setName("%sObject" % name)
		obj.link(datablock)
		#self.scene.link(object)
		self.group.objects.link(obj)

		return obj

	def armatureName(self):
		return self.model.MODL.Name + '.Armature'

	def textureName(self, texnr):
		return self.model.MODL.Name + '.Tex' + str(texnr)

	def materialName(self, matnr):
		return self.model.MODL.Name + '.Mat' + str(matnr)

	def meshName(self, geosetnr):
		return self.model.MODL.Name + '.Mesh' + str(geosetnr)

	def lightName(self, lightnr):
		return self.model.MODL.Name + '.Lamp' + str(lightnr)

	def particleEmitterName(self, premnr):
		return self.model.MODL.Name + '.ParticleEmitter' + str(premnr)

	def toBlender(self, model):
		self.model = model
		self.meshes = []
		print "Model name " + self.model.MODL.Name
		self.group = Blender.Group.New(self.model.MODL.Name)
		self.armature = Blender.Armature.New(self.armatureName()) # is linked in loadSequences

		sys.stderr.write("Converting model \"%s\" to Blender data structures..\n" % self.model.MODL.Name)
		self.loadTextures()
		self.loadMaterials()
		self.loadMeshes()
		self.loadBones()
		self.loadSequences()
		self.loadLights()
		self.loadParticleEmitters()
		self.loadParticleEmitters2()
		sys.stderr.write("Done converting model \"%s\".\n" % self.model.MODL.Name)

		self.armature = None
		self.meshes = None
		self.scene = None
		self.model = None

	def loadTextures(self):
		(filepath, dummy) = os.path.split(self.model.filename)
		self.textures = []
		for texnr in range(self.model.TEXS.ntexs):
			bTexture = Blender.Texture.New(self.textureName(texnr))
			#self.linkObject(bTexture, self.textureName(texnr))
			bTexture.setType('Image')
			bTexture.setImageFlags('InterPol', 'UseAlpha', 'MipMap')
			texturefile = self.model.TEXS.textures[texnr].TexturePath;
			if texturefile!='':
				if os.name!='nt':
					texturefile = texturefile.replace("\\", "/")
				(texturefile, dummy) = os.path.splitext(texturefile)
				texturefile = os.path.join(filepath, texturefile + ".png")
				sys.stderr.write("texture %d: PNG texture file (%s)\n" % (texnr, texturefile))

				try:
					bTexture.setImage(Blender.Image.Load(texturefile))
				except IOError:
					sys.stderr.write("Unable to assign texture file.\n")

					pass

				self.textures.append(bTexture)
			else:
				sys.stderr.write("texture %d: None\n" % texnr)
				self.textures.append(None)

	def loadMaterials(self):
		self.materials = []
		for matnr in range(self.model.MTLS.nmtls):
			mat = Blender.Material.New(self.materialName(matnr))
			#self.linkObject(mat, self.materialName(matnr))
			mat.setRGBCol(self.teamcolour)
			mat.setMode('Shadeless', 'ZTransp')
			setTexFace = 1
			for laynr in range(self.model.MTLS.materials[matnr].LAYS.nlays):
				mapto = 0
				lay = self.model.MTLS.materials[matnr].LAYS.layers[laynr]
				if lay.FilterMode == 1: # the usual kind of texture
					mapto = mapto | Blender.Texture.MapTo.ALPHA | Blender.Texture.MapTo.COL
				elif lay.FilterMode == 2: # the kind of tecture that shows team colour through the alpha
					mapto = mapto | Blender.Texture.MapTo.COL
					setTexFace = 0
				elif lay.FilterMode == 3:
					# set 'texture blending mode' to 'add'
					pass
				elif lay.FilterMode == 4:
					# set 'texture blending mode' to 'add'
					mapto = mapto | Blender.Texture.MapTo.ALPHA
				elif lay.FilterMode == 5:
					# modulate ??
					pass
				tex = self.textures[self.model.MTLS.materials[matnr].LAYS.layers[laynr].TextureID]
				if tex!=None:
					mat.setTexture(laynr, tex, Blender.Texture.TexCo.UV, mapto)
					# FIXME we also need a way to specify the other kind of ALPHA setting
					# FIXME as well as the DVar option in the MapTo tab of Shading->Material Buttons
			if setTexFace==1:
				mat.setMode('TexFace')
			self.materials.append(mat)

	def loadMeshes(self):
		for geosetnr in range(len(self.model.GEOS.geosets)):
			geoset = self.model.GEOS.geosets[geosetnr]
			mymesh = Blender.Mesh.New(self.meshName(geosetnr))
			mymeshObject = self.linkObject(mymesh, self.meshName(geosetnr))
			sys.stderr.write("mesh name \"%s\"\n" % self.model.MODL.Name + '.Mesh' + str(geosetnr))
			sys.stderr.write("mesh %d: %d NRMS\n" % (geosetnr, geoset.NRMS.nvrts))

			#mymesh.hasFaceUV(1);

			normalVertices = []

			# vertices
			for vertnr in range(len(geoset.VRTX.vertices)):
				vert = geoset.VRTX.vertices[vertnr]
				x = vert.x
				y = vert.y
				z = vert.z
				mymesh.verts.extend(Blender.Mathutils.Vector([x,y,z]))
				mymesh.verts[vertnr].no.x = geoset.NRMS.vertices[vertnr].x
				mymesh.verts[vertnr].no.y = geoset.NRMS.vertices[vertnr].y
				mymesh.verts[vertnr].no.z = geoset.NRMS.vertices[vertnr].z

				#if HAS_TEX==1:
				#	v.uvco[0] = geoset.UVAS.UVBS[0].vertices[vert_i].x;
				#	v.uvco[1] = geoset.UVAS.UVBS[0].vertices[vert_i].y;

			sys.stderr.write("mesh %d: %d verts\n" % (geosetnr, len(mymesh.verts)))
			sys.stderr.write("Mesh added to Blender.\n")

			# faces
			for i in range(geoset.PVTX.nvrts / 3):
				v1 = geoset.PVTX.vertices[3 * i]
				v2 = geoset.PVTX.vertices[3 * i + 1]
				v3 = geoset.PVTX.vertices[3 * i + 2]
				mymesh.faces.extend([mymesh.verts[v1], mymesh.verts[v2], mymesh.verts[v3]])

				uv_v1 = Blender.Mathutils.Vector(geoset.UVAS.UVBS[0].vertices[v1].x, 1-geoset.UVAS.UVBS[0].vertices[v1].y);
				uv_v2 = Blender.Mathutils.Vector(geoset.UVAS.UVBS[0].vertices[v2].x, 1-geoset.UVAS.UVBS[0].vertices[v2].y);
				uv_v3 = Blender.Mathutils.Vector(geoset.UVAS.UVBS[0].vertices[v3].x, 1-geoset.UVAS.UVBS[0].vertices[v3].y);
				mymesh.faces[i].uv = [uv_v1, uv_v2, uv_v3];
				mymesh.faces[i].mode = mymesh.faces[i].mode | Blender.Mesh.FaceModes['TEX'];
				mymesh.faces[i].mat = 0;

			sys.stderr.write("mesh %d: %d faces\n" % (geosetnr, len(mymesh.faces)))

			# vertex groups
			#self.groups = {}
			#for vertexIndex in range(len(geoset.GNDX.vertexGroups)):
			#	sys.stderr.write("Vertex group count %i of group %i. Matrices count %i" % geoset.GNDX.vertexGroups[vertexIndex] % vertexIndex % len(geoset.MATS.matrices))
			#	objectList = geoset.MATS.matrices[geoset.MTGC.groupCount[geoset.GNDX.vertexGroups[vertexIndex]]]
			#	sys.stderr.write("objectList: %s\n" % type(objectList))
			#	boneNames = [self.BONE.bones[objectIndex].OBJ.Name for objectIndex in objectList]
			#
			#	for name in boneNames:
			#		if self.groups.has_key(name):
			#			self.groups[name].append(vertexIndex)
			#		else:
			#			self.groups[name] = [vertexIndex]
			#
			#for grp_name, grp in self.groups:
			#	mymesh.addVertGroup(grp_name)
			#	mymesh.assignVertsToGroup(grp_name, grp, 1.0, 'replace')

			mymesh.update()
			mymeshObject.setMaterials([self.materials[geoset.MaterialID]]);
			self.meshes.append(mymeshObject)

	def loadBones(self):
		sys.stderr.write("Ordering bones..\n")
		boneOrder = []
		for bone_id in range(len(self.model.bones)):
			if bone_id not in boneOrder:
				self.boneInsert(bone_id, boneOrder)
		sys.stderr.write("Bone order: %s\n" % str(boneOrder))

		wbBoneVertices = [[] for x in self.model.bones]
		wbBoneChildren = [[] for x in self.model.bones]
		blMeshObjArray = []
		# populate wbBoneVertices for bone tail calculations,
		#   and blMeshObjArray for armature parenting
		for geoset_i in range(len(self.model.GEOS.geosets)):
			blMeshObj = self.meshes[geoset_i]
			blMeshObjArray.append(blMeshObj)
			blMesh = blMeshObj.getData()
			wbGeoset = self.model.GEOS.geosets[geoset_i]
			wbGroups = [[] for x in range(len(self.model.bones))]
			# construct the group arrays
			for vert_id in range(wbGeoset.GNDX.nvgrps):
				groupid = wbGeoset.GNDX.vertexGroups[vert_id]
				group = wbGeoset.MATS.getGroup(wbGeoset, groupid)
				for boneingroup in group:
					wbGroups[boneingroup].append(vert_id)
			# assign vertices to Blender's vertex groups
			for bone_id in range(len(self.model.bones)):
				if len(wbGroups[bone_id]) > 0:
					blMesh.addVertGroup(self.model.bones[bone_id].Name)
					blMesh.assignVertsToGroup(self.model.bones[bone_id].Name, wbGroups[bone_id], 1.0, 'replace')
					wbBoneVertices[bone_id].extend(
						[[wbGeoset.VRTX.vertices[i].x,
						wbGeoset.VRTX.vertices[i].y,
						wbGeoset.VRTX.vertices[i].z] for i in wbGroups[bone_id]])
			blMesh.update()

		#bones = {}
		wbBoneHeads = [[] for x in self.model.bones]
		print "We have %i MDX bones" % len(self.model.BONE.bones)
		# create the bones and set the bone head
		for bone_id in boneOrder:
			wbBone = self.model.BONE.bones[bone_id]
			wbPivt = self.model.PIVT.pivpts[wbBone.OBJ.ObjectID]
			wbBoneKey = wbBone.OBJ.Name

			if wbBone.OBJ.Parent != 0xFFFFFFFF:
				wbBoneChildren[wbBone.OBJ.Parent].append(bone_id)
			#else:
				#parentTail = [0, 0, 0]

			myEditBone = Blender.Armature.Editbone()
			self.armature.makeEditable()

			# TODO: create bone object
			#loadObject(wbBone, myEditBone)
			#self.loadObjectParent(wbBone, myEditBone) # load parent data

			self.armature.bones[wbBoneKey] = myEditBone
			self.armature.bones[wbBoneKey].head = Blender.Mathutils.Vector([wbPivt.x, wbPivt.y, wbPivt.z])
			#self.armature.bones[wbBoneKey].hasIK(1)

			#if wbBone.skinnable:
			#	self.armature.bones[wbBoneKey].setBoneclass((SKINNABLE))
			#else:
			#	self.armature.bones[wbBoneKey].setBoneclass((UNSKINNABLE))

			sys.stderr.write("Adding Blender bone '" + wbBoneKey + "\n")

		# set the bone tails
		for bone_id in boneOrder:
			wbBone = self.model.BONE.bones[bone_id]
			wbBoneKey = wbBone.OBJ.Name

			if len(wbBoneChildren[bone_id]) > 0:
				# average the heads of all children
				avg = [0.0, 0.0, 0.0]

				for childIndex in wbBoneChildren[bone_id]:
					childKey = self.model.BONE.bones[childIndex].OBJ.Name
					avg = map(add, avg, self.armature.bones[childKey].head)

				avg = [x/len(wbBoneChildren[bone_id]) for x in avg]
				self.armature.bones[wbBoneKey].tail = Blender.Mathutils.Vector([avg[0], avg[1], avg[2]])
			elif len(wbBoneVertices[bone_id]) > 0:
				# average all the points belonging to this bone and
				# tail = mirror of head through this middle point
				avg = [0.0, 0.0, 0.0]

				for vert in wbBoneVertices[bone_id]:
					avg = map(add, avg, vert)

				avg = [x/len(wbBoneVertices[bone_id]) for x in avg]
				self.armature.bones[wbBoneKey].tail = Blender.Mathutils.Vector([avg[0], avg[1], avg[2]])


		# create scene object
		blArmObj = self.linkObject(self.armature, self.armatureName())
		blArmObj.makeParent(blMeshObjArray)

	def boneInsert(self, boneIndex, boneOrder):
		#sys.stderr.write("boneIndex: %s\n" % str(boneIndex))
		parentID = self.model.BONE.bones[boneIndex].OBJ.Parent
		#sys.stderr.write("parentID: %s\n" % str(parentID))
		# get index
		
		if parentID != -1:
			parentIndex = -1
			
			for i in range(len(self.model.BONE.bones)):
				if parentID == self.model.BONE.bones[i].OBJ.ObjectID:
					parentIndex = i
					break;
					
			if parentIndex != -1 and parentIndex not in boneOrder:
				self.boneInsert(parentIndex, boneOrder)
		
		boneOrder.append(boneIndex)

	def loadSequences(self):
		# <DEBUG>
		#file = open(Blender.Get('datadir') + "/warblender.modelKeyFrames.txt", 'w')
		#keys = self.model.keyFrames.keys()
		#keys.sort()

		#for item in keys:
		#	file.write(str(item) + ": " + str([obj.Name for obj in self.model.keyFrames[item]]) + "\n")

		#file.close()
		# </DEBUG>
		for wbSeq in self.model.SEQS.seqs:
			blAct = Blender.Armature.NLA.NewAction(wbSeq.Name)
			sys.stderr.write("New action from sequence '" + wbSeq.Name + "'\n")
			startTime = wbSeq.IntStart # start time of action in MDX time scale
			endTime = wbSeq.IntEnd

			if (self.model.keyFrames.has_key(startTime)):
				wbActBones = self.model.keyFrames[startTime]
				#for name in [bone.Name for bone in self.model.keyFrames[end]]:
				#	sys.stderr.write("Bone in list: " + name + "\n")
				for wbBone in wbActBones:
					sys.stderr.write("bone name: " + wbBone.Name + "\n")
					self.makeBoneIpo(blAct, wbBone, startTime, endTime, wbSeq.NoLooping)
			else:
				sys.stderr.write("Warning: Missing key frame bones of sequence " + wbSeq.Name + ".")

	def wbTime2blFrame(self, animTime, startTime):
		""" translates animTime from MDX time scale to Blender time scale, wrt to origin startTime """
		timeScale = 0.001
		return float(animTime - startTime) * self.fps * timeScale + 1

	def makeBoneIpo(self, blAct, wbBone, startTime, endTime, NoLooping):
		if NoLooping:
			Xpo = 'Constant'
		else:
			Xpo = 'Cyclic'

		#index = len(self.armature.bones)  # add new?
		blBone = self.armature.bones[wbBone.Name]

		translation = wbBone.KGTR.present and wbBone.KGTR.keyFrames.has_key(startTime) and wbBone.KGTR.keyFrames.has_key(endTime)
		rotation = wbBone.KGRT.present and wbBone.KGRT.keyFrames.has_key(startTime) and wbBone.KGRT.keyFrames.has_key(endTime)
		scaling = wbBone.KGSC.present and wbBone.KGSC.keyFrames.has_key(startTime) and wbBone.KGSC.keyFrames.has_key(endTime)

		for name in blAct.getChannelNames():
			print "Channel %s" % name

		#self.armature.bones[wbBone.Name].setPose([], blAct)
		ipo = blAct.getChannelIpo(wbBone.Name)

		# FIXME we probably still have to set the ones that are not specified to some
		#	default value! Perhaps 0 rotation, 0 translation, 1 scale? Add elses to the ifs to do this

		k = wbBone.KGTR
		if translation:
			xcurve = ipo.addCurve('LocX')
			xcurve.setInterpolation('Linear')
			xcurve.setExtrapolation(Xpo)

			ycurve = ipo.addCurve('LocY')
			ycurve.setInterpolation('Linear')
			ycurve.setExtrapolation(Xpo)

			zcurve = ipo.addCurve('LocZ')
			zcurve.setInterpolation('Linear')
			zcurve.setExtrapolation(Xpo)

			startFrame = k.keyFrames[startTime] # index of the starting frame in the wb bone's KG(TR|RT|SC) data
			endFrame = k.keyFrames[endTime]
			for framei in range(startFrame, endFrame + 1):
				frame = k.array[framei] # the struct containing the time (.Frame) and the data (.x, .y, .z, etc)
				blFramei = self.wbTime2blFrame(frame.Frame, startTime)
				sys.stderr.write("\tFrame "+str(blFramei)+": TR("+str(frame.x)+","+str(-1*frame.z)+","+str(frame.y)+")\n")
				xcurve.addBezier((blFramei, frame.x))
				ycurve.addBezier((blFramei, -1*frame.z))
				zcurve.addBezier((blFramei, frame.y))

		k = wbBone.KGRT
		if rotation:
			#self.makeRotationCurve_quat(ipo, k, startTime, endTime, Xpo)
			#def makeRotationCurve_quat(self, ipo, k, startTime, endTime, Xpo):

			xcurve = ipo.addCurve('QuatX')
			xcurve.setInterpolation('Linear')
			xcurve.setExtrapolation(Xpo)

			ycurve = ipo.addCurve('QuatY')
			ycurve.setInterpolation('Linear')
			ycurve.setExtrapolation(Xpo)

			zcurve = ipo.addCurve('QuatZ')
			zcurve.setInterpolation('Linear')
			zcurve.setExtrapolation(Xpo)

			wcurve = ipo.addCurve('QuatW')
			wcurve.setInterpolation('Linear')
			wcurve.setExtrapolation(Xpo)

			startFrame = k.keyFrames[startTime] # index of the starting frame in the wb bone's KG(TR|RT|SC) data
			endFrame = k.keyFrames[endTime]
			for framei in range(startFrame, endFrame + 1):
				frame = k.array[framei] # the struct containing the time (.Frame) and the data (.x, .y, .z, etc)
				blFramei = self.wbTime2blFrame(frame.Frame, startTime)
				#s_t2 = math.sin(math.acos(frame.d)) # sin_theta2: sine of half the rotation angle
				#if s_t2==0:
				#	axis = [0, 0, 0]
				#else:
				#	axis = [frame.a/s_t2, -1*frame.c/s_t2, frame.b/s_t2]
				if wbBone.Parent == -1:
					axis = [frame.a, frame.b, frame.c]
				else:
					axis = [frame.a, frame.c, frame.b]

				# Past this point, axis contains the right-handed axis, compensated for the Blender bug
				# FIXME When the Blender bug is fixed, axis = [a, b, -c]

				# FIXME Do stuff here to translate the quaternion from left-handedness to right-handedness
				quat_vals = [frame.d]
				quat_vals[len(quat_vals):] = axis
				q = Blender.Mathutils.Quaternion(quat_vals)
				q.normalize()
				xcurve.addBezier((blFramei, q.x))
				ycurve.addBezier((blFramei, q.y))
				zcurve.addBezier((blFramei, q.z))
				wcurve.addBezier((blFramei, q.w))

		k = wbBone.KGSC
		if scaling:
			xc = ipo.getCurve('SizeX')
			print [i.getPoints() for i in xc.getPoints()]


			xcurve = ipo.addCurve('SizeX')
			xcurve.setInterpolation('Linear')
			xcurve.setExtrapolation(Xpo)

			ycurve = ipo.addCurve('SizeY')
			ycurve.setInterpolation('Linear')
			ycurve.setExtrapolation(Xpo)

			zcurve = ipo.addCurve('SizeZ')
			zcurve.setInterpolation('Linear')
			zcurve.setExtrapolation(Xpo)

			startFrame = k.keyFrames[startTime] # index of the starting frame in the wb bone's KG(TR|RT|SC) data
			endFrame = k.keyFrames[endTime]
			for framei in range(startFrame, endFrame + 1):
				frame = k.array[framei] # the struct containing the time (.Frame) and the data (.x, .y, .z, etc)
				blFramei = self.wbTime2blFrame(frame.Frame, startTime)
				sys.stderr.write("\tFrame "+str(blFramei)+": SC("+str(frame.x)+","+str(-1*frame.z)+","+str(frame.y)+")\n")
				xcurve.addBezier((blFramei, frame.x))
				ycurve.addBezier((blFramei, -1*frame.z))
				zcurve.addBezier((blFramei, frame.y))
		return ipo

	def makeRotationCurve_euler(self, ipo, k, start, end, Xpo):
		xcurve = ipo.addCurve('RotX')
		xcurve.setInterpolation('Linear')
		xcurve.setExtrapolation(Xpo)

		ycurve = ipo.addCurve('RotY')
		ycurve.setInterpolation('Linear')
		ycurve.setExtrapolation(Xpo)

		zcurve = ipo.addCurve('RotZ')
		zcurve.setInterpolation('Linear')
		zcurve.setExtrapolation(Xpo)

		fStart = k.keyFrames[start]
		fEnd = k.keyFrames[end]
		for framei in range(fStart, fEnd + 1):
			frame = k.rotation[framei]
			quat = Quaternion([frame.a, frame.c, frame.b, frame.d])
			mat = quat.toEuler()
			xcurve.addBezier((self.wbTime2blFrame(frame.Frame, start), mat.x))
			ycurve.addBezier((self.wbTime2blFrame(frame.Frame, start), -1*mat.z))
			zcurve.addBezier((self.wbTime2blFrame(frame.Frame, start), mat.y))

	# new, optional
	# @author Tamino Dauth
	def loadLights(self):

		if checkForOptionalBlock(self.model.LITE):
			return

		sys.stderr.write("loading %i lights\n" % self.model.LITE.nlits)

		for lightnr in range(self.model.LITE.nlits):
			wbLamp = self.model.LITE.lights[lightnr]
			blLamp = Blender.Lamp.New(self.lightName(lightnr))
			object = self.linkObject(blLamp, self.lightName(lightnr))

			loadObject(wbLamp, blLamp)
			self.loadObjectParent(wbLamp, blLamp) # load parent data

			#(0:Omnidirectional;1:Directional;2:Ambient)
			if wbLamp.Type == 0:
				blLamp.setType('Lamp')
			elif wbLamp.Type == 1:
				blLamp.setType('Spot')
			elif wbLamp.Type == 2:
				blLamp.setType('Area')
			#else:
				# todo throw exception

			#blLamp.setDistance(wbLamp.AttEnd)
			# start und Ende der Schwächung
			#wbLamp.AttStart
			#wbLamp.AttEnd
			blLamp.R = wbLamp.ColR
			blLamp.G = wbLamp.ColB
			blLamp.B = wbLamp.ColB
			blLamp.setEnergy(wbLamp.Intensity)
			#wbLamp.AmbColR
			#wbLamp.AmbColG
			#wbLamp.AmbColB
			#wbLamp.AmbColIntensity



			self.loadObjectPivotPointLocation(wbLamp.OBJ.ObjectId, object)

	# not optional
	def loadParticleEmitters(self):

		if checkForOptionalBlock(self.model.PREM):
			return

		sys.stderr.write("loading %i particle emitters\n" % self.model.PREM.nprems)

		for premnr in range(self.model.PREM.nprems):
			wbParticle = self.model.PREM.particleemitters[premnr]
			blParticle = Blender.Particle.New(self.particleName(premnr))
			#object = self.linkObject(blParticle, self.particleName(premnr))

			# TODO: Does not contain an OBJ instance has its own values!
			blParticle.setName(wbParticle.Name)

			blParticle.maxvel = wbParticle.InitVelocity
			blParticle.avvel = 100 # 100% of max velocity
			blParticle.lifetime = wbParticle.LifeSpan
			blParticle.startFrame = 0
			blParticle.endFrame = prem.EmissionRate # TODO: correct value?
			blParticle.amount = prem.Longitude * prem.Latitude # TODO: Float values, get amount by size (width * height).
			blParticle.tanacc = prem.Gravity * 100 / wbParticle.InitVelocity # TODO: percentage (not latacc?)
			blParticle.type = 'EMITTER'
			blParticle.distribution = Particle.DISTRIBUTION['GRID']
			blParticle.particleDistribution = Particle.EMITFROM['VERTS']
			blParticle.drawAs = Particle.DRAWAS['GROUP']
			blParticle.displayPercentage = 100

			#for rot in blParticle.
			#prem.KGRT = KGRT(1)
			#prem.KGRT.readFrom(file, model)
			#prem.KGSC = KGSC(1)

			if PREM.usesMdl(prem):
				reader = mdx.Reader()
				mdxModel = None

				try:
					mdxModel = reader.loadFile(prem.ModelPath)
				except mdx.MDXFileFormatError:
					sys.stderr.write("Could not read model file \"%1%\" of particle emitter \"%2%\".\n" % prem.ModelPath % wbParticle.Name)


				importer=Importer(scene=self.scene, fps=self.fps)
				importer.toBlender(mdxModel) # TODO: Link load file with its file and do not add to the MDLX file (use model.filename)
				# adding particle system to load object
				group=Group.Get(mdxModel.MODL.Name)

				for object in group.objects:
					object.newParticleSystem(blParticle.getName())

			elif PREM.usesTga(prem): # TODO: Load model file as TGA texture (plane)?!
				print "Uses tga"


			# do not inherit translation, rotation and scaling
			#if wbParticle.Parent == -1:
			# inherit translation, rotation and scaling
			#else:
				#blParticle.setParent(

			self.loadObjectPivotPointLocation(wbParticle.ObjectID, object)

	# not optional
	def loadParticleEmitters2(self):

		if checkForOptionalBlock(self.model.PRE2):
			return

		sys.stderr.write("loading %i particle emitters 2\n" % self.model.PRE2.npre2s)
