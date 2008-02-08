#!/usr/bin/python
from util_scripts.path import path
import os, sys, re

_sep = os.path.sep

def create_dir(relpath):
	d = '.' + _sep + relpath + _sep
	if not os.path.isdir(d):
		os.mkdir(d)

curid = 10000
def get_next_id():
	global curid
	curid += 1
	return curid

class Animation(object):
	def __init__(self, agent, action, direction):
		self.agent = agent
		self.action = action
		self.direction = direction
		self.image_files = []
		self.path = ''
		
	def add_image_file(self, filename):
		self.image_files.append(filename)
		
	def write_xml(self, path, delay):
		self.path = path
		f = ['<animation delay="%s" id="%d">' % (delay, get_next_id())]
		for i in self.image_files:
			f.append('\t<frame source="%s" />' % i)
		f.append('</animation>')
		open(path, 'w').write('\n'.join(f))

class Agent(object):
	def __init__(self, name):
		self.animations = {}
		self.name = ''
	
	def add_image_file(self, filename, action, direction):
		key = (action, direction)
		if not self.animations.has_key(key):
			self.animations[key] = Animation(self.name, action, direction)
		self.animations[key].add_image_file(filename)

	def write_action(self, name, animations):
		f = ['\t\t<action id="%s">' % name]
		for animation in animations:
			f.append('\t\t\t<animation source="%s" direction="%s" />' % (animation.path, animation.direction))
		f.append('\t\t</action>\n')
		return f
	
	def write_xml(self, path):
		create_dir('animations' + _sep + self.name)
		
		f = ['<?xml version="1.0"?>']
		f.append('<dataset id="%s" format="1.0">' % get_next_id())
		f.append('\t<object id="%s" blocking="1">\n' % self.name)
		
		animkeys = sorted(self.animations.keys())
		curkey = animkeys[1][0]
		frames = []
		for key in animkeys:
			self.animations[key].write_xml('animations' + _sep + self.name + _sep + key[0] + '_' + key[1] + '.xml', '100')
			if key[0] != curkey:
				f.extend(self.write_action(curkey, frames))
				curkey, frames = key[0], []
			else:
				frames.append(self.animations[key])
		f.extend(self.write_action(curkey, frames))
		curkey, frames = key, []
			
		f.append('\t</object>\n</dataset>')
		open(path, 'w').write('\n'.join(f))

class Agents(object):
	FILE_FULL_RE = re.compile(r'(?P<action>\w+)_(?P<direction>\d+)_(?P<frame>\d+)')
	FILE_PART_RE = re.compile(r'(?P<action>\w+)_(?P<direction>\d+)')
	def __init__(self):
		self.agents = {}
		
	def add_image_file(self, filename):
		parts = filename.split(_sep)
		agent = parts[-2]
		fname = parts[-1]
		m = Agents.FILE_FULL_RE.search(fname)
		if not m:
			m = Agents.FILE_PART_RE.search(fname)
			if not m:
				print 'Invalid image name format: ' + filename
				return
		action = m.group('action')
		direction = m.group('direction')
		if not self.agents.has_key(agent):
			self.agents[agent] = Agent(agent)
			self.agents[agent].name = agent
		self.agents[agent].add_image_file(filename, action, direction)
	
	def write_xml(self):
		create_dir('agents')
		create_dir('animations')
		for agent in self.agents.values():
			agent.write_xml('agents' + _sep + agent.name + '.xml')


def main():
	pathname = sys.argv[1]
	p = path(pathname)
	files = p.walkfiles('*.png')
	agents = Agents()
	for f in files:
		agents.add_image_file(f)
	agents.write_xml()

if __name__ == '__main__':
	main()
