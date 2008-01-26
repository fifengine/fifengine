/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
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
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "util/logger.h"

#include "util/fife_math.h"
#include "util/logger.h"
#include "video/fonts/abstractfont.h"
#include "video/image.h"
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "floatingtextrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);


	FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, AbstractFont* font, int position):
		RendererBase(),
		m_renderbackend(renderbackend),
		m_font(font) {
		setPipelinePosition(position);
		setEnabled(true);
	}

	FloatingTextRenderer::~FloatingTextRenderer() {
	}

	void FloatingTextRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		const std::string* saytext = NULL;

		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = *instance_it;
			saytext = instance->getSayText();
			if (saytext) {
				ExactModelCoordinate elevpos = instance->getLocation().getElevationCoordinates();
				ScreenPoint drawpt = cam->toScreenCoordinates(elevpos);
				
				m_font->setColor(25,25,112);
				Image* img = m_font->getAsImage(*saytext);
				Rect r;
				r.x = drawpt.x;
				r.y = drawpt.y;
				r.w = img->getWidth();
				r.h = img->getHeight();
				img->render(r);
			}
		}
	}
}
