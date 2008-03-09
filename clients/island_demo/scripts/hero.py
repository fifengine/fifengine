import random
from agent import Agent
import settings as TDS

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_KICK = 0, 1, 2, 3

class Hero(Agent):
	def __init__(self, model, agentName, layer, uniqInMap=True):
		super(Hero, self).__init__(model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE
		self.idlecounter = 1

	def onInstanceActionFinished(self, instance, action):
		self.idle()
		if action.Id() != 'idle':
			self.idlecounter = 1
		else:
			self.idlecounter += 1
		if self.idlecounter % 7 == 0:
			txtindex = random.randint(0, len(TDS.heroTexts) - 1)
			instance.say(TDS.heroTexts[txtindex], 2500)
	
	def start(self):
		self.idle()
	
	def idle(self):
		self.state = _STATE_IDLE
		self.agent.act('idle', self.agent.getFacingLocation())
		
	def run(self, location):
		self.state = _STATE_RUN
		self.agent.move('run', location, 4 * TDS.TestAgentSpeed)
	
	def kick(self, location):
		self.state = _STATE_KICK
		self.agent.act('kick', location)