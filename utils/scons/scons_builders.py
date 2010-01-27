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


import os, sys

#**************************************************************************
#project file generators (scon builders)
#**************************************************************************

_sep = os.path.sep
msvcbuildpath = os.path.join('build', 'win32', 'build_environments', 'visual_studio_8')
msvcbuildpath9 = os.path.join('build', 'win32', 'build_environments', 'visual_studio_9')
cbbuildpath_win32 = os.path.join('build', 'win32', 'build_environments', 'code_blocks')
cbbuildpath_linux = os.path.join('build', 'linux', 'code_blocks')


def generate_msvc_project(target, source, env):
	def create_dict_tree(source_dict):
		for f in source_dict.keys():
			parts = f.split(os.path.sep, 1)
			if len(parts) > 1:
				try:
					source_dict[parts[0]][parts[1]] = {}
				except KeyError:
					source_dict[parts[0]] = {}
					source_dict[parts[0]][parts[1]] = {}
				del source_dict[f]
		for k, d in source_dict.items():
			create_dict_tree(d)
		return source_dict

	def get_msvc_repr(d, tabcount=2, curpath=''):
		retstr = []
		for k in sorted(d.keys()):
			newpath = os.path.join(curpath, k)
			if len(d[k].keys()):
				retstr.append(tabcount * '\t' + '<Filter Name="%s">' % k)
				retstr.append(get_msvc_repr(d[k], tabcount+1, newpath))
				retstr.append(tabcount * '\t' + '</Filter>')
			else:
				newpath = os.path.join('..', '..', '..', '..', 'engine', newpath)
				retstr.append(tabcount * '\t' + '<File RelativePath="%s"></File>' % newpath.replace('/','\\'))
		return '\n'.join(retstr)

	vcpaths = [os.path.abspath(str(f)).rsplit('%sengine%s' % (_sep, _sep))[-1] for f in source]
	vcpaths.append(os.path.join('swigwrappers', 'python', 'fife_wrap.cc'))
	xmlstr = get_msvc_repr(create_dict_tree(dict([[p, {}] for p in vcpaths])))
	projtxt = open(os.path.join(msvcbuildpath, 'engine_template.xml'), 'r').read()
	projtxt = projtxt.replace('__FILE_INSERTION_POINT__', xmlstr)

	oldprojtxt = ''
	try:
		oldprojtxt = open(str(target[0]), 'r').read()
	except:
		pass

	if(oldprojtxt <> projtxt):
		open(str(target[0]), 'w').write(projtxt)
		print "FIFE msvc project file succesfully created (%s)" % os.path.abspath(str(target[0]))
	else:
		print "FIFE msvc project file already up-to-date (%s)" % os.path.abspath(str(target[0]))


def generate_msvc_project9(target, source, env):
	def create_dict_tree(source_dict):
		for f in source_dict.keys():
			parts = f.split(os.path.sep, 1)
			if len(parts) > 1:
				try:
					source_dict[parts[0]][parts[1]] = {}
				except KeyError:
					source_dict[parts[0]] = {}
					source_dict[parts[0]][parts[1]] = {}
				del source_dict[f]
		for k, d in source_dict.items():
			create_dict_tree(d)
		return source_dict

	def get_msvc_repr(d, tabcount=2, curpath=''):
		retstr = []
		for k in sorted(d.keys()):
			newpath = os.path.join(curpath, k)
			if len(d[k].keys()):
				retstr.append(tabcount * '\t' + '<Filter Name="%s">' % k)
				retstr.append(get_msvc_repr(d[k], tabcount+1, newpath))
				retstr.append(tabcount * '\t' + '</Filter>')
			else:
				newpath = os.path.join('..', '..', '..', '..', 'engine', newpath)
				retstr.append(tabcount * '\t' + '<File RelativePath="%s"></File>' % newpath.replace('/','\\'))
		return '\n'.join(retstr)

	vcpaths = [os.path.abspath(str(f)).rsplit('%sengine%s' % (_sep, _sep))[-1] for f in source]
	vcpaths.append(os.path.join('swigwrappers', 'python', 'fife_wrap.cc'))
	xmlstr = get_msvc_repr(create_dict_tree(dict([[p, {}] for p in vcpaths])))
	projtxt = open(os.path.join(msvcbuildpath9, 'engine_template.xml'), 'r').read()
	projtxt = projtxt.replace('__FILE_INSERTION_POINT__', xmlstr)

	oldprojtxt = ''
	try:
		oldprojtxt = open(str(target[0]), 'r').read()
	except:
		pass

	if(oldprojtxt <> projtxt):
		open(str(target[0]), 'w').write(projtxt)
		print "FIFE msvc9 project file succesfully created (%s)" % os.path.abspath(str(target[0]))
	else:
		print "FIFE msvc9 project file already up-to-date (%s)" % os.path.abspath(str(target[0]))

def generate_codeblocks_project_win32(target, source, env):
	codeblocksHeaderDef = \
	'''		<Unit filename="..\..\..\engine\%s">
				<Option compilerVar=""/>
				<Option compile="0"/>
				<Option link="0"/>
				<Option target="default"/>
			</Unit>'''

	codeblocksCppDef = \
	'''		<Unit filename="..\..\..\..\engine\%s">
				<Option compilerVar="CPP"/>
				<Option target="default"/>
			</Unit>'''
	xmlstr = []
	for f in source:
		newf = os.path.abspath(str(f)).rsplit('%sengine%s' % (_sep, _sep))[-1]
		newf = newf.replace('/', '\\')
		if str(f).endswith('.hpp') or str(f).endswith('.h'):
			xmlstr.append(codeblocksHeaderDef % newf)
		else:
			xmlstr.append(codeblocksCppDef % newf)
	
	xmlstr.append(codeblocksCppDef % os.path.join('swigwrappers', 'python', 'fife_wrap.cc'))

	projtxt = open(os.path.join(cbbuildpath_win32, 'engine_template.xml'), 'r').read()
	projtxt = projtxt.replace('__FILE_INSERTION_POINT__', '\n'.join(xmlstr))
	open(str(target[0]), 'w').write(projtxt)
	print "FIFE code::blocks project file succesfully created (%s)" % os.path.abspath(str(target[0]))

def generate_codeblocks_project_linux(target, source, env):
	codeblocksHeaderDef = \
	'''		<Unit filename="../../../engine/%s">
				<Option compilerVar=""/>
				<Option compile="0"/>
				<Option link="0"/>
				<Option target="default"/>
			</Unit>'''

	codeblocksCppDef = \
	'''		<Unit filename="../../../engine/%s">
				<Option compilerVar="CPP"/>
				<Option target="default"/>
			</Unit>'''
	xmlstr = []
	for f in source:
		newf = os.path.abspath(str(f)).rsplit('%sengine%s' % (_sep, _sep))[-1]
		newf = newf.replace('/', '\\')
		if str(f).endswith('.hpp') or str(f).endswith('.h'):
			xmlstr.append(codeblocksHeaderDef % newf)
		else:
			xmlstr.append(codeblocksCppDef % newf)
			
	xmlstr.append(codeblocksCppDef % os.path.join('swigwrappers', 'python', 'fife_wrap.cc'))

	projtxt = open(os.path.join(cbbuildpath_linux, 'engine_template.xml'), 'r').read()
	projtxt = projtxt.replace('__FILE_INSERTION_POINT__', '\n'.join(xmlstr))
	open(str(target[0]), 'w').write(projtxt)
	print "FIFE code::blocks project file succesfully created (%s)" % os.path.abspath(str(target[0]))

