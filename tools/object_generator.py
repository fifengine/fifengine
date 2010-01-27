#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from util_scripts.path import path
import os, sys

DIRNAME = 'clients/rio_de_hola/objects/nature/trees'
NAMESPACE = 'http://www.fifengine.de/xml/rio_de_hola'
STATIC = 1
BLOCKING = 1

_sep = os.path.sep

def dirpath(p):
	return _sep.join(p.split(_sep)[:-1]) + _sep

def filename(p):
	return p.split(_sep)[-1]

class Animation(object):
	def __init__(self, path_to_xml):
		self.path = path_to_xml
		self.direction = int(dirpath(path_to_xml).split(_sep)[-2])
		self.action_name = dirpath(path_to_xml).split(_sep)[-3]
	
	def __str__(self):
		return self.path
	
	def relpath(self, rootpath):
		return self.path.replace(rootpath, '')

class StaticImage(object):
	def __init__(self, path_to_image):
		self.path = path_to_image
		self.direction = int(filename(path_to_image).split('.')[0])

	def __str__(self):
		return self.path

	def relpath(self, rootpath):
		return self.path.replace(rootpath, '')

OBJECT_HEADER = '<object id="%s" namespace="%s" blocking="%d" static="%d">'
ACTION_HEADER = '\t<action id="%s">'
ANIMATION_LINE = '\t\t<animation source="%s" direction="%d" />'
ACTION_FOOTER = '\t</action>'
IMAGE_LINE = '\t<image source="%s" direction="%d" />'
OBJECT_FOOTER = '</object>\n'

class Obj(object):
	def __init__(self, path_to_xml):
		self.path = path_to_xml
		self.dirpath = dirpath(path_to_xml)
		self.name = self.dirpath.split(_sep)[-2]
		try:
			num = int(self.name)
			self.name = self.dirpath.split(_sep)[-3] + ':' + self.name
		except ValueError:
			pass  # ok, object is not named as plain integer
		self.actions = {}
		self.images = []
	
	def animation_is_part_of(self, anim):
		return anim.path.find(self.dirpath) != -1
	
	def image_is_part_of(self, img):
		return (img.path.find(self.dirpath) != -1) and (img.relpath(self.dirpath).find(_sep) == -1)
	
	def add_animation(self, animation):
		if not self.actions.has_key(animation.action_name):
			self.actions[animation.action_name] = []
		self.actions[animation.action_name].append(animation)
	
	def add_image(self, image):
		self.images.append(image)

	def write_xml(self):
		lines = []
		a = lines.append
		a(OBJECT_HEADER % (self.name, NAMESPACE, BLOCKING, STATIC))
		self.images.sort(lambda x, y: cmp(str(x), str(y)))
		for i in self.images:
			a(IMAGE_LINE % (i.relpath(self.dirpath), i.direction))
		
		for action_name in sorted(self.actions.keys()):
			a(ACTION_HEADER % action_name)
			self.actions[action_name].sort(lambda x, y: cmp(str(x), str(y)))
			for animation in self.actions[action_name]:
				a(ANIMATION_LINE % (animation.relpath(self.dirpath), animation.direction))
			a(ACTION_FOOTER)
		a(OBJECT_FOOTER)
		
		print 'FILE : ', self.path
		print '\n'.join(lines)
		print ''
		open(self.path, 'w').write('\n'.join(lines))
	
	def __str__(self):
		return self.name

p = path(DIRNAME)
obj_files = [str(f) for f in p.walkfiles('object.xml')]
anim_files = [str(f) for f in p.walkfiles('animation.xml')]
images = [str(f) for f in p.walkfiles('*.png')]

objects = []
# iterate all object.xml files
for o in obj_files:
	obj = Obj(str(o))
	objects.append(obj)
	
	for a in anim_files:
		anim = Animation(str(a))
		if obj.animation_is_part_of(anim):
			obj.add_animation(anim)
	for i in images:
		img = StaticImage(str(i))
		if obj.image_is_part_of(img):
			obj.add_image(img)

for o in objects:
	o.write_xml()

print "all done"