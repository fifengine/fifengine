#! /usr/bin/env python
import os,sys,string
SUFFIX_LIST = [ "aa","ab","at","ae","ak","al","an","ao","ap","ar","as","aq" ]
IDLE,WALK,RUN,CLIMB,PICKUP,USE,DODGE,HITFRONT,HITBACK,KICK,THROW,PUNCH =  range(len(SUFFIX_LIST))
ACTION_IDS = range(len(SUFFIX_LIST))
ACTION_NAMES = [
"IDLE","WALK","RUN","CLIMB","PICKUP","USE","DODGE","HITFRONT","HITBACK","KICK","THROW","PUNCH"
]
SUFFIX_DICT = dict(zip(SUFFIX_LIST,ACTION_IDS))

USAGE="""
*** make_xml.py ***
Generates XML Critter information from frm info in critter.dat
--list			List all critters
--to-xml CRITTER	Output XML generated for a critter CRITTER
			into a file in the current directory
--all-xml DIRECTORY	Output XML files for all critters in the list
			into the directory DIRECTORY
"""

CRITTER_XML = string.Template("""\
<!--
	This is an auto-generated XML file
	defining the animation actions a
	critter can perform.
	
	Generator: make_xml.py
-->
<complex-animation name="$name" geometry="1">
$actions
</complex-animation>""")

ACTION_XML = string.Template("""\
	<!-- ACTION: $name FRAMES: $numFrames FPS: $fps -->
	<action id="$id" direction="255">
		<animation src="$filename"/>
$partialActions
	</action>
""")

PARTIAL_ACTION_XML=string.Template("""\
		<partial-action number="$number">
			<frames start="$startFrame" number="$endFrame"/>
			<step number="$stepNumber"/>
			<partial-action action="$nextActionId" number="$nextNumber" direction="255"/>
		</partial-action>
""")

MOTION_XML = string.Template("""\
		<motion distance="$distance"/>
""")

class Action(object):
	def __init__(self,critter,suffix):
		self.critter = critter
		self.suffix = suffix
		self.id = SUFFIX_DICT[ suffix ]
		self.filename = "art/critters/" + self.critter.basename + self.suffix +".frm"
		self.frminfo_parsed = False
		
	def __str__(self):
		if self.frminfo_parsed:
			return self.suffix + "[nframes="+str(self.numframes)+",fps="+str(self.fps)+"]"
		return self.suffix + "[id="+str(self.id)+"]"
	
	def parseFrmInfo(self):
		if self.frminfo_parsed: 
			return
		frminfo = os.popen("./frminfo -t " + self.filename).read()
		print >> sys.stderr, "Parsing FRMINFO: ",self.filename
		frminfo = [tuple(line.replace(" ","").split("=")) for line in frminfo.split("\n")[:-1]]
		values = dict(frminfo)
		self.numframes = int(values['num_frames'])
		self.fps = int(values['frames_per_second'])
		self.shiftx = int(values["shift_xy"].split(',')[0])
		self.frminfo_parsed = True
	def measureDistance(self):
		cmd ="./frminfo -t " +self.filename + " -i " + str(self.numframes-1)+ " -d 1"
		frminfo = os.popen(cmd).read()
		frminfo = [tuple(line.replace(" ","").split("=")) for line in frminfo.split("\n")[:-1]]
		values = dict(frminfo)
		x,y = map(int,values["shift_xy"].split(","))
		distance = (x - self.shiftx+ 16) / 32
		print >> sys.stderr, ACTION_NAMES[self.id],"distance:",distance
		return distance
		
	def generatePartialActions(self,actionId):
		partialActions = ""
		npas=1
		if actionId in  [WALK,RUN]:
			npas = self.measureDistance()
			partialActions += MOTION_XML.substitute(distance=npas)
			
		if npas==0:
			npas=1
			
		delta = (self.numframes/npas)
		startFrame=0
		endFrame = delta 
		nextActionId = actionId	
		for number in range(npas):
			nextNumber = (number+1) % npas
			stepNumber=number
			partialActions += PARTIAL_ACTION_XML.safe_substitute(locals())
			startFrame += delta
		return partialActions
		
	def buildXML(self):
		#if self.id not in [RUN,WALK]: return ""
		self.parseFrmInfo()
		id = self.id
		name = ACTION_NAMES[id]
		fps = self.fps
		numFrames = self.numframes
		filename = self.filename
		partialActions = self.generatePartialActions(id)
		return ACTION_XML.safe_substitute(locals())
		
class Critter(object):
	def __init__(self,basename):
		self.basename = basename
		self.actions = {}
		self.xml=""
	def __str__(self):
		return self.basename +"("+",".join(map(str,self.actions.values()))+")"
	
	def addAction(self,suffix):
		action = Action(self,suffix)
		self.actions[ action.id ] = action
		
	def buildXML(self):
		if self.xml: return self.xml
		name = self.basename
		actions = ""
		for action in self.actions.values():
			actions += action.buildXML()
		
		return CRITTER_XML.safe_substitute(locals())
		
def readCritters():
	critters = os.popen("./frminfo -l 14").read().split("\n")[1:]
	critter_types = {}
	for critter in critters:
		basename = os.path.basename(critter).split('.')[0][:6]
		if not basename:
			continue
		
		action = os.path.basename(critter).split('.')[0][6:]
		if not critter_types.has_key(basename):
			critter_types[ basename ] = Critter(basename)
		critter = critter_types[ basename ]
		critter.addAction( action )
		
	return critter_types
		

def exportToXML(critter_name,critters,directory="./"):
	if not critters.has_key(critter_name):
		print >> sys.stderr, "Critter ",critter_name," not found."
		sys.exit(1)
		
	critter = critters[critter_name]
	
	xml= critter.buildXML()
	fname = os.path.join(directory,critter.basename + ".xml")
	file =open(fname,"w")
	file.write(xml)
	print >> sys.stderr, "Wrote to ",fname
	
def main():
	if len(sys.argv) == 1:
		print USAGE
		sys.exit(1)
	print >> sys.stderr, "Reading Critter List"
	critters = readCritters()
	if sys.argv[1] == '--list':
		for name,critter in critters.items():
			name_getter = lambda x: ACTION_NAMES[x]
			print name," actions  = ",map(name_getter,critter.actions.keys())
		sys.exit(0)
			
	if len(sys.argv) == 2:
		print USAGE
		sys.exit(1)
		
	if sys.argv[1] == '--to-xml':
		exportToXML( sys.argv[2], critters)
	if sys.argv[1]=='--all-xml':
		for critter_name in critters.keys():
			exportToXML(critter_name,critters,sys.argv[2])
	
if __name__=="__main__": main();