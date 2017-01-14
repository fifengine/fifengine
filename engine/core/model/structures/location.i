/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "model/structures/location.h"
%}

namespace FIFE {

	class Map;
	class Layer;
	class NotSet;
	class ModelCoordinate;
	class ExactModelCoordinate;

	// for some reason swig ignores operator= and gives warning. Ignore this for now
	%ignore Location::operator=;

	class Location {
	public:
		Location();
		Location(const Location& loc);
		Location(Layer* layer);
		~Location();
		void reset();
		Location& operator=(const Location& rhs) const;
		inline bool operator==(const Location& loc) const;
		
		Map* getMap() const;
		void setLayer(Layer* layer);
		Layer* getLayer() const;
		
		void setExactLayerCoordinates(const ExactModelCoordinate& coordinates);
		void setLayerCoordinates(const ModelCoordinate& coordinates);
		void setMapCoordinates(const ExactModelCoordinate& coordinates);
		
		ExactModelCoordinate& getExactLayerCoordinatesRef();
		ExactModelCoordinate getExactLayerCoordinates() const;
		ExactModelCoordinate getExactLayerCoordinates(const Layer* layer) const;
		
		ModelCoordinate getLayerCoordinates() const;
		ModelCoordinate getLayerCoordinates(const Layer* layer) const;
		
		ExactModelCoordinate getMapCoordinates() const;
		
		bool isValid() const;
		double getCellOffsetDistance() const;
		
		double getMapDistanceTo(const Location& location) const;
		double getLayerDistanceTo(const Location& location) const;
	};
	std::ostream& operator<<(std::ostream& os, const Location& l);
}

namespace std {
	%template(LocationVector) vector<FIFE::Location>;
}
