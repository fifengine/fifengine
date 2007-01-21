import os

def read_wrapped_proc(proc, filename):
	try:
		x = file(filename, 'r')
		return proc(filename, x.read())
	finally:
		x.close()

def write_wrapped_proc(proc, filename, data):
	tmp = proc(data)
	if not tmp: return
	try:
		x = file(filename, 'w')
		x.write(tmp)
	finally:
		x.close()

def file_iter(path, ext, proc):
	# Create the file list
	stuff = []
	subpath = None
	for entry in os.walk(path):
		if '.svn' in entry[0]: continue # skip versioning info folders
		if entry[0] != subpath:
			if subpath != None:
				yield (subpath, stuff)
				stuff = []
			subpath = entry[0]
			
		#print "EXPLORING:", subpath
		for x in entry[2]:
			if x.endswith(ext):
				stuff += [i for i in read_wrapped_proc(proc, os.path.join(subpath, x))]
	yield (subpath, stuff)

def idx(seq, thing):
	try: return seq.index(thing)
	except: return len(seq)

def cleanup(x):
	if ':' in x: x = x.split(':')[0]
	x = x.strip()
	if x.endswith(';'): x = x[:-1]
	x = x.strip()
	return x

def parsefile(name, text):
	import re
	cpp_comment = re.compile("//.*?\\n", re.S)
	c_comment = re.compile("/\*.*?\*/", re.S)
	whitespace = re.compile("[ \t]+")
	template = re.compile("template <[^>]*>")
	#print name, ":"
	level = 0
	stack = []
	for line in whitespace.sub(' ',
			c_comment.sub('', cpp_comment.sub('', text.replace("\\\n", ' ')))
			).split('\n'):
		line = line.strip()
		if not line or line.startswith('#'): continue
		while line:
			openpos = idx(line, '{')
			closepos = idx(line, '}')
			chunk = line[:min(openpos, closepos)].strip()
			if chunk.startswith('template'): chunk = template.sub('', chunk)
				
			if 'class ' in chunk:
				#print "VALID CHUNK:", chunk
				chunk = chunk[chunk.index('class '):]
			name = (chunk + ' ').split(' ', 1)[1]
			if chunk.startswith('class '):
				yield [cleanup(x) for x in stack + ['class ' + name]]
			if openpos < closepos:
				tmp = line[:openpos].strip()
				line = line[openpos+1:]
				level += 1
				tag = ''
				for x in ('namespace', 'class'):
					if chunk.startswith(x):
						tag = x
				if tag:
					stack.append(tag + ' ' + name)
				else:
					stack.append('')
			elif openpos > closepos:
				result = level, line[:closepos].strip()
				line = line[closepos+1:]
				level -= 1
				stack.pop()
			else:
				result = level, line.strip()
				line = ''

class sorter:
	def __init__(self):
		self.current = {}
		self.sub = {}

	def __call__(self, paths):
		#print "CALL SORTER WITH:", paths
		for x in paths:
			self._add(x)
		return self

	def _add(self, apath):
		while apath and not apath[0]: apath = apath[1:]
		if not apath: return
		key = apath[0]
		if len(apath) == 1: 
			self.current[key] = None
		else:
			if key.startswith('class'): return
			# nested-class fwd declarations are disabled for now.
			value = apath[1:]
			if not self.sub.has_key(key):
				self.sub[key] = sorter()
			self.sub[key]._add(value)

	def __str__(self):
		return self._format('')

	def _format(self, indent):
		result = ''.join([indent + x + ';\n' for x in self.current])
		for key in self.sub:
			result += indent + key + ' {\n' + self.sub[key]._format(indent + '\t') + indent + '}\n'
		return result

def main():
	for x in file_iter('.', '.h', parsefile):
		print "DIR:", x[0]
		write_wrapped_proc(lambda d: str(sorter()(d)), 
		                   os.path.join(x[0], 'fwd.h'), x[1])

if __name__ == '__main__':
	main()
