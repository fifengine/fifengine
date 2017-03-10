#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

try:
	import Tkinter as TK
except ImportError:
	# Python 3
	import tkinter as TK
	xrange = range
import math, time

LMB = 0x100
RMB = 0x400
DBLCLICK_TRESHOLD = 0.3 # secs
	
class Shape(object):
	def __init__(self, center_pt, sidecount):
		assert (sidecount in (4, 6))
		self.rotation = 0
		self.tilting = 0
		self.zoomval = 70
		self.center_pt = center_pt
		startangle = 0
		if sidecount == 6:
			startangle = math.pi / 6.0
		self.angles = [startangle + 2 * math.pi * (s / float(sidecount)) for s in xrange(sidecount-1)]
		shape_pts = [(0,0,0)]
		maxy, miny = 0, 0
		for a in self.angles:
			x, y, z = shape_pts[-1]
			newx, newy = math.cos(a) + x, math.sin(a) + y
			maxy, miny = max(maxy, newy), min(miny, newy)
			shape_pts.append((newx, newy, 0))
		xmove, ymove = 0.5, (abs(miny) + maxy) / 2
		self.shape_pts = [(pt[0]-xmove, pt[1]-ymove, 0) for pt in shape_pts]
		self.reflections = self.create_shape_reflections()
		
	def create_shape_reflections(self):
		reflections = []
		sides = len(self.shape_pts)
		if sides == 4:
			for x,y in ((1,0), (1,1), (0,1), (-1,1), (-1,0), (-1,-1), (0,-1), (1,-1)):
				reflections.append([(pt[0]+x, pt[1]+y, 0) for pt in self.shape_pts])
		elif sides == 6:
			W = math.cos(math.pi/6.0)
			H = math.sin(math.pi/6.0)
			for x,y in ((2*W,0), (W,1+H), (-W,1+H), (-2*W,0), (-W,-1-H)):
				reflections.append([(pt[0]+x, pt[1]+y, 0) for pt in self.shape_pts])
		return reflections
	
	def tilt(self, degrees):
		self.tilting += math.radians(degrees)
		if self.tilting < 0:
			self.tilting = 0
		elif self.tilting > math.pi/2 * 0.95:
			self.tilting = math.pi/2 * 0.95
	
	def rotate(self, degrees):
		self.rotation += math.radians(degrees)
		if self.rotation < 0:
			self.rotation = 0
		elif self.rotation > math.pi/2 * 0.95:
			self.rotation = math.pi/2 * 0.95
		
	def zoom(self, amount):
		self.zoomval += amount
		if self.zoomval < 10:
			self.zoomval = 10
		if self.zoomval > 150:
			self.zoomval = 150
				
	def transform_pts(self, pts):
		rotated_pts = []
		for x, y, z in pts:
			rotated_pts.append((x * math.cos(self.rotation) - y * math.sin(self.rotation),
			                  -(x * math.sin(self.rotation) + y * math.cos(self.rotation)),
			                   z))
		tilted_pts = []
		for x, y, z in rotated_pts:
			tilted_pts.append((x,
			                   y * math.cos(self.tilting) - z * math.sin(self.tilting),
		                           z * math.sin(self.tilting) + z * math.cos(self.tilting)))
	
		zoomed_pts = []
		for x, y, z in tilted_pts:
			zoomed_pts.append((int(round(self.center_pt[0] + self.zoomval * x)),
			                   int(round(self.center_pt[1] + self.zoomval * y))))
		return zoomed_pts
	
	def get_screen_pts(self):
		return self.transform_pts(self.shape_pts)
		
	def get_screen_bounding_box(self, screenpts):
		x2, y2 = x1, y1 = screenpts[0]
		for pt in screenpts:
			x1, y1 = min(x1, pt[0]), min(y1, pt[1])
			x2, y2 = max(x2, pt[0]), max(y2, pt[1])
		return (x1, y1), (x2, y2)
		
	def get_reflections(self):
		return [self.transform_pts(reflection) for reflection in self.reflections]
		
	def get_tilting(self):
		return math.degrees(self.tilting)
		
	def get_rotation(self):
		return math.degrees(self.rotation)
	
