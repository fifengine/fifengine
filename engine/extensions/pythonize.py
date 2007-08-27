# coding: utf-8
import fife, re

__doc__ = """\
Pythonize FIFE

Import this extension to get a more
pythonic interface to FIFE.

Currently it implements the following
conveniences:

* FIFE Exceptions print their message.
* Automatic property generation for:
  * fife.Engine
  * fife.Instance
  * fife.Image
  * fife.Animation

"""

__all__ = ()

fife.Exception.__str__ = fife.Exception.getMessage

classes = [ fife.Engine, fife.Instance, fife.Point, fife.Image, fife.Animation ]

def createProperties():
	""" Autocreate properties for getXYZ/setXYZ functions.
	"""
	try:
		import inspect
		getargspec = inspect.getargspec
	except ImportError:
		print "Pythonize: inspect not available - properties are generated with dummy argspec."
		getargspec = lambda func : ([],'args',None,None)

	def isSimpleGetter(func):
		if not callable(func):
			return False
		try:
			argspec = getargspec(func)
			return not (argspec[0] and any(argspec[2:]))
		except TypeError, e:
			#print func, e
			return False
	
	getter = re.compile(r"^get[A-Z]")
	for class_ in classes:
		methods = [(name,attr) for name,attr in class_.__dict__.items()
						if isSimpleGetter(attr) ]
		getters = []
		for name,method in methods:
			if getter.match(name):
				settername = 'set' + name[3:]
				propertyname = name[3].lower() + name[4:] 
				setter = dict(methods).get(settername,None)
				setattr(class_,propertyname, property(method, setter))

createProperties()
