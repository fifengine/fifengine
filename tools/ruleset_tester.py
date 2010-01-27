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

import Tkinter as TK
import math

class Ruleset (object):
	def __init__ (self):
		# move to somewhere else later (file ...)
		self.ST_name = 'Strength'
		self.ST_desc = 'The strenght of your character, affects hitpoints, carry weight and the weapons skill.'
		self.AG_name = 'Agility'
		self.AG_desc = 'The agility of your character influences armor class, action points and the weapons skill.'
		self.IN_name = 'Intelligence'
		self.IN_desc = 'Your intelligence is important for the technology skill and your ability to talk with other characters.'
		
		self.weapon_name = 'Weapons'
		self.weapon_desc = 'A high weapons skill will let you fire weapons more precisly.'
		self.tech_name = 'Technology'
		self.tech_desc = 'Boost this skill to become a real hacker.'
		self.talk_name = 'Talk'
		self.talk_desc = 'A high talk skill can save bullets.'

	def set_main (self, ST, AG, IN):
		self.ST = ST
		self.AG = AG
		self.IN = IN	
		# now calc boni
		self.calc_boni()

	def calc_boni (self):
		self.STbonus = 0
		self.AGbonus = 0
		self.INbonus = 0
		
		if self.ST > 4 :
			self.STbonus = (self.ST - 4) * 1
		if self.AG > 2 :
			self.AGbonus = (self.AG - 2) * 1			
		if self.IN > 4 :
			self.INbonus = (self.AG - 5) * 1
				
	def skill_weapon (self, type, count):
		# weapon = 2 x (ST + AG) + 10%
		#if self.weapon == False:
		self.weapon = 2 * (self.ST + self.AG) + self.ST + self.AG

		if type == 1 :
			# increase
				if count != 0 :
					self.weapon = self.weapon + count
					
		if type == 0 :
			# decrease
				if (self.weapon - count) != 0 :
					self.weapon = self.weapon - count
				else: 
					self.weapon = 0
		
	def skill_tech (self, type, count):
		self.tech = 3 * self.IN + 2 * self.INbonus
		
		if type == 1 :
			# increase
				if count != 0 :
					self.tech = self.tech + count
		
		if type == 0 :
			# decrease
				if (self.tech - count) != 0 :
					self.tech = self.tech - count
				else:
					self.tech = 0
		
	def skill_talk (self, type, count):
		self.talk = 2 * self.IN + self.INbonus

		if type == 1 :
			# increase
				if count != 0 :
					self.talk = self.talk + count
		
		if type == 0 :
			# decrease
				if (self.talk - count) != 0 :
					self.talk = self.talk - count
				else:
					self.talk = 0
		
	def calc_skills (self):
		self.skill_weapon(0,0)
		self.skill_tech(0,0)
		self.skill_talk(0,0)	
	

class GUI (object):
	
	def __init__  (self):
		self.root = TK.Tk()
		self.root.title('FIFE Techdemo Ruleset-tester')
		self.root.geometry("350x100")
		
		# inject ruleset
		self.RULES = Ruleset()
		self.RULES.set_main(2,2,2)
		self.RULES.calc_skills()

		self.frm1 = TK.Frame(master=self.root)
		self.frm1.grid(column = 1, row = 1)

		self.create_widgets()
		self.create_buttons()

	def create_widgets (self):

		mainstat = {
		"a" : [self.RULES.ST_name, self.RULES.ST_desc, self.RULES.ST, self.RULES.STbonus],
		"b" : [self.RULES.AG_name, self.RULES.AG_desc, self.RULES.AG, self.RULES.AGbonus],
		"c" : [self.RULES.IN_name, self.RULES.IN_desc, self.RULES.IN, self.RULES.INbonus]
		}
		
		skills = {
		"a" : [self.RULES.weapon_name, self.RULES.weapon_desc, self.RULES.weapon],
		"b" : [self.RULES.tech_name, self.RULES.tech_desc, self.RULES.tech],
		"c" : [self.RULES.talk_name, self.RULES.talk_desc, self.RULES.talk]	
		}

		col = 1
		row = 2

		# container for generated entry-widgets
		self.entries = []
		self.entry_vars = []

		# create widgets for mainstat
		for key in mainstat:
			label = TK.Label(self.frm1, text=mainstat[key][0], relief= TK.GROOVE, bd=0, width=10, anchor=TK.W)
			label.grid(column = col, row = row)			
			col = col + 1

			self.entry_vars.append(TK.StringVar(self.root))
			entry_key = TK.Entry(self.frm1, width=2, textvariable=self.entry_vars[-1])
			entry_key.grid(column = col, row = row, padx = 0)
			entry_key.insert(0, mainstat[key][2])
			col = col + 1

			label = TK.Label(self.frm1, text=mainstat[key][3], relief= TK.RIDGE, bd=2, width=3)
			label.grid(column = col, row = row)				
			row = row + 1
			
			col = 1			
			
			self.entries.append(entry_key)	

		col = 5
		row = 2

		for key in skills:
			label = TK.Label(self.frm1, text=skills[key][0], relief= TK.GROOVE, bd=0, width=10, anchor=TK.W)
			label.grid(column = col, row = row, padx = 4)			
			col = col + 1

			label = TK.Label(self.frm1, text=skills[key][2], relief= TK.RIDGE, bd=2, width=3)
			label.grid(column = col, row = row)				
			row = row + 1
			
			col = 5	

	def create_buttons (self):
		col = 6
		row = 6
		
		button_calc = TK.Button(self.frm1, text='Calculate', command=self.calc)
		button_calc.grid(column = col, row = row)
		
		col = 7
		
		button_quit = TK.Button(self.frm1, text='Quit', command=self.exit)
		button_quit.grid(column = col, row = row)
		
		pass
		
	def calc (self):
		# prepare entrys for calculation
		
		tmp_vars = []
		for i,var in enumerate(self.entry_vars) :
			inumber = var.get()
			tmp_vars.append(int(inumber))
			
		# set new mainstats & skill values
		# 0 = weapons
		# 2 = talk
		# 1 = technology
		self.RULES.set_main(tmp_vars[0],tmp_vars[2],tmp_vars[1])
		self.RULES.calc_skills()
				
		# print new stats
		self.create_widgets()

	def exit(self):
		self.root.quit()

	def run (self):
		self.root.mainloop()	
 
# demo 
if __name__ == '__main__':
	gui = GUI()    
	gui.run()
