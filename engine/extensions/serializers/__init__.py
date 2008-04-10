import fife, sys, os
from traceback import print_exc

__all__ = ('ET', 'SerializerError', 'InvalidFormat', 'WrongFileType', 'NameClash', 'NotFound', 'warn', 'root_subfile', 'reverse_root_subfile')

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

def root_subfile(masterfile, subfile):
	'''
	Returns new path for given subfile (path), which is rooted against masterfile
	E.g. if masterfile is ./../foo/bar.xml and subfile is ./../foo2/subfoo.xml,
	returned path is ../foo2/subfoo.xml
	NOTE: masterfile is expected to be *file*, not directory. subfile can be either
	'''
	s = '/'
	
	masterfile = os.path.abspath(masterfile)
	subfile = os.path.abspath(subfile)
	
	master_fragments = masterfile.split(s)
	sub_fragments = subfile.split(s)
	
	master_leftovers = []
	sub_leftovers = []
	
	for i in xrange(len(master_fragments)):
		try:
			if master_fragments[i] == sub_fragments[i]:
				master_leftovers = master_fragments[i+1:]
				sub_leftovers = sub_fragments[i+1:]
		except IndexError:
			break
	
	pathstr = ''
	for f in master_leftovers[:-1]:
		pathstr += '..' + s
	pathstr += s.join(sub_leftovers)
	return pathstr

def reverse_root_subfile(masterfile, subfile):
	'''
	does inverse operation to root_subfile. E.g. 
	E.g. if masterfile is ./../foo/bar.xml and subfile is ../foo2/subfoo.xml,
	returned path ./../foo2/subfoo.xml
	Usually this function is used to convert saved paths into engine relative paths
	NOTE: masterfile is expected to be *file*, not directory. subfile can be either
	'''
	s = '/'
	
	masterfile = os.path.abspath(masterfile).split(s)[:-1]
	subfile = os.path.abspath( s.join(masterfile) + s + subfile )
	masterfile = os.getcwd() + s + 'foo.bar' # cheat a little to satisfy root_subfile
	return root_subfile(masterfile, subfile)
	
	
	