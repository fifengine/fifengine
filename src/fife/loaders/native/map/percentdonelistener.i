// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "loaders/native/map/percentdonelistener.h"
%}

namespace FIFE {
	%feature("director") PercentDoneListener;
	class PercentDoneListener {
	public:
		virtual ~PercentDoneListener();
		virtual void OnEvent(unsigned int percentDone) = 0;
	};
}