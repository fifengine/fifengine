/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

%module fife
%{
#include "model/metamodel/object.h"
%}

%include "util/base/utilbase.i"
%include "model/metamodel/abstractvisual.i"

namespace FIFE {

	class Action;
	class AbstractPather;

	class Object : public ResourceClass {
	public:
		Object(const std::string& identifier, const std::string& name_space, Object* inherited=NULL);
		~Object();

		const std::string& getId() const { return m_id; }
		const std::string& getNamespace() const { return m_namespace; }

		Action* createAction(const std::string& identifier, bool is_default=false);
		Action* getAction(const std::string& identifier) const;
		Action* getDefaultAction() const { return m_defaultaction; }

		void setPather(AbstractPather* pather);
		AbstractPather* getPather() const { return m_pather; }

		Object* getInherited() const { return m_inherited; }
		void adoptVisual(AbstractVisual* visual) { m_visual = visual; }
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }

		void setBlocking(bool blocking) { m_blocking = blocking; }
		bool isBlocking() const;

		void setStatic(bool stat) { m_static = stat; }
		bool isStatic() const;

		bool operator==(const Object& obj) const;
		bool operator!=(const Object& obj) const;

	};
}
