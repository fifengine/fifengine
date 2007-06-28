# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _fife

# This file is compatible with both classic and new-style classes.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class Exception(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Exception, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Exception, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::Exception instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, Exception, 'this', _fife.new_Exception(*args))
        _swig_setattr(self, Exception, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_Exception):
        try:
            if self.thisown: destroy(self)
        except: pass

    def getMessage(*args): return _fife.Exception_getMessage(*args)

class ExceptionPtr(Exception):
    def __init__(self, this):
        _swig_setattr(self, Exception, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Exception, 'thisown', 0)
        self.__class__ = Exception
_fife.Exception_swigregister(ExceptionPtr)

class SDLException(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, SDLException, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, SDLException, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::SDLException instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, SDLException, 'this', _fife.new_SDLException(*args))
        _swig_setattr(self, SDLException, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_SDLException):
        try:
            if self.thisown: destroy(self)
        except: pass


class SDLExceptionPtr(SDLException):
    def __init__(self, this):
        _swig_setattr(self, SDLException, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SDLException, 'thisown', 0)
        self.__class__ = SDLException
_fife.SDLException_swigregister(SDLExceptionPtr)

class NotFound(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NotFound, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NotFound, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::NotFound instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, NotFound, 'this', _fife.new_NotFound(*args))
        _swig_setattr(self, NotFound, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_NotFound):
        try:
            if self.thisown: destroy(self)
        except: pass


class NotFoundPtr(NotFound):
    def __init__(self, this):
        _swig_setattr(self, NotFound, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NotFound, 'thisown', 0)
        self.__class__ = NotFound
_fife.NotFound_swigregister(NotFoundPtr)

class IndexOverflow(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, IndexOverflow, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, IndexOverflow, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::IndexOverflow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, IndexOverflow, 'this', _fife.new_IndexOverflow(*args))
        _swig_setattr(self, IndexOverflow, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_IndexOverflow):
        try:
            if self.thisown: destroy(self)
        except: pass


class IndexOverflowPtr(IndexOverflow):
    def __init__(self, this):
        _swig_setattr(self, IndexOverflow, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, IndexOverflow, 'thisown', 0)
        self.__class__ = IndexOverflow
_fife.IndexOverflow_swigregister(IndexOverflowPtr)

class InvalidFormat(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, InvalidFormat, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, InvalidFormat, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::InvalidFormat instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, InvalidFormat, 'this', _fife.new_InvalidFormat(*args))
        _swig_setattr(self, InvalidFormat, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_InvalidFormat):
        try:
            if self.thisown: destroy(self)
        except: pass


class InvalidFormatPtr(InvalidFormat):
    def __init__(self, this):
        _swig_setattr(self, InvalidFormat, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, InvalidFormat, 'thisown', 0)
        self.__class__ = InvalidFormat
_fife.InvalidFormat_swigregister(InvalidFormatPtr)

class CannotOpenFile(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, CannotOpenFile, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, CannotOpenFile, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::CannotOpenFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, CannotOpenFile, 'this', _fife.new_CannotOpenFile(*args))
        _swig_setattr(self, CannotOpenFile, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_CannotOpenFile):
        try:
            if self.thisown: destroy(self)
        except: pass


class CannotOpenFilePtr(CannotOpenFile):
    def __init__(self, this):
        _swig_setattr(self, CannotOpenFile, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CannotOpenFile, 'thisown', 0)
        self.__class__ = CannotOpenFile
_fife.CannotOpenFile_swigregister(CannotOpenFilePtr)

class OutOfMemory(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, OutOfMemory, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, OutOfMemory, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::OutOfMemory instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, OutOfMemory, 'this', _fife.new_OutOfMemory(*args))
        _swig_setattr(self, OutOfMemory, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_OutOfMemory):
        try:
            if self.thisown: destroy(self)
        except: pass


