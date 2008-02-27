# the following dictionary defines the allowed dependencies checked while testing
# format = module -> allowed dependencies from this module
ALLOWED_MODULE_DEPS = {
	'controller': ('model', 'eventchannel', 'gui', 'video', 'loaders', 'view', 'audio', 'util', 'vfs', 'pathfinder'),
	'model': ('util', 'eventchannel'),
	'eventchannel': ('util',),
	'gui': ('eventchannel', 'util', 'video', 'vfs'),
	'video': ('util',),
	'loaders': ('audio', 'video', 'vfs', 'xml', 'util', 'view', 'model'),
	'view': ('model', 'video', 'util', 'audio', 'eventchannel'),
	'audio': ('util', 'vfs'),
	'util': ['engine'],
	'vfs': ('util',),
	'xml': ('vfs', 'util'),
	'pathfinder': ('util', 'model'),
}