class Gui(object):
	W = 400
	H = 400
	def __init__(self):
		self.sides = (4, 6)
		self.cur_side_index = 0
		self.shape = Shape((Gui.W/2, Gui.H/2), self.sides[0])
		self.prev_mouse_pt = (0, 0)
		self.root = TK.Tk()
		self.canvas = TK.Canvas(self.root, width=Gui.W, height=Gui.H)
		self.canvas.create_window(285, 280, window=TK.Frame(self.canvas, relief=TK.GROOVE, borderwidth=2), anchor=TK.CENTER)
		self.canvas.bind('<Motion>', self.on_move)
		self.canvas.bind('<ButtonPress-1>', self.on_mb)
		self.canvas.bind('<ButtonPress-3>', self.on_mb)
		self.canvas.bind('<Configure>', self.on_resize)
		self.canvas.pack()
		self.update_view()
		self.last_click_time = time.time()
			
	def on_resize(self, event):
		pass
		
	def on_move(self, event):
		if (event.state & RMB):
			self.shape.zoom((self.prev_mouse_pt[1] - event.y) / 3)
			self.update_view()
		elif (event.state & LMB):
			self.shape.rotate((self.prev_mouse_pt[0] - event.x) / 2)
			self.shape.tilt((self.prev_mouse_pt[1] - event.y) / 2)
			self.update_view()
			self.prev_mouse_pt = (event.x, event.y)
		
	def on_mb(self, event):
		self.prev_mouse_pt = (event.x, event.y)
		if (time.time() - self.last_click_time) < DBLCLICK_TRESHOLD:
			self.cur_side_index = (self.cur_side_index+1) % len(self.sides)
			newshape = Shape((Gui.W/2, Gui.H/2), self.sides[self.cur_side_index])
			newshape.rotation = self.shape.rotation
			newshape.tilting = self.shape.tilting
			newshape.zoomval = self.shape.zoomval
			self.shape = newshape
			self.update_view()
		self.last_click_time = time.time()
	
	def draw_shape(self, shapepts, boundpts=None, fillshape=True):
		if boundpts:
			pt1, pt2 = boundpts
			self.canvas.create_polygon(pt1[0], pt1[1], pt2[0], pt1[1], pt2[0], pt2[1], pt1[0], pt2[1], outline="blue", fill="", tag="boundingbox")
		fillcolor = "white"
		if not fillshape:
			fillcolor = ""
		self.canvas.create_polygon(fill=fillcolor, outline="black", tag="shape", *self.flatten_pts(shapepts))
	
	def update_texts(self, size, transform):
		self.canvas.create_text(10, 20, text="Bounding box size x:%d, y:%d" % (size[0], size[1]), tag="size", anchor=TK.NW)
		self.canvas.create_text(10, 40, text="Transform x:%d, y:%d" % (transform[0], transform[1]), tag="transform", anchor=TK.NW)
		self.canvas.create_text(10, 60, text="Tilting: %d" % self.shape.get_tilting(), tag="tilting", anchor=TK.NW)
		self.canvas.create_text(10, 80, text="Rotation: %d" % self.shape.get_rotation(), tag="rotation", anchor=TK.NW)
		self.canvas.create_text(10, 330, text="Doubleclick to change shape", anchor=TK.NW)
		self.canvas.create_text(10, 350, text="Right button + drag = zoom", anchor=TK.NW)
		self.canvas.create_text(10, 370, text="Left button + drag = tilt/rotate", anchor=TK.NW)
	
	def flatten_pts(self, pts):
		return [c for pt in pts for c in pt]
			
	def update_view(self):
		self.canvas.delete("all")
		shapepts = self.shape.get_screen_pts()
		boundpts = self.shape.get_screen_bounding_box(shapepts)
		size = boundpts[1][0] - boundpts[0][0], boundpts[1][1] - boundpts[0][1]
		reflections = self.shape.get_reflections()
		transform = (reflections[0][0][0] - shapepts[0][0], -(reflections[0][0][1] - shapepts[0][1]))
	
		for reflection_pts in reflections[1:]:
			self.draw_shape(reflection_pts, fillshape=False)
		
		self.draw_shape(shapepts, boundpts)
		transform_boundpts = self.shape.get_screen_bounding_box(reflections[0])
		self.draw_shape(reflections[0], transform_boundpts, fillshape=False)
	
		self.update_texts(size, transform)
	
	def run(self):
		self.root.mainloop()
	
if __name__ == '__main__':
	gui = Gui()
	gui.run()
