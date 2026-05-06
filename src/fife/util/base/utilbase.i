// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/base/fifeclass.h"
%}

// ignore overload operator
%ignore FIFE::Exception::operator=;
%include "util/base/exception.h"
%include "util/resource/resource.i"

namespace FIFE {

	typedef std::size_t fifeid_t;

	class FifeClass{
	public:
		virtual ~FifeClass();
		fifeid_t getFifeId();
	};

	%extend FifeClass {
		bool __eq__(const PyObject *other) { return false; }
		bool __ne__(const PyObject *other) { return true; }
		bool __eq__(FifeClass *other)
		{
			if (!other) return false;
			return $self->getFifeId() == other->getFifeId();
		}
		bool __ne__(FifeClass *other)
		{
			if (!other) return true;
			return $self->getFifeId() != other->getFifeId();
		}
		fifeid_t __hash__() { return $self->getFifeId(); }
	}
}
