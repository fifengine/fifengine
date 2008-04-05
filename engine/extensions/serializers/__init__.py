import fife, sys
from traceback import print_exc

__all__ = ('ET', 'SerializerError', 'InvalidFormat', 'WrongFileType', 'NameClash', 'NotFound', 'warn')

try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class SerializerError(Exception):
	pass

class InvalidFormat(SerializerError):
	pass

class WrongFileType(SerializerError):
	pass

class NameClash(SerializerError):
	pass

class NotFound(SerializerError):
	pass

def warn(self, msg):
	print 'Warning (%s): %s' % (self.filename, msg)
