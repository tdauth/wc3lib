import sys
import os

import Blender
#from Blender.Armature import SKINNABLE, UNSKINNABLE
from Blender.Mathutils import Quaternion
#from Blender.Armature.Bone import LOC, ROT, SIZE

from mdx import mdx
from converter import Converter


class Exporter(Converter):
	def __init__(self, scene=None, fps=None):
		Converter.__init__(self, scene, fps)
