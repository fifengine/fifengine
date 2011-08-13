#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2011 by the FIFE team
#  http://www.fifengine.net
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

import xml.dom.minidom as dom
import os

from fife import fife

class PackageAnimationOutput:
	def __init__(self):
		self.frameWidth = 0
		self.frameHeight = 0
		self.outname = ''
		
class AnimPacker:
	def __init__(self):
		self.engine = fife.Engine()
		engineSetting = self.engine.getSettings()
		engineSetting.setScreenWidth(1)
		engineSetting.setScreenHeight(1)
		engineSetting.setRenderBackend('SDL')
		self.engine.init()

		self.model = self.engine.getModel()
		self.imagemgr = self.engine.getImageManager()
		self.vfs = self.engine.getVFS()
		self.rb = self.engine.getRenderBackend()
		
	def __del__(self):
		self.engine.destroy()
		
	def packAnim(self, action, output):
		""" Packs given images into atlas. """
		images = []
		regions = []
		totalwidth, totalheight = 0, 0
		out = PackageAnimationOutput()

		class Region:
			def __init__(self, left, right, top, bottom):
				self.left = left
				self.right = right
				self.top = top
				self.bottom = bottom

		anims = action.get2dGfxVisual().getActionImageAngles()
		for ii, animrot in enumerate(anims): # for each direction 
			anim = action.get2dGfxVisual().getAnimationByAngle(animrot)
			for i in range(anim.getFrameCount()): # for each frame
				image = anim.getFrame(i)
				width = image.getWidth()
				height = image.getHeight()

				if width == 0 or height == 0:
					continue
				
				if i == 0:
					totalheight += height
					if ii == 0:
						out.frameWidth = width
						out.frameHeight = height
				if ii == 0:
					totalwidth += width
				
				# find a place for given grame
				region = Region(i * width, (i + 1) * width,
								ii * height, (ii + 1) * height)
				#print region.left, region.right, region.top, region.bottom
				regions.append(region)
				images.append(image)

		# print 'Size: %dx%d\n' % (totalwidth, totalheight)
		assert len(regions) == len(images)

		# create atlas (big one) texture
		imageAtlas = self.imagemgr.loadBlank(totalwidth, totalheight)

		# modify their contents
		for i, image in enumerate(images):
			imageAtlas.copySubimage(regions[i].left, regions[i].top, image)

		# save them
		out.outname = os.path.basename(output) + '.png'
		imageAtlas.saveImage(output + '.png')
		return out
	
	def packActions(self, fifeObject, output):
		doc = dom.Document()
		object_elem = doc.createElement('object')
		object_elem.setAttribute('id', fifeObject.getId())
		object_elem.setAttribute('namespace', fifeObject.getNamespace())
		object_elem.setAttribute('blocking', str(int(fifeObject.isBlocking())))
		object_elem.setAttribute('static', str(int(fifeObject.isStatic())))
		doc.appendChild(object_elem)
		
		visual = self.object.get2dGfxVisual()
		for angle in visual.getStaticImageAngles():
			image_id = visual.getStaticImageIndexByAngle(angle)
			image = self.imagemgr.get(image_id)
			image_src = doc.createElement('image')
			image_src.setAttribute('source', os.path.basename(image.getName()))
			image_src.setAttribute('direction', str(angle))
			image_src.setAttribute('x_offset', str(image.getXShift()))
			image_src.setAttribute('y_offset', str(image.getYShift()))
			object_elem.appendChild(image_src)
			image.saveImage(os.path.join(os.path.dirname(output), os.path.basename(image.getName())))

		actions = self.object.getActionIds()
		for actionId in actions:
			print ' * action:', actionId
			action_elem = doc.createElement('action')
			action_elem.setAttribute('id', actionId)
			object_elem.appendChild(action_elem)

			action = self.object.getAction(actionId)
			out_action = os.path.join(os.path.dirname(output), actionId)
			out = self.packAnim(action, out_action)
			
			anim_elem = doc.createElement('animation')
			anim_elem.setAttribute('atlas', out.outname)
			anim_elem.setAttribute('width', str(out.frameWidth))
			anim_elem.setAttribute('height', str(out.frameHeight))		
			action_elem.appendChild(anim_elem)
					
			anims = action.get2dGfxVisual().getActionImageAngles()
			for ii, animrot in enumerate(anims):
				anim = action.get2dGfxVisual().getAnimationByAngle(animrot)

				dir_elem = doc.createElement('direction')
				dir_elem.setAttribute('dir', str(animrot))
				dir_elem.setAttribute('frames', str(anim.getFrameCount()))
				dir_elem.setAttribute('delay', str(action.getDuration() / anim.getFrameCount()))
				
				xshift = anim.getFrame(0).getXShift()
				yshift = anim.getFrame(0).getYShift()
				if xshift != 0:
					dir_elem.setAttribute('x_offset', str(xshift))
				if yshift != 0:
					dir_elem.setAttribute('y_offset', str(yshift))
					
				anim_elem.appendChild(dir_elem)
		
		fp = open(output, 'w')
		fp.write('<?fife type="object"?>\n')
		# doc.writexml(fp, '', '\t', '\n', 'ascii')
		doc.documentElement.writexml(fp, '', '\t', '\n')
		fp.close()
		
	def startPacking(self, file, outdir = None):
		if os.path.isfile(file):
			loader = fife.MapLoader(self.model, self.vfs, self.imagemgr, self.rb)
			print 'loading:', file
			loader.loadImportFile(file)
			
			# there' only one namespace and one object (single import file)
			self.ns = self.model.getNamespaces()[0]
			self.object = self.model.getObjects(self.ns)[0]
			print 'object', '"'+self.object.getId()+'"'
			
			if len(self.object.getActionIds()) is 0:
				print 'object', '"'+self.object.getId()+'"', 'doesn\'t have any defined actions, quitting.'
				return
				
			if outdir is None:
				outdir = os.path.dirname(file) + '-out'
					
			try:
				os.mkdir(outdir)
			except:
				pass
			
			output = os.path.join(outdir, os.path.basename(file))
			print 'saving to:', output
			self.packActions(self.object, output)
			self.model.deleteObjects()
			print 'done\n'
		else:
			raise file+ " is not a file!"