import random
from agent import Agent
from settings import Setting

TDS = Setting()

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_KICK, _STATE_TALK = xrange(5)

class Hero(Agent):
	def __init__(self, model, agentName, layer, uniqInMap=True):
		super(Hero, self).__init__(model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE
		self.idlecounter = 1

	def onInstanceActionFinished(self, instance, action):
		self.idle()
		if action.getId() != 'stand':
			self.idlecounter = 1
		else:
			self.idlecounter += 1
		if self.idlecounter % 7 == 0:
			heroTexts = TDS.readSetting("heroTexts", type='list', text=True)
			txtindex = random.randint(0, len(heroTexts) - 1)
			instance.say(heroTexts[txtindex], 2500)

	def start(self):
		self.idle()

	def idle(self):
		self.state = _STATE_IDLE
		self.agent.act('stand', self.agent.getFacingLocation())

	def run(self, location):
		self.state = _STATE_RUN
		self.agent.move('run', location, 4 * float(TDS.readSetting("TestAgentSpeed")))

	def kick(self, target):
		self.state = _STATE_KICK
		self.agent.act('kick', target)

	def talk(self, target):
		self.state = _STATE_TALK
		self.agent.act('talk', target)
