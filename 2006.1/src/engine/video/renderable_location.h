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

#ifndef RENDERABLE_LOCATION_H
#define RENDERABLE_LOCATION_H
#include <string>

namespace FIFE {

	/**	Contains information about the Location of a RenderAble.
	 *  This Class is used to give RenderableProvider, the information
	 * 	where to find the data. It is basically here to make the
	 *	code in the ImageCache more readable.
	 *	Normally you could use it like this:
	 *	@code
	 *  RenderableLocation location(RenderableLocation::RL_FILE, "somepicture.png");
	 *  ImageID = imageCachePtr->addImageFromLocation( location );
	 *  @endcode
	 *  This is, however, equivalent to writing:
	 *  @code
	 *  ImageID = imageCachePtr->addImageFromFile( "somepicture.jpg" );
	 *  @endcode
	 *
	 *	@note: Currently only Files are implemented. Other stuff will follow.
	 *  @warning: getFileExtension() has to be written.
	 *
	 */
    class RenderableLocation {

	public:
		enum LocationTypes {
			RL_UNDEFINED,
			RL_FILE,
			RL_FILE_FRAME,
			RL_FILE_FRAME_DIR
			// Next Implementation to come: RL_TILESET,
			// After that: RL_WHAT_EVER_YOU_ADD
		};

		/** Constructor
		 */
		RenderableLocation( enum LocationTypes type = RL_UNDEFINED, const std::string& loc = "");

		/** Create RL_FILE from a single-image file.
		 *
		 * @param Path to image in VFS
		 */
		RenderableLocation(const std::string & filename);
		
		/** Create RL_FILE_FRAME from a simple animation.
		 *
		 * @param Path to image in VFS
		 * @param Index for requested frame inside file
		 */
		RenderableLocation(const std::string& loc, unsigned int frame_idx);
		
		/** Create RL_FILE_FRAME_DIR from a animation.
		 *
		 * @param Path to image in VFS
		 * @param Index for requested frame inside file
		 * @param Index for requested direction/heading
		 */
		RenderableLocation(const std::string& loc, unsigned int frame_idx, unsigned int dir_idx);

		/** Copy Constructor
		 */
		RenderableLocation( const RenderableLocation& loc );

		/** Destructor
		 */
		~RenderableLocation();

		// Used if isFile() is true:
		/** Check whether the RenderableLocation is a file
		 */
		bool isFile() const { return m_type >= RL_FILE; };
		/** Return the filename
		 */
		std::string toFilename() const { return m_string; };
		/** Return exstension
		 */
		std::string getFileExtension() const;

		/** Return a string representation
		 * For Error Reporting
		 */
		std::string toString() const;

		LocationTypes getType() const { return m_type; }
		unsigned int  getFrame() const { return m_frame_idx; }
		unsigned int  getDirection() const { return m_direction_idx; }

		/** Check whether the RenderableLocation is valid
		 * @bug This should really be bool isValid()
		 */
		operator bool() const;

		/** Compare two RenderableLocations for equality
		 */
		bool operator ==(const RenderableLocation& loc) const;
		/** Compare two RenderableLocations
		 * This is needed as the locations should be stored in a \c std::map
		 */
		bool operator <(const RenderableLocation& loc) const;

	private:
		// This is an implementation detail.
		// If direct vars hit the cache size too much,
		// we'll be using implicit sharing.
		enum LocationTypes m_type;
		std::string m_string;
		unsigned int m_frame_idx;
		unsigned int m_direction_idx;
	};

}; //FIFE
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
