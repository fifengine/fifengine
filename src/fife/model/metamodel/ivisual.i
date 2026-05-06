// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/ivisual.h"
%}

namespace FIFE {
	class IVisual {
	public:
		virtual ~IVisual();
	private:
		IVisual();
	};
}