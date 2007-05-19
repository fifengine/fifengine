#!/usr/bin/python
import sys, re, os
from path import path

ROOTDIRNAME = 'engine'

sources = list(path(ROOTDIRNAME).walkfiles('*.cpp'))
reMethodDef = re.compile(r'(\w+::[~\w]+)\(.*?\)\s*{\s*$')
SKIPPED_SOURCES = ['log.cpp', 'vfs/raw', 'renderbackends']
		
for src in sources:
	skipSrc = False
	for s in SKIPPED_SOURCES:
		if str(src).find(os.path.sep + s) != -1:
			skipSrc = True
	if skipSrc:
		print "--- Skipping " + src
		continue	
		
	out = ['#include "util/log.h"\n']; 
	appended = False
	inComment = False
	for line in src.lines():
		if (line.find('/*') != -1):
			if not inComment and line.find('/*') != -1:
				if line.find('*/') == -1:
					inComment = True
				out.append(line)
				continue
			elif inComment:
				if line.find('*/') != -1:
					inComment = False
				out.append(line)
				continue
			elif line.strip().startswith('//'):
				out.append(line)
				continue
			
		out.append(line)
		m = reMethodDef.search(line)
		if m:
			out.append('FLOW_TRACE(%s);\n' % m.group(1))
			appended = True
	if appended:
		print 'writing to ' + src
		src.write_lines(out)

