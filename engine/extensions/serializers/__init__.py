import fife, sys
from traceback import print_exc

__all__ = ('ET', 'SerializerError', 'InvalidFormat', 'NameClash', 'NotFound', 'guarded', 'warn')

try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class SerializerError(Exception):
	pass

class InvalidFormat(SerializerError):
	pass

class NameClash(SerializerError):
	pass

class NotFound(SerializerError):
	pass

def guarded(fn):
	'''
	Use "guarded" decorator to catch serializer exceptions, and print more nice output before exception is re-raised
	Usage of decorator benefits if instance of class where decorated method resides, contains attributes
	self.filename (currently processed filename) and self.node (ET node currently under work).
	In case these are in place, filename gets printed and node dumped along with the exception info
	'''
	def wrapped(self, *args, **kwargs):
		try:
			result = fn(self, *args, **kwargs)
			return result
		except:
			print "\n\n=================== SERIALIZER EXCEPTION INFO BEGIN ======================"
			print "Exception : ", sys.exc_info()[0]
			if hasattr(self, 'filename'):
				print "Error while processing file " + self.filename
				print "----------------------------------------------------------------"
			print 'Traceback:'
			print_exc()
			if hasattr(self, 'node') and self.node:
				print "----------------------------------------------------------------"
				print "XML dump:"
				ET.dump(self.node)
			print "==================== SERIALIZER EXCEPTION INFO END =======================\n\n"
			# re-raise fife exceptions as native python exceptions, otherwise python seems to barf
			# about "cannot raise type..."
			if type(sys.exc_info()[0]) == type:
				raise SerializerError(sys.exc_info()[1])
			else:
				raise
	return wrapped

def warn(self, msg):
	print 'Warning (%s): %s' % (self.filename, msg)
