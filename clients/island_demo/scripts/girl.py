from agent import Agent
import settings as TDS
import fife

_STATE_NONE, _STATE_IDLE, _STATE_RUN = 0, 1, 2

class Girl(Agent):
	def __init__(self, model, agentName, layer, uniqInMap=True):
		super(Girl, self).__init__(model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE
		self.waypoints = ((67, 80), (75, 44))
		self.waypoint_counter = 0

	def OnActionFinished(self, instance, action):
		if self.state == _STATE_RUN:
			self.idle()
		else:
			self.run(self.getNextWaypoint())

	def getNextWaypoint(self):
		self.waypoint_counter += 1
		l = fife.Location()
		l.setLayer(self.layer)
		l.setLayerCoordinates(fife.ModelCoordinate(*self.waypoints[self.waypoint_counter % len(self.waypoints)]))
		return l
	
	def start(self):
		self.run(self.getNextWaypoint())
	
	def idle(self):
		self.state = _STATE_IDLE
		self.agent.act('default', self.agent.getFacingLocation(), False)
		
	def run(self, location):
		self.state = _STATE_RUN
		self.agent.move('run', location, 3 * TDS.TestAgentSpeed)
	