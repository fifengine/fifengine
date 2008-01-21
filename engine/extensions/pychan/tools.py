# coding: utf-8

### Functools ###

def applyOnlySuitable(func,**kwargs):
	"""
	This nifty little function takes another function and applies it to a dictionary of
	keyword arguments. If the supplied function does not expect one or more of the
	keyword arguments, these are silently discarded. The result of the application is returned.
	This is useful to pass information to callbacks without enforcing a particular signature.
	"""
	if hasattr(func,'im_func'):
		code = func.im_func.func_code
		varnames = code.co_varnames[1:code.co_argcount]#ditch bound instance
	else:
		code = func.func_code
		varnames = code.co_varnames[0:code.co_argcount]

	#http://docs.python.org/lib/inspect-types.html
	if code.co_flags & 8:
		return func(**kwargs)
	for name,value in kwargs.items():
		if name not in varnames:
			del kwargs[name]
	return func(**kwargs)
