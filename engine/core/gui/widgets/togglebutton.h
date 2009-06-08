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

#ifndef FIFE_GUICHAN_ADDON_TOGGLEBUTTON_H
#define FIFE_GUICHAN_ADDON_TOGGLEBUTTON_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes
#include <guichan.hpp>
#include <guichan/actionlistener.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace gcn {
	/**
     * An implementation of a toggleable button.
	 *
	 * If the button is in a group, all other buttons in that group will be untoggled
	 * when a button gets toggled. If the button is already toggled, you can untoggle 
	 * it by clicking on it.
	 *
     */
	class ToggleButton : public Button, public ActionListener {
		public:
			/**
			 * Constructor
			 *
			 * @param up_image Image displayed when the button isn't toggled
			 * @param down_image Image displayed when the button is toggled
			 * @param hover_file Image displayed when the mouse cursor is over button
			 * @param caption Text to be displayed on button
			 * @param group The group the button belongs to
			 */
			ToggleButton(Image *up_image = 0, Image *down_image = 0, Image *hover_image = 0, const std::string& caption = "", const std::string& group = "");

			/**
			 * Destructor
			 */
			~ToggleButton();

			/**
			 * Draws the button
			 */
			void draw(Graphics *graphics);

			/**
			 * Adjust size to fit image and caption
			 */
			void adjustSize();

			/**
			 * Sets the image that will be displayed when the button isn't toggled
			 *
			 * @param image Image to be displayed
			 */
			void setUpImage(Image* image);

			/**
			 * Sets the image that will be displayed when the button is toggled or pressed
			 *
			 * @param image Image to be displayed
			 */
			void setDownImage(Image* image);

			/**
			 * Sets the image which will be displayed when the mouse cursor is over the button
			 *
			 * @param image Image to be displayed
			 */
			void setHoverImage(Image* image);
			
			/**
			 * Sets the number of pixels the image or text will be offset from
			 * the top left corner of button when the button is pressed or toggled.
			 *
			 * @param x Offset from left
			 * @param y Offset from top
			 * @see getDownXOffset
			 * @see getDownYOffset
			 */
			void setDownOffset(int x, int y);

			/**
			 * Gets the number of pixels the image or text will be offset
			 * from the left of button when the button is pressed or toggled.
			 *
			 * @return Offset from left when button is pressed
			 * @see setDownOffset
			 */
			int getDownXOffset() const;

			/**
			 * Gets the number of pixels the image or text will be offset
			 * from the top of button when the button is pressed or toggled.
			 *
			 * @return Offset from top when button is pressed
			 * @see setDownOffset
			 */
			int getDownYOffset() const;

			/**
			 * Checks if the radio button is selected.
			 *
			 * @return True if the radio button is selecte, false otherwise.
			 * @see setSelected
			 */
			bool isToggled() const;

			/**
			 * Sets the radio button to selected or not.
			 *
			 * @param selected True if the radio button should be selected,
			 *                 false otherwise.
			 * @see isSelected
			 */
			void setToggled(bool toggled);

			// From Guichan 0.8.1
			/**
			 * Sets the group the toggle button should belong to. Note that
			 * a toggle button group is unique per application, not per Gui object
			 * as the group is stored in a static map.
			 *
			 * @param group The name of the group.
			 * @see getGroup
			 */
			void setGroup(const std::string &group);

			/**
			 * Gets the group the toggle button belongs to.
			 *
			 * @return The group the toggle button belongs to.
			 * @see setGroup
			 */
			const std::string &getGroup() const;

		protected:
			// Inherited from gcn::Widget
			/**
			 * Toggle button when it is activated
			 *
			 * @param actionEvent ActionEvent object
			 */
			void action(const ActionEvent& actionEvent);

		private:
			// Image to be used when the button is not toggle
			Image *m_upImage;

			// Image to be used when the button is toggled or pressed
			Image *m_downImage;

			// Image to be used when the mouse cursor is over the image
			Image *m_hoverImage;

			// Number of pixels the image/text will be offset from the top left
			// corner, when the button is pressed or toggled
			int x_downoffset;
			int y_downoffset;

			/**
			 * Whether the button is toggled or not.
			 */
			bool m_toggled;			

			//-- From Guichan 0.8.1 --
			/**
			 * Holds the group of the toggle button.
			 */
			std::string m_group;

			/**
			 * Typedef.
			 */
			typedef std::multimap<std::string, ToggleButton *> GroupMap;

			/**
			 * Typedef.
			 */
			typedef GroupMap::iterator GroupIterator;

			/**
			 * Holds all available toggle button groups.
			 */
			static GroupMap m_groupMap;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
