// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/action.h"
%}

%include "util/base/utilbase.i"
%include "model/metamodel/ivisual.i"

namespace FIFE {

	class Action : public FifeClass {
	public:
		Action(const std::string& identifier);
		virtual ~Action();
		const std::string& getId();
		void adoptVisual(IVisual* visual);
		template<typename T> T* getVisual() const;
		void setDuration(uint32_t duration);
		uint32_t getDuration();
	};
}