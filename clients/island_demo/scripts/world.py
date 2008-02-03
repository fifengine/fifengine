import fife, math
from eventlistenerbase import EventListenerBase
from loaders import loadMapFile
from savers import saveMapFile
from hero import Hero
from girl import Girl
import settings as TDS

class World(EventListenerBase):
	def __init__(self, engine):
		super(World, self).__init__(engine, regMouse=True, regKeys=True)
		self.engine = engine
		self.eventmanager = engine.getEventManager()
		self.model = engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.view = self.engine.getView()
		self.filename = ''
		
		
	def reset(self):
		self.map, self.elevation, self.agentlayer = None, None, None
		self.cameras = {}
		self.hero, self.girl = None, None
		self.cur_cam2_x, self.initial_cam2_x, self.cam2_scrolling_right = 0, 0, True
		self.target_rotation = 0

	def load(self, filename):
		self.filename = filename
		self.reset()
		self.map = loadMapFile(filename, self.engine)
		self.elevation = self.map.getElevations("id", "TechdemoMapElevation")[0]
		self.agentlayer = self.elevation.getLayers("id", "TechdemoMapObjectLayer")[0]
		self.hero = Hero(self.model, 'PC', self.agentlayer)
		self.hero.start()
		self.girl = Girl(self.model, 'NPC:girl', self.agentlayer)
		self.girl.start()
		
		for cam in self.view.getCameras():
			self.cameras[cam.getId()] = cam
		self.cameras['main'].attachToInstance(self.hero.agent)
				
		self.view.resetRenderers()
		renderer = fife.FloatingTextRenderer.getInstance(self.view)
		textfont = self.engine.getGuiManager().createFont('content/fonts/rpgfont.png', 0, TDS.FontGlyphs);
		renderer.changeDefaultFont(textfont)
		
		renderer = self.view.getRenderer('CoordinateRenderer')
		renderer.clearActiveLayers()
		renderer.addActiveLayer(self.elevation.getLayers("id", TDS.CoordinateLayerName)[0])
		
		renderer = self.view.getRenderer('QuadTreeRenderer')
		renderer.setEnabled(True)
		renderer.clearActiveLayers()
		if TDS.QuadTreeLayerName:
			renderer.addActiveLayer(self.elevation.getLayers("id", TDS.QuadTreeLayerName)[0])
			
		self.cam2_loc = self.cameras['small'].getLocation()
		self.cam2_loc.setExactLayerCoordinates( fife.ExactModelCoordinate( 40.0, 40.0, 0.0 ))
		self.initial_cam2_x = self.cameras['small'].getLocation().getExactLayerCoordinates().x
		self.cur_cam2_x = self.initial_cam2_x
		self.cam2_scrolling_right = True
		self.cameras['small'].setEnabled(False)
		
		self.target_rotation = self.cameras['main'].getRotation()
	
	def save(self, filename):
		saveMapFile(filename, self.engine, self.map)

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == 't':
			r = self.view.getRenderer('GridRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == 'c':
			r = self.view.getRenderer('CoordinateRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == 's':
			c = self.cameras['small']
			c.setEnabled(not c.isEnabled())
		elif keystr == 'r':
			self.model.deleteMaps()
			self.metamodel.deleteDatasets()
			self.view.clearCameras()
			self.load(self.filename)
		elif keystr == 'o':
			self.target_rotation = (self.target_rotation + 90) % 360
	
	def changeRotation(self):
		currot = self.cameras['main'].getRotation()
		if self.target_rotation != currot:
			self.cameras['main'].setRotation((currot + 5) % 360)
	
	def mouseReleased(self, evt):
		if (evt.getButton() == fife.IMouseEvent.LEFT):
			newTarget = fife.ScreenPoint(evt.getX(), evt.getY())
			dy = -(newTarget.y - self.cameras['main'].toScreenCoordinates(self.cameras['main'].getLocation().getElevationCoordinates()).y)
			newTarget.z = (int)(math.tan(self.cameras['main'].getTilt()* (math.pi / 180.0)) * dy);
			target_elevcoord = self.cameras['main'].toElevationCoordinates(newTarget)
			target_elevcoord.z = 0
			l = fife.Location()
			l.setLayer(self.agentlayer)
			l.setElevationCoordinates(target_elevcoord)
			self.hero.run(l)

	def onConsoleCommand(self, command):
		result = ''
		try:
			result = str(eval(command))
		except:
			pass
		return result

		
	def pump(self):
		if self.cameras['small'].isEnabled():
			self.cam2_loc = self.cameras['small'].getLocation()
			c = self.cam2_loc.getExactLayerCoordinates()
			if self.cam2_scrolling_right:
				self.cur_cam2_x = c.x = c.x+0.1
				if self.cur_cam2_x > self.initial_cam2_x+10:
					self.cam2_scrolling_right = False
			else:
				self.cur_cam2_x = c.x = c.x-0.1
				if self.cur_cam2_x < self.initial_cam2_x-10:
					self.cam2_scrolling_right = True
			self.cam2_loc.setExactLayerCoordinates(c)
			self.cameras['small'].setLocation(self.cam2_loc)
		self.changeRotation()
