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

from fife import fife
from fife.extensions.serializers import ET

def loadXMLAnimation(engine, filename):
	f = engine.getVFS().open(filename)
	f.thisown = 1

	imgMgr = engine.getImageManager()
	
	tree = ET.parse(f)
	node = tree.getroot()

	animation = fife.Animation.createAnimation()
	
	common_frame_delay = int(node.get('delay', 0))
	x_offset = int(node.get('x_offset', 0))
	y_offset = int(node.get('y_offset', 0))
	animation.setActionFrame(int(node.get('action', 0)))

	frames = node.findall('frame')
	if not frames:
		raise InvalidFormat('animation without <frame>s')

	for frame in frames:
		source = frame.get('source')
		if not source:
			raise InvalidFormat('animation without <frame>s')

		frame_x_offset = int(frame.get('x_offset', x_offset))
		frame_y_offset = int(frame.get('y_offset', y_offset))
		frame_delay = int(frame.get('delay', common_frame_delay))

		# xml paths are relative to the directory of the file they're used in.
		path = filename.split('/')
		path.pop()
		path.append(str(source))

		image_file = '/'.join(path)

		img = imgMgr.create(image_file)
		img.setXShift(frame_x_offset)
		img.setYShift(frame_y_offset)
		
		animation.addFrame(img, frame_delay)
		
#		animation.thisown = 0
	return animation

