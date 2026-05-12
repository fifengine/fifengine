// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "pathfinder/routepather/routepather.h"
%}

%include "model/metamodel/ipather.i"

namespace FIFE {
	%feature("notabstract") RoutePather;
	class RoutePather : public IPather {
	public:
		RoutePather();
		virtual ~RoutePather();
		std::string getName() const;
	};
}