class OutOfMemoryPtr(OutOfMemory):
    def __init__(self, this):
        _swig_setattr(self, OutOfMemory, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, OutOfMemory, 'thisown', 0)
        self.__class__ = OutOfMemory
_fife.OutOfMemory_swigregister(OutOfMemoryPtr)

class InvalidConversion(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, InvalidConversion, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, InvalidConversion, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::InvalidConversion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, InvalidConversion, 'this', _fife.new_InvalidConversion(*args))
        _swig_setattr(self, InvalidConversion, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_InvalidConversion):
        try:
            if self.thisown: destroy(self)
        except: pass


class InvalidConversionPtr(InvalidConversion):
    def __init__(self, this):
        _swig_setattr(self, InvalidConversion, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, InvalidConversion, 'thisown', 0)
        self.__class__ = InvalidConversion
_fife.InvalidConversion_swigregister(InvalidConversionPtr)

class NotSupported(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NotSupported, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NotSupported, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::NotSupported instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, NotSupported, 'this', _fife.new_NotSupported(*args))
        _swig_setattr(self, NotSupported, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_NotSupported):
        try:
            if self.thisown: destroy(self)
        except: pass


class NotSupportedPtr(NotSupported):
    def __init__(self, this):
        _swig_setattr(self, NotSupported, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NotSupported, 'thisown', 0)
        self.__class__ = NotSupported
_fife.NotSupported_swigregister(NotSupportedPtr)

class NameClash(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NameClash, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NameClash, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::NameClash instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, NameClash, 'this', _fife.new_NameClash(*args))
        _swig_setattr(self, NameClash, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_NameClash):
        try:
            if self.thisown: destroy(self)
        except: pass


class NameClashPtr(NameClash):
    def __init__(self, this):
        _swig_setattr(self, NameClash, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NameClash, 'thisown', 0)
        self.__class__ = NameClash
_fife.NameClash_swigregister(NameClashPtr)

class Duplicate(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Duplicate, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Duplicate, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::Duplicate instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, Duplicate, 'this', _fife.new_Duplicate(*args))
        _swig_setattr(self, Duplicate, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_Duplicate):
        try:
            if self.thisown: destroy(self)
        except: pass


class DuplicatePtr(Duplicate):
    def __init__(self, this):
        _swig_setattr(self, Duplicate, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Duplicate, 'thisown', 0)
        self.__class__ = Duplicate
_fife.Duplicate_swigregister(DuplicatePtr)

class ScriptException(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ScriptException, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ScriptException, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::ScriptException instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, ScriptException, 'this', _fife.new_ScriptException(*args))
        _swig_setattr(self, ScriptException, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_ScriptException):
        try:
            if self.thisown: destroy(self)
        except: pass


class ScriptExceptionPtr(ScriptException):
    def __init__(self, this):
        _swig_setattr(self, ScriptException, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ScriptException, 'thisown', 0)
        self.__class__ = ScriptException
_fife.ScriptException_swigregister(ScriptExceptionPtr)

class EventException(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, EventException, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, EventException, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::EventException instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, EventException, 'this', _fife.new_EventException(*args))
        _swig_setattr(self, EventException, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_EventException):
        try:
            if self.thisown: destroy(self)
        except: pass


class EventExceptionPtr(EventException):
    def __init__(self, this):
        _swig_setattr(self, EventException, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EventException, 'thisown', 0)
        self.__class__ = EventException
_fife.EventException_swigregister(EventExceptionPtr)

class Engine(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Engine, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Engine, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ FIFE::Engine instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, Engine, 'this', _fife.new_Engine(*args))
        _swig_setattr(self, Engine, 'thisown', 1)
    def __del__(self, destroy=_fife.delete_Engine):
        try:
            if self.thisown: destroy(self)
        except: pass


class EnginePtr(Engine):
    def __init__(self, this):
        _swig_setattr(self, Engine, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Engine, 'thisown', 0)
        self.__class__ = Engine
_fife.Engine_swigregister(EnginePtr)



