import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class XMLAnimationLoader(fife.AnimationLoader):
	def __init__(self, imagepool, vfs):
		fife.AnimationLoader.__init__(self)
		self.imagepool = imagepool
		self.vfs = vfs
		self.thisown = 0
	
	def loadResource(self, location):
		f = self.vfs.open(location.getFilename())
		tree = ET.parse(f)
		root = tree.getroot()
		
		animation = fife.Animation()
		common_frame_delay = int(root.get('delay', 0))
		x_offset = int(root.get('x_offset', 0))
		y_offset = int(root.get('y_offset', 0))
		animation.setActionFrame(int(root.get('action', 0)))
		
		frames = root.findall('frame')
		if not frames:
			raise fife.InvalidFormat('animation without <frame>s')
		
		for frame in frames:
			source = frame.get('source')
			if not source:
				raise fife.InvalidFormat('animation without <frame>s')

			frame_x_offset = int(frame.get('x_offset', x_offset))
			frame_y_offset = int(frame.get('y_offset', y_offset))
			frame_delay = int(frame.get('delay', common_frame_delay))
			
			image = self.imagepool.getImage(self.imagepool.addResourceFromFile(source))
			image.setXShift(frame_x_offset)
			image.setYShift(frame_y_offset)
			animation.addFrame(image, frame_delay);

		animation.thisown = 0
		return animation
