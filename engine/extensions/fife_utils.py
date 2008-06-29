import fife, re

__all__ = ['is_fife_exc']

_exc_re = re.compile(r'_\[(\w+)\]_')

def is_fife_exc(type, original_exc):
	ret = False
	m = _exc_re.search(str(original_exc))
	if m:
		if m.group(1) == type('').getTypeStr():
			ret = True
	return ret


