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

#include "view/visual.h"
#include "view/camera.h"
#include "floatingtextrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);


	FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, int position, AbstractFont* font):
		RendererBase(renderbackend, position),
		m_font(font) {
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
				InstanceVisual* visual = instance->getVisual<InstanceVisual>();
				const Rect& ir = visual->getCachedImageDimensions();
				m_font->setColor(25,25,112);
				Image* img = m_font->getAsImageMultiline(*saytext);
				Rect r;
				r.x = (ir.x + ir.w/2) - img->getWidth()/2; /// the center of the text rect is always aligned to the instance's rect center.
				r.y = ir.y- img->getHeight(); /// make the text rect floating higher than the instance.
				r.w = img->getWidth();
				r.h = img->getHeight();
				img->render(r);
			}
		}
	}
}
