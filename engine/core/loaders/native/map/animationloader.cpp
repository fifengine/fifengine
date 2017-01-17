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

// Standard C++ library includes

// 3rd party library includes
#include <tinyxml.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/fife_boost_filesystem.h"
#include "vfs/vfs.h"
#include "vfs/raw/rawdata.h"
#include "video/imagemanager.h"
#include "video/animationmanager.h"
#include "video/image.h"
#include "video/animation.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "animationloader.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_NATIVE_LOADERS);

	AnimationLoader::AnimationLoader(VFS* vfs, ImageManager* imageManager, AnimationManager* animationManager)
	: m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager) {

	}

	bool AnimationLoader::isLoadable(const std::string& filename) {
		bfs::path animPath(filename);

		std::string animationFilename = animPath.string();
		TiXmlDocument animFile;

		try {
			RawData* data = m_vfs->open(animationFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					// TODO - this could be expanded to do more checks
					animFile.Parse(data->readString(data->getDataLength()).c_str());

					if (animFile.Error()) {
						return false;
					}
				}

				// done with data delete resource
				delete data;
				data = 0;
			}
		}
		catch (NotFound&) {
			return false;
		}
		
		// if we get here then loading the file went well
		TiXmlElement* root = animFile.RootElement();

		if (root && root->ValueStr() == "assets") {
			if (root->FirstChildElement("animation")) {
				return true;
			}
		}

		return false;
	}

	AnimationPtr AnimationLoader::load(const std::string& filename) {
		bfs::path animPath(filename);

		std::string animationFilename = animPath.string();

		TiXmlDocument doc;

		AnimationPtr animation;

		try {
			RawData* data = m_vfs->open(animationFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					doc.Parse(data->readString(data->getDataLength()).c_str());

					if (doc.Error()) {
						return animation;
					}

					// done with data delete resource
					delete data;
					data = 0;
				}
			}
		}
		catch (NotFound& e) {
			FL_ERR(_log, e.what());

			// TODO - should we abort here
			//        or rethrow the exception
			//        or just keep going

			return animation;
		}

		// if we get here then everything loaded properly
		// so we can just parse out the contents
		TiXmlElement* root = doc.RootElement();

		if (root && root->ValueStr() == "assets") {
			animation = loadAnimation(filename, root->FirstChildElement("animation"));
		}

		return animation;
	}

	std::vector<AnimationPtr> AnimationLoader::loadMultiple(const std::string& filename) {
		bfs::path animPath(filename);

		std::string animationFilename = animPath.string();

		TiXmlDocument doc;

		std::vector<AnimationPtr> animationVector;

		try {
			RawData* data = m_vfs->open(animationFilename);

			if (data) {
				if (data->getDataLength() != 0) {
					doc.Parse(data->readString(data->getDataLength()).c_str());

					if (doc.Error()) {
						return animationVector;
					}

					// done with data delete resource
					delete data;
					data = 0;
				}
			}
		}
		catch (NotFound& e) {
			FL_ERR(_log, e.what());

			// TODO - should we abort here
			//        or rethrow the exception
			//        or just keep going

			return animationVector;
		}

		// if we get here then everything loaded properly
		// so we can just parse out the contents
		TiXmlElement* root = doc.RootElement();

		if (root && root->ValueStr() == "assets") {
			for (TiXmlElement* animationElem = root->FirstChildElement("animation"); animationElem; animationElem = animationElem->NextSiblingElement("animation")) {
				AnimationPtr animation = loadAnimation(filename, animationElem);
				if (animation) {
					animationVector.push_back(animation);
				}
			}
		}

		return animationVector;
	}

	AnimationPtr AnimationLoader::loadAnimation(const std::string& filename, TiXmlElement* animationElem) {
		AnimationPtr animation;
		if (!animationElem) {
			return animation;
		}

		bfs::path animPath(filename);
		std::string animationFilename = animPath.string();

		bool alreadyLoaded = false;
		// first try to use the id, if no id exists it use the filename as fallback
		const std::string* animationId = animationElem->Attribute(std::string("id"));
		if (animationId) {
			if (!m_animationManager->exists(*animationId)) {
				animation = m_animationManager->create(*animationId);
			} else {
				animation = m_animationManager->getPtr(*animationId);
				alreadyLoaded = animation->getFrameCount() != 0;
			}
		} else {
			if (HasParentPath(animPath)) {
				animPath= GetParentPath(animPath) / animationFilename;
			} else {
				animPath = bfs::path(animationFilename);
			}
			if (!m_animationManager->exists(animPath.string())) {
				animation = m_animationManager->create(animPath.string());
			} else {
				animation = m_animationManager->getPtr(animPath.string());
				alreadyLoaded = animation->getFrameCount() != 0;
			}
		}

		if (alreadyLoaded) {
			return animation;
		}

		int direction = 0;
		int actionFrame = -1;
		int animDelay = 0;
		int animXoffset = 0;
		int animYoffset = 0;

		int success = animationElem->QueryValueAttribute("direction", &direction);
		if (success == TIXML_SUCCESS) {
			animation->setDirection(direction);
		}
		success = animationElem->QueryValueAttribute("action_frame", &actionFrame);
		if (success == TIXML_SUCCESS) {
			animation->setActionFrame(actionFrame);
		}
		animationElem->QueryValueAttribute("delay", &animDelay);
		animationElem->QueryValueAttribute("x_offset", &animXoffset);
		animationElem->QueryValueAttribute("y_offset", &animYoffset);

		for (TiXmlElement* frameElement = animationElem->FirstChildElement("frame"); frameElement; frameElement = frameElement->NextSiblingElement("frame")) {
			const std::string* sourceId = frameElement->Attribute(std::string("source"));
			if (sourceId) {
				bfs::path framePath(filename);

				if (HasParentPath(framePath)) {
					framePath = GetParentPath(framePath) / *sourceId;
					if (!bfs::exists(framePath)) {
						framePath = bfs::path(*sourceId);
					}
				} else {
					framePath = bfs::path(*sourceId);
				}

				ImagePtr imagePtr;
				if (!m_imageManager->exists(framePath.string())) {
					imagePtr = m_imageManager->create(framePath.string());
				} else {
					imagePtr = m_imageManager->getPtr(framePath.string());
				}

				if (imagePtr) {
					int frameXoffset = 0;
					success = frameElement->QueryValueAttribute("x_offset", &frameXoffset);
					if (success == TIXML_SUCCESS) {
						imagePtr->setXShift(frameXoffset);
					} else {
						imagePtr->setXShift(animXoffset);
					}

					int frameYoffset = 0;
					success = frameElement->QueryValueAttribute("y_offset", &frameYoffset);
					if (success == TIXML_SUCCESS) {
						imagePtr->setYShift(frameYoffset);
					} else {
						imagePtr->setYShift(animYoffset);
					}

					int frameDelay = 0;
					success = frameElement->QueryValueAttribute("delay", &frameDelay);
					if (success == TIXML_SUCCESS) {
						animation->addFrame(imagePtr, frameDelay);
					} else {
						animation->addFrame(imagePtr, animDelay);
					}
				}
			}
		}

		return animation;
	}
}
