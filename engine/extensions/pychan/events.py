#coding: utf-8

"""
PyChan event handling
=====================

Users shouldn't need to use this module directly.
L{widgets.Widget.capture} and L{widgets.Widget.mapEvents} provide
a convenient API to capture events.

Nevertheless to understand how its supposed to work
take a look at L{EventMapper} and L{EventListener}

Available Events
----------------

"""

import fife
import exceptions
import manager
import tools

EVENTS = [
	"mouseEntered",
	"mouseExited",
	"mousePressed",
	"mouseReleased",
	"mouseClicked",
	"mouseMoved",
	"mouseDragged",
	"action",
	"keyPressed",
	"keyReleased",
]

# Add the EVENTS to the docs.
__doc__ += "".join([" - %s\n" % event for event in EVENTS])

MOUSE_EVENT, KEY_EVENT, ACTION_EVENT = range(3)
def getEventType(name):
	if "mouse" in name:
		return MOUSE_EVENT
	if "key" in name:
		return KEY_EVENT
	return ACTION_EVENT


CALLBACK_NONE_MESSAGE = """\
You passed None as parameter to %s.capture, which would normally remove a mapped event.
But there was no event mapped. Did you accidently call a function instead of passing it?
"""

class EventListener(fife.GUIEventListener):
	"""
	Redirector for event callbacks.
	Use *only* from L{EventMapper}.

	This class uses the SWIG director feature - overriden
	virtual methods are called from C++ to - listen to
	Guichan events.

	When the module is first loaded the event handler
	methods are auto-generated from the list L{EVENTS}.
	This is effectively the same code as::
	  def mouseEntered(self,event):
	    self._redirectEvent("mouseEntered",event)

	This way L{EVENTS} and the actually receivable events
	are forced to be in sync.
	"""
	def __init__(self,debug=True):
		super(EventListener,self).__init__()
		self.events = {}
		self.indent = 0
		self.debug = debug
		self.guimanager = manager.Manager.manager.guimanager

	def _redirectEvent(self,name,event):
		self.indent += 4
		event = self.translateEvent(getEventType(name), event)
		if name in self.events:
			if self.debug: print "-"*self.indent, name
			for f in self.events[name].itervalues():
				f( event )
		self.indent -= 4

	def translateEvent(self,event_type,event):
		if event_type == MOUSE_EVENT:
			return self.guimanager.translateMouseEvent(event)
		if event_type == KEY_EVENT:
			return self.guimanager.translateKeyEvent(event)
		return event

def _redirect(name):
	def redirectorFunc(self,event):
		self._redirectEvent(name,event)
	return redirectorFunc

for event_name in EVENTS:
	setattr(EventListener,event_name,_redirect(event_name))

class EventMapper(object):
	"""
	Handles events and callbacks for L{widgets.Widget}
	and derived classes.

	Every PyChan widget has an L{EventMapper} instance
	as attribute *event_mapper*.

	This instance handles all necessary house-keeping.
	Such an event mapper can be either *attached* or
	*detached*. In its attached state an L{EventListener}
	is added to the Guichan widget and will redirect
	the events to the callbacks.

	In its detached state no events are received from the
	real Guichan widget.

	The event mapper starts in the detached state.
	When a new event is captured the mapper attaches itself
	automatically. The widget doesn't need to handle that.
	"""
	def __init__(self,widget):
		super(EventMapper,self).__init__()
		self.widget = widget
		self.listener = EventListener()
		self.is_attached = False
		self.debug = manager.Manager.manager.debug

	def __del__(self):
		self.detach()
	def __repr__(self):
		return "EventMapper(%s)" % repr(self.widget)

	def attach(self):
		"""
		Start receiving events.
		No need to call this manually.
		"""

		if self.is_attached:
			return
		if not self.listener.events:
			return
		if self.debug: print "Attach:",self
		self.widget.real_widget.addKeyListener( self.listener )
		self.widget.real_widget.addMouseListener( self.listener )
		self.widget.real_widget.addActionListener( self.listener )
		self.is_attached = True

	def detach(self):
		"""
		Stop receiving events.
		No need to call this manually.
		"""

		if not self.is_attached:
			return
		if self.debug: print "Detach:",self
		self.widget.real_widget.removeKeyListener( self.listener )
		self.widget.real_widget.removeMouseListener( self.listener )
		self.widget.real_widget.removeActionListener( self.listener )
		self.is_attached = False

	def capture(self,event_name,callback,group_name):
		if event_name not in EVENTS:
			raise exceptions.RuntimeError("Unknown eventname: " + event_name)

		if callback is None:
			if self.isCaptured(event_name):
				del self.listener.events[event_name][group_name]
				if not self.listener.events:
					self.detach()
			elif self.debug:
					print CALLBACK_NONE_MESSAGE % str(self.widget)
			return

		if not callable(callback):
			raise RuntimeError("An event callback must be either a callable or None - not %s" % repr(callback))

		def captured_f(event):
			tools.applyOnlySuitable(callback,event=event,widget=self.widget)

		if event_name not in self.listener.events:
			self.listener.events[event_name] = {group_name : captured_f}
		else:
			self.listener.events[event_name][group_name] = captured_f
		self.attach()

	def isCaptured(self,event_name,group_name):
		return event_name in self.listener.events and group_name in self.listener.events[event_name]

	def getCapturedEvents(self):
		return self.listener.events.keys()


def splitEventDescriptor(name):
	""" Utility function to split "widgetName/eventName" descriptions into tuples. """
	L = name.split("/")
	if len(L) not in (1,2,3):
		raise exceptions.RuntimeError("Invalid widgetname / eventname combination: " + name)
	if len(L) == 1:
		L = L[0],"action"
	elif L[1] not in EVENTS:
		raise exceptions.RuntimeError("Unknown event name: " + name)
	if len(L) == 2:
		L = L[0],L[1],"default"
	return L

