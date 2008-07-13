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

		const std::string& getId();
		const std::string& getNamespace();

		Action* createAction(const std::string& identifier, bool is_default=false);
		Action* getAction(const std::string& identifier);
		Action* getDefaultAction();

		void setPather(AbstractPather* pather);
		AbstractPather* getPather();

		Object* getInherited();
		void adoptVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
		
		void setBlocking(bool blocking);
		bool isBlocking();
		
		void setStatic(bool stat);
		bool isStatic();
	};

	class ObjectLoader : public ResourceLoader {
	public:
		Object* load(const ResourceLocation& location); 
		Object* load(const std::string& filename); 
	};
}
