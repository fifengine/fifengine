// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/actionaudio.h"
#include "model/metamodel/action.h"
#include "model/metamodel/object.h"
#include "model/model.h"
#include "util/log/logger.h"
#include "util/xml/xmlhelper.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/animationmanager.h"
#include "video/imagemanager.h"
#include "view/visual.h"

#include "animationloader.h"
#include "atlasloader.h"
#include "objectloader.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_NATIVE_LOADERS);

    ObjectLoader::ObjectLoader(
        Model* model,
        VFS* vfs,
        ImageManager* imageManager,
        AnimationManager* animationManager,
        const AnimationLoaderPtr& animationLoader,
        const AtlasLoaderPtr& atlasLoader) :
        m_model(model), m_vfs(vfs), m_imageManager(imageManager), m_animationManager(animationManager)
    {
        assert(m_model && m_vfs && m_imageManager && m_animationManager);

        if (animationLoader) {
            m_animationLoader = animationLoader;
        } else {
            m_animationLoader.reset(new AnimationLoader(m_vfs, m_imageManager, m_animationManager));
        }

        if (atlasLoader) {
            m_atlasLoader = atlasLoader;
        } else {
            m_atlasLoader.reset(new AtlasLoader(m_model, m_vfs, m_imageManager, m_animationManager));
        }
    }

    ObjectLoader::~ObjectLoader() { }

    void ObjectLoader::setAnimationLoader(const AnimationLoaderPtr& animationLoader)
    {
        assert(animationLoader);

        m_animationLoader = animationLoader;
    }

    AnimationLoaderPtr ObjectLoader::getAnimationLoader()
    {
        return m_animationLoader;
    }

    void ObjectLoader::setAtlasLoader(const AtlasLoaderPtr& atlasLoader)
    {
        assert(atlasLoader);

        m_atlasLoader = atlasLoader;
    }

    AtlasLoaderPtr ObjectLoader::getAtlasLoader()
    {
        return m_atlasLoader;
    }

    bool ObjectLoader::isLoadable(const std::string& filename) const
    {
        fs::path objectPath(filename);

        XML::Document objectFile;

        try {
            RawData* data = m_vfs->open(objectPath.string());

            if (data) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(objectFile, xml)) {
                        std::ostringstream oss;
                        oss << " Failed to load" << objectPath.string() << " : " << __FILE__ << " [" << __LINE__ << "]"
                            << std::endl;
                        FL_ERR(_log, oss.str());

                        delete data;
                        return false;
                    }
                } else {
                    std::ostringstream oss;
                    oss << " Failed to load" << objectPath.string() << " : " << __FILE__ << " [" << __LINE__ << "]"
                        << std::endl;
                    FL_ERR(_log, oss.str());

                    return false;
                }

                // done with data delete resource
                delete data;
                data = 0;
            } else {
                std::ostringstream oss;
                oss << " Failed to load" << objectPath.string() << " : " << __FILE__ << " [" << __LINE__ << "]"
                    << std::endl;
                FL_ERR(_log, oss.str());

                return false;
            }
        } catch (NotFound&) {
            std::ostringstream oss;
            oss << " Failed to load" << objectPath.string() << " : " << __FILE__ << " [" << __LINE__ << "]"
                << std::endl;
            FL_ERR(_log, oss.str());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return false;
        }

        // if we get here then loading the file went well
        XML::Element* root = objectFile.RootElement();

        if (XML::HasName(root, "assets")) {
            if (root->FirstChildElement("object")) {
                return true;
            }
        }

        return false;
    }

    void ObjectLoader::load(const std::string& filename)
    {
        fs::path objectPath(filename);

        XML::Document objectFile;

        try {
            RawData* data = m_vfs->open(objectPath.string());

            if (data) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(objectFile, xml)) {
                        delete data;
                        return;
                    }
                }

                // done with data delete resource
                delete data;
                data = 0;
            }
        } catch (NotFound&) {
            std::ostringstream oss;
            oss << " Failed to load" << objectPath.string() << " : " << __FILE__ << " [" << __LINE__ << "]"
                << std::endl;
            FL_ERR(_log, oss.str());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return;
        }
        std::string objectDirectory = "";
        if (HasParentPath(objectPath)) {
            objectDirectory = GetParentPath(objectPath).string();
        }

        // if we get here then loading the file went well
        XML::Element* root = objectFile.RootElement();
        if (root) {
            for (const XML::Element* importElement = root->FirstChildElement("import"); importElement;
                 importElement                     = importElement->NextSiblingElement("import")) {
                const char* importDir  = XML::Attribute(importElement, "dir");
                const char* importFile = XML::Attribute(importElement, "file");

                std::string directory = "";
                if (importDir) {
                    directory = importDir;
                }

                std::string file = "";
                if (importFile) {
                    file = importFile;
                }

                if (importDir && !importFile) {
                    fs::path fullPath(objectDirectory);
                    fullPath /= directory;
                    loadImportDirectory(fullPath.string());
                } else if (importFile) {
                    fs::path fullFilePath(file);
                    fs::path fullDirPath(directory);
                    if (importDir) {
                        fullDirPath = fs::path(objectDirectory);
                        fullDirPath /= directory;
                    } else {
                        fullFilePath = fs::path(objectDirectory);
                        fullFilePath /= file;
                    }
                    loadImportFile(fullFilePath.string(), fullDirPath.string());
                }
            }
        }

        if (XML::HasName(root, "assets")) {
            for (XML::Element* objectElem = root->FirstChildElement("object"); objectElem;
                 objectElem               = objectElem->NextSiblingElement("object")) {
                const char* objectId    = XML::Attribute(objectElem, "id");
                const char* namespaceId = XML::Attribute(objectElem, "namespace");

                Object* obj = NULL;
                if (objectId && namespaceId) {
                    const char* parentId = XML::Attribute(objectElem, "parent");

                    if (parentId) {
                        Object* parent = m_model->getObject(parentId, namespaceId);
                        if (parent) {
                            try {
                                obj = m_model->createObject(objectId, namespaceId, parent);
                            } catch (NameClash&) {
                                // TODO - handle exception
                                assert(false);
                            }
                        }
                    } else {
                        // this will make sure the object has not already been loaded
                        if (m_model->getObject(objectId, namespaceId) == NULL) {
                            try {
                                obj = m_model->createObject(objectId, namespaceId);
                            } catch (NameClash& e) {
                                FL_ERR(_log, e.what());

                                // TODO - handle exception
                                assert(false);
                            }
                        }
                    }
                }

                if (obj) {
                    obj->setFilename(objectPath.string());
                    ObjectVisual::create(obj);

                    int isBlocking = 0;
                    objectElem->QueryIntAttribute("blocking", &isBlocking);
                    obj->setBlocking(isBlocking != 0);

                    int isStatic = 0;
                    objectElem->QueryIntAttribute("static", &isStatic);
                    obj->setStatic(isStatic != 0);

                    const char* pather = XML::Attribute(objectElem, "pather");

                    if (pather) {
                        obj->setPather(m_model->getPather(pather));
                    } else {
                        obj->setPather(m_model->getPather("RoutePather"));
                    }

                    const char* costId = XML::Attribute(objectElem, "cost_id");
                    if (costId) {
                        obj->setCostId(costId);
                        double cost = 1.0;
                        int success = objectElem->QueryDoubleAttribute("cost", &cost);
                        if (success == XML::SUCCESS) {
                            obj->setCost(cost);
                        }
                    }

                    const char* areaId = XML::Attribute(objectElem, "area_id");
                    if (areaId) {
                        obj->setArea(areaId);
                    }

                    double speed = 1.0;
                    int success  = root->QueryDoubleAttribute("speed", &speed);
                    if (success == XML::SUCCESS) {
                        obj->setSpeed(speed);
                    }

                    // loop over all walkable areas
                    for (XML::Element* walkableElement = objectElem->FirstChildElement("walkable_area");
                         walkableElement;
                         walkableElement = walkableElement->NextSiblingElement("walkable_area")) {
                        const char* walkableId = XML::Attribute(walkableElement, "id");
                        if (walkableId) {
                            obj->addWalkableArea(walkableId);
                        }
                    }

                    int cellStack = 0;
                    objectElem->QueryIntAttribute("cellstack", &cellStack);
                    obj->setCellStackPosition(cellStack);

                    double ax = 0;
                    double ay = 0;
                    double az = 0;

                    int xRetVal = XML::QueryAttribute(objectElem, "anchor_x", &ax);
                    int yRetVal = XML::QueryAttribute(objectElem, "anchor_y", &ay);
                    if (xRetVal == XML::SUCCESS && yRetVal == XML::SUCCESS) {
                        obj->setRotationAnchor(ExactModelCoordinate(ax, ay, az));
                    }

                    int isRestrictedRotation = 0;
                    objectElem->QueryIntAttribute("restricted_rotation", &isRestrictedRotation);
                    obj->setRestrictedRotation(isRestrictedRotation != 0);

                    int zStep   = 0;
                    int zRetVal = objectElem->QueryIntAttribute("z_step_limit", &zStep);
                    if (zRetVal == XML::SUCCESS) {
                        obj->setZStepRange(zStep);
                    }

                    // loop over all multi parts
                    for (XML::Element* multiElement = objectElem->FirstChildElement("multipart"); multiElement;
                         multiElement               = multiElement->NextSiblingElement("multipart")) {
                        const char* partId = XML::Attribute(multiElement, "id");
                        if (partId) {
                            obj->addMultiPartId(partId);
                        }
                        for (XML::Element* multiRotation = multiElement->FirstChildElement("rotation"); multiRotation;
                             multiRotation               = multiRotation->NextSiblingElement("rotation")) {
                            int rotation = 0;
                            multiRotation->QueryIntAttribute("rot", &rotation);
                            // relative coordinates which are used to position the object
                            for (XML::Element* multiCoordinate = multiRotation->FirstChildElement("occupied_coord");
                                 multiCoordinate;
                                 multiCoordinate = multiCoordinate->NextSiblingElement("occupied_coord")) {
                                int x   = 0;
                                int y   = 0;
                                xRetVal = XML::QueryAttribute(multiCoordinate, "x", &x);
                                yRetVal = XML::QueryAttribute(multiCoordinate, "y", &y);
                                if (xRetVal == XML::SUCCESS && yRetVal == XML::SUCCESS) {
                                    int z = 0;
                                    multiCoordinate->QueryIntAttribute("z", &z);
                                    obj->addMultiPartCoordinate(rotation, ModelCoordinate(x, y, z));
                                }
                            }
                        }
                    }

                    // loop over all image tags
                    for (XML::Element* imageElement = objectElem->FirstChildElement("image"); imageElement;
                         imageElement               = imageElement->NextSiblingElement("image")) {
                        const char* sourceId = XML::Attribute(imageElement, "source");

                        if (sourceId) {
                            fs::path imagePath(filename);

                            if (HasParentPath(imagePath)) {
                                imagePath = GetParentPath(imagePath) / sourceId;
                            } else {
                                imagePath = fs::path(sourceId);
                            }

                            if (!fs::exists(imagePath)) {
                                imagePath = fs::path(sourceId);
                            }

                            ImagePtr imagePtr;
                            if (!m_imageManager->exists(imagePath.string())) {
                                imagePtr = m_imageManager->create(imagePath.string());
                            } else {
                                imagePtr = m_imageManager->getPtr(imagePath.string());
                            }

                            if (imagePtr) {
                                int xOffset = 0;
                                int success = imageElement->QueryIntAttribute("x_offset", &xOffset);

                                if (success == XML::SUCCESS) {
                                    imagePtr->setXShift(xOffset);
                                }

                                int yOffset = 0;
                                success     = imageElement->QueryIntAttribute("y_offset", &yOffset);

                                if (success == XML::SUCCESS) {
                                    imagePtr->setYShift(yOffset);
                                }

                                int direction = 0;
                                success       = imageElement->QueryIntAttribute("direction", &direction);

                                if (success == XML::SUCCESS) {
                                    ObjectVisual* objVisual = obj->getVisual<ObjectVisual>();

                                    if (objVisual) {
                                        objVisual->addStaticImage(
                                            direction, static_cast<int32_t>(imagePtr->getHandle()));
                                    }
                                }
                            }
                        }
                    }

                    for (XML::Element* actionElement = objectElem->FirstChildElement("action"); actionElement;
                         actionElement               = actionElement->NextSiblingElement("action")) {
                        const char* actionId = XML::Attribute(actionElement, "id");

                        if (actionId) {
                            int isDefault = 0;
                            actionElement->QueryIntAttribute("default", &isDefault);
                            Action* action = obj->createAction(actionId, (isDefault != 0));

                            // Fetch ActionAudio data
                            XML::Element* soundElement = actionElement->FirstChildElement("sound");
                            if (soundElement) {
                                const char* clip = XML::Attribute(soundElement, "source");
                                if (clip) {
                                    ActionAudio* audio = new ActionAudio();
                                    action->adoptAudio(audio);
                                    audio->setSoundFileName(clip);

                                    const char* group = XML::Attribute(soundElement, "group");
                                    if (group) {
                                        audio->setGroupName(group);
                                    }

                                    float value = 0;
                                    int success = XML::QueryAttribute(soundElement, "volume", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setGain(value);
                                    success = XML::QueryAttribute(soundElement, "max_volume", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setMaxGain(value);
                                    success = XML::QueryAttribute(soundElement, "min_volume", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setMinGain(value);
                                    success = XML::QueryAttribute(soundElement, "ref_distance", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setReferenceDistance(value);
                                    success = XML::QueryAttribute(soundElement, "max_distance", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setMaxDistance(value);
                                    success = XML::QueryAttribute(soundElement, "rolloff", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setRolloff(value);
                                    success = XML::QueryAttribute(soundElement, "pitch", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setPitch(value);
                                    success = XML::QueryAttribute(soundElement, "cone_inner_angle", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setConeInnerAngle(value);
                                    success = XML::QueryAttribute(soundElement, "cone_outer_angle", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setConeOuterAngle(value);
                                    success = XML::QueryAttribute(soundElement, "cone_outer_gain", &value);
                                    if (success == XML::SUCCESS)
                                        audio->setConeOuterGain(value);

                                    int boolValue = 0;
                                    success       = soundElement->QueryIntAttribute("looping", &boolValue);
                                    if (success == XML::SUCCESS)
                                        audio->setLooping(boolValue != 0);
                                    success = soundElement->QueryIntAttribute("relative_position", &boolValue);
                                    if (success == XML::SUCCESS)
                                        audio->setRelativePositioning(boolValue != 0);
                                    success = soundElement->QueryIntAttribute("direction", &boolValue);
                                    if (success == XML::SUCCESS)
                                        audio->setDirection(boolValue != 0);

                                    double vx = 0;
                                    double vy = 0;
                                    double vz = 0;
                                    if (XML::QueryAttribute(soundElement, "x_velocity", &vx) == XML::SUCCESS &&
                                        XML::QueryAttribute(soundElement, "y_velocity", &vy) == XML::SUCCESS) {
                                        XML::QueryAttribute(soundElement, "z_velocity", &vz);
                                        audio->setVelocity(AudioSpaceCoordinate(vx, vy, vz));
                                    }
                                }
                            }

                            // Create and fetch ActionVisual
                            ActionVisual::create(action);

                            for (XML::Element* animElement = actionElement->FirstChildElement("animation"); animElement;
                                 animElement               = animElement->NextSiblingElement("animation")) {
                                // Fetch already created animation
                                const char* animationId = XML::Attribute(animElement, "animation_id");
                                if (animationId) {
                                    AnimationPtr animation = m_animationManager->getPtr(animationId);
                                    if (animation) {
                                        ActionVisual* actionVisual = action->getVisual<ActionVisual>();
                                        if (actionVisual) {
                                            actionVisual->addAnimation(animation->getDirection(), animation);
                                            action->setDuration(animation->getDuration());
                                            continue;
                                        }
                                    }
                                }

                                // Create animated spritesheet
                                const char* sourceId = XML::Attribute(animElement, "atlas");
                                if (sourceId) {
                                    fs::path atlasPath(filename);

                                    if (HasParentPath(atlasPath)) {
                                        atlasPath = GetParentPath(atlasPath) / sourceId;
                                    } else {
                                        atlasPath = fs::path(sourceId);
                                    }

                                    ImagePtr atlasImgPtr;
                                    // we need to load this since its shared image
                                    if (!m_imageManager->exists(atlasPath.string())) {
                                        atlasImgPtr = m_imageManager->create(atlasPath.string());
                                    } else {
                                        atlasImgPtr = m_imageManager->getPtr(atlasPath.string());
                                    }

                                    int animFrames  = 0;
                                    int animDelay   = 0;
                                    int animXoffset = 0;
                                    int animYoffset = 0;
                                    int frameWidth  = 0;
                                    int frameHeight = 0;

                                    XML::QueryAttribute(animElement, "width", &frameWidth);
                                    XML::QueryAttribute(animElement, "height", &frameHeight);
                                    XML::QueryAttribute(animElement, "frames", &animFrames);
                                    XML::QueryAttribute(animElement, "delay", &animDelay);
                                    XML::QueryAttribute(animElement, "x_offset", &animXoffset);
                                    XML::QueryAttribute(animElement, "y_offset", &animYoffset);
                                    int nDir = 0;

                                    for (XML::Element* dirElement = animElement->FirstChildElement("direction");
                                         dirElement;
                                         dirElement = dirElement->NextSiblingElement("direction")) {
                                        int dir;
                                        dirElement->QueryIntAttribute("dir", &dir);

                                        static char tmp[64];
                                        snprintf(tmp, 64, "%03d", dir);
                                        std::string aniId =
                                            std::string(objectId) + ":" + actionId + ":" + std::string(tmp);
                                        AnimationPtr animation = m_animationManager->create(aniId);

                                        int frames;
                                        int success = XML::QueryAttribute(dirElement, "frames", &frames);
                                        if (success != XML::SUCCESS) {
                                            frames = animFrames;
                                        }

                                        int delay;
                                        success = XML::QueryAttribute(dirElement, "delay", &delay);
                                        if (success != XML::SUCCESS) {
                                            delay = animDelay;
                                        }

                                        int xoffset;
                                        success = XML::QueryAttribute(dirElement, "x_offset", &xoffset);
                                        if (success != XML::SUCCESS) {
                                            xoffset = animXoffset;
                                        }

                                        int yoffset;
                                        success = XML::QueryAttribute(dirElement, "y_offset", &yoffset);
                                        if (success != XML::SUCCESS) {
                                            yoffset = animYoffset;
                                        }

                                        int action_frame;
                                        success = XML::QueryAttribute(dirElement, "action_frame", &action_frame);
                                        if (success == XML::SUCCESS) {
                                            animation->setActionFrame(action_frame);
                                        }

                                        for (int iframe = 0; iframe < frames; ++iframe) {
                                            static char tmpBuf[64];
                                            snprintf(tmpBuf, 64, "%03d:%04d", dir, iframe);

                                            std::string frameId =
                                                std::string(objectId) + ":" + actionId + ":" + std::string(tmpBuf);
                                            Rect region(
                                                frameWidth * iframe, frameHeight * nDir, frameWidth, frameHeight);
                                            ImagePtr framePtr;
                                            if (!m_imageManager->exists(frameId)) {
                                                framePtr = m_imageManager->create(frameId);
                                                framePtr->useSharedImage(atlasImgPtr, region);
                                                framePtr->setXShift(xoffset);
                                                framePtr->setYShift(yoffset);
                                            } else {
                                                framePtr = m_imageManager->getPtr(frameId);
                                            }
                                            animation->addFrame(framePtr, delay);
                                        }

                                        ActionVisual* actionVisual = action->getVisual<ActionVisual>();
                                        if (actionVisual) {
                                            actionVisual->addAnimation(dir, animation);
                                            action->setDuration(animation->getDuration());
                                        }
                                        ++nDir;
                                    }
                                    continue;
                                }

                                // Load animation.xml with frames
                                sourceId = XML::Attribute(animElement, "source");
                                if (sourceId) {
                                    int direction = 0;
                                    int success   = XML::QueryAttribute(animElement, "direction", &direction);

                                    fs::path animPath(filename);

                                    if (HasParentPath(animPath)) {
                                        animPath = GetParentPath(animPath) / sourceId;
                                    } else {
                                        animPath = fs::path(sourceId);
                                    }

                                    AnimationPtr animation;
                                    if (m_animationLoader && m_animationLoader->isLoadable(animPath.string())) {
                                        animation = m_animationLoader->load(animPath.string());
                                    }

                                    if (action && animation) {
                                        if (success != XML::SUCCESS) {
                                            direction = animation->getDirection();
                                        }
                                        ActionVisual* actionVisual = action->getVisual<ActionVisual>();
                                        if (actionVisual) {
                                            actionVisual->addAnimation(direction, animation);
                                            action->setDuration(animation->getDuration());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void ObjectLoader::loadImportFile(const std::string& file, const std::string& directory)
    {
        if (!file.empty()) {
            fs::path importFilePath(directory);
            importFilePath /= file;

            std::string importFileString = importFilePath.string();
            if (m_atlasLoader && m_atlasLoader->isLoadable(importFileString)) {
                m_atlasLoader->loadMultiple(importFileString);
            }
            if (m_animationLoader && m_animationLoader->isLoadable(importFileString)) {
                m_animationLoader->loadMultiple(importFileString);
            }
            if (isLoadable(importFileString)) {
                load(importFileString);
            }
        }
    }

    void ObjectLoader::loadImportDirectory(const std::string& directory)
    {
        if (!directory.empty()) {
            fs::path importDirectory(directory);
            std::string importDirectoryString = importDirectory.string();

            std::set<std::string> files = m_vfs->listFiles(importDirectoryString);

            // load all xml files in the directory
            std::set<std::string>::iterator iter;
            for (iter = files.begin(); iter != files.end(); ++iter) {
                // TODO - vtchill - may need a way to allow clients to load things other
                // than .xml and .zip files
                std::string ext = GetExtension(*iter);
                if (ext == ".xml" || ext == ".zip") {
                    loadImportFile(*iter, importDirectoryString);
                }
            }

            std::set<std::string> nestedDirectories = m_vfs->listDirectories(importDirectoryString);
            for (iter = nestedDirectories.begin(); iter != nestedDirectories.end(); ++iter) {
                // do not attempt to load anything from a .svn directory
                if ((*iter).find(".svn") == std::string::npos) {
                    loadImportDirectory(importDirectoryString + "/" + *iter);
                }
            }
        }
    }
} // namespace FIFE