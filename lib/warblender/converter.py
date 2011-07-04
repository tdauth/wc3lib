import Blender
from mdx import mdx

TEAMCOLOUR = [0, 0, 1]

class Converter:
	def __init__(self, scene=None, fps=None):
		if scene == None:
			self.scene = Blender.Scene.GetCurrent()
		else:
			self.scene = scene
		if fps == None:
			self.fps = self.scene.getRenderingContext().framesPerSec()
		else:
			self.fps = fps
		self.teamcolour = TEAMCOLOUR
