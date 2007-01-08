/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "map/elevation.h"
#include "map/grid.h"

#include "mapdump.h"

namespace FIFE {

	MapDump::MapDump(){
		std::string name = "Map Dump";
		std::string extension = ".dm1";

		dump_format = new file_format();
		dump_format->name = name;
		dump_format->extension = extension;
		m_supported_formats.push_back(dump_format);
	}
	
	MapDump::~MapDump(){
	}
	

	
	bool MapDump::writeFile(void* data_structure, std::string& file_name, file_format* format) {
		map::Map* map_structure = reinterpret_cast<map::Map*>(data_structure);
		if (!map_structure) {
			return false;
		}
		std::string output_file_name = file_name;
		output_file_name.append(format->extension);
		m_output_file = new RawWriteText(output_file_name);
		size_t elevation_count = map_structure->getElevationCount();
		for( size_t index = 0; index < elevation_count; index++ ){
			if( !map_structure->getElevation(index) ){
				return false;
			}
			map::Elevation* elevation = map_structure->getElevation(index);
			if( !elevation ){
				return false;
			}
			std::string output_buffer = "Elevation: ";
			m_output_file->write(output_buffer, 0, output_buffer.size());
			m_output_file->rawWrite<int>(index);
			m_output_file->newLine();
			
			output_buffer = "Objects: \n";
			m_output_file->writeLine(output_buffer);
			/*
			MapElevation::type_objects object_list = elevation->getObjects();
			
			MapElevation::type_objects::const_iterator obj_end = object_list.end();
			for( MapElevation::type_objects::iterator i = object_list.begin(); i != obj_end; i++ ){
				Object* local_obj = *i;
				output_buffer = "name: ";
				std::string obj_name = local_obj->getName();
				output_buffer.append(obj_name);
				output_buffer.append("\n");
				m_output_file->writeLine(output_buffer);
				output_buffer = "position: ";
				m_output_file->write(output_buffer, 0, output_buffer.size());
				m_output_file->rawWrite<int>(local_obj->getPosition());	
				m_output_file->newLine();			
			}
			*/
			size_t tile_grid_count = elevation->getNumGrids();

			output_buffer = "Tiles: \n";
			m_output_file->writeLine(output_buffer);			
			for( size_t tile_index = 0; tile_index < tile_grid_count; tile_index++ ){
				map::Grid* local_tile_grid = elevation->getGrid(tile_index);
				output_buffer = "Tile Grid: ";
				m_output_file->write(output_buffer, 0, output_buffer.size());
				m_output_file->rawWrite<size_t>(tile_index);
				m_output_file->newLine();
				output_buffer = "Tile Global Alpha: ";
				int global_alpha = 0;
				global_alpha = local_tile_grid->getGlobalAlpha();

				m_output_file->write(output_buffer, 0, output_buffer.size());
				m_output_file->rawWrite<int>(global_alpha);
				m_output_file->newLine();
			}
		}
		m_output_file->close();
		return true;	
	}
}
