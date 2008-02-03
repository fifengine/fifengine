import common, fife

class Agent(fife.InstanceListener):
	def __init__(self, model, agentName, layer):
		fife.InstanceListener.__init__(self)
		self.model = model
		self.agentName = agentName
		self.layer = layer
		self.agent = layer.getInstances('name', agentName)[0]
		self.agent.addListener(self)

	def OnActionFinished(self, instance, action):
		raise ProgrammingError('No OnActionFinished defined for Agent')

	def start(self):
		raise ProgrammingError('No start defined for Agent')
