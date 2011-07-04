import pyogre.ogre as ogre
import SampleFramework as sf

# @todo Maybe we should implement the renderer in C++
class Widget(QWidget):

	def __init__(self, parent = None):
		QWidget.__init__(self, parent)
		sceneManager = SceneManager("Scene Manager")
		camera = sceneManager.createCamera("Camera")