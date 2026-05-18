// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "objectsaver.h"

// Standard C++ library includes
#include <cassert>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// FIFE includes
#include "audio/actionaudio.h"
#include "model/metamodel/action.h"
#include "model/metamodel/ipather.h"
#include "model/metamodel/object.h"
#include "model/model.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/xml/xmlhelper.h"
#include "video/animation.h"
#include "video/imagemanager.h"
#include "view/visual.h"

namespace FIFE
{
    namespace
    {
        Logger& _log = []() -> Logger& {
            static Logger log(LM_NATIVE_SAVERS);
            return log;
        }();
    } // namespace

    namespace
    {
        void writeWalkableAreas(XML::Element* parent, Object const * object)
        {
            std::list<std::string> const areas = object->getWalkableAreas();
            if (areas.empty()) {
                return;
            }

            XML::Document* doc = parent->GetDocument();

            for (auto const & areaId : areas) {
                XML::Element* walkableElement = doc->NewElement("walkable_area");
                walkableElement->SetAttribute("id", areaId.c_str());
                parent->InsertEndChild(walkableElement);
            }
        }

        void writeMultiParts(XML::Element* parent, Object const * object)
        {
            std::list<std::string> const partIds = object->getMultiPartIds();
            if (partIds.empty()) {
                return;
            }

            XML::Document* doc = parent->GetDocument();

            std::multimap<int32_t, ModelCoordinate> const coords = object->getMultiPartCoordinates();

            for (auto const & partId : partIds) {
                XML::Element* multiElement = doc->NewElement("multipart");
                multiElement->SetAttribute("id", partId.c_str());

                std::set<int32_t> rotations;
                for (auto const& [rotation, _] : coords) {
                    static_cast<void>(_);
                    rotations.insert(rotation);
                }

                for (int32_t const rotation : rotations) {
                    XML::Element* rotationElement = doc->NewElement("rotation");
                    rotationElement->SetAttribute("rot", rotation);

                    auto [begin, end] = coords.equal_range(rotation);
                    for (auto it = begin; it != end; ++it) {
                        XML::Element* coordElement = doc->NewElement("occupied_coord");
                        coordElement->SetAttribute("x", it->second.x);
                        coordElement->SetAttribute("y", it->second.y);
                        if (it->second.z != 0) {
                            coordElement->SetAttribute("z", it->second.z);
                        }
                        rotationElement->InsertEndChild(coordElement);
                    }

                    multiElement->InsertEndChild(rotationElement);
                }

                parent->InsertEndChild(multiElement);
            }
        }

        void writeImages(XML::Element* parent, Object const * object, ImageManager* imageManager)
        {
            auto* objVisual = object->getVisual<ObjectVisual>();
            if (objVisual == nullptr) {
                return;
            }

            std::vector<int32_t> angles;
            objVisual->getStaticImageAngles(angles);

            if (angles.empty()) {
                return;
            }

            XML::Document* doc = parent->GetDocument();

            for (int32_t const angle : angles) {
                int32_t const imageIndex = objVisual->getStaticImageIndexByAngle(angle);
                ImagePtr const image     = imageManager->getPtr(static_cast<ResourceHandle>(imageIndex));
                if (!image) {
                    continue;
                }

                XML::Element* imageElement = doc->NewElement("image");
                imageElement->SetAttribute("direction", angle);
                imageElement->SetAttribute("source", image->getName().c_str());

                int32_t const xOffset = image->getXShift();
                int32_t const yOffset = image->getYShift();
                if (xOffset != 0) {
                    imageElement->SetAttribute("x_offset", xOffset);
                }
                if (yOffset != 0) {
                    imageElement->SetAttribute("y_offset", yOffset);
                }

                parent->InsertEndChild(imageElement);
            }
        }

        void writeActionSound(XML::Element* parent, Action const * action)
        {
            ActionAudio const * audio = action->getAudio();
            if (audio == nullptr) {
                return;
            }

            XML::Document* doc = parent->GetDocument();

            XML::Element* soundElement = doc->NewElement("sound");
            soundElement->SetAttribute("source", audio->getSoundFileName().c_str());

            if (!audio->getGroupName().empty()) {
                soundElement->SetAttribute("group", audio->getGroupName().c_str());
            }

            if (!Mathd::Equal(audio->getGain(), 0.0F)) {
                soundElement->SetAttribute("volume", audio->getGain());
            }

            if (!Mathd::Equal(audio->getMaxGain(), 0.0F)) {
                soundElement->SetAttribute("max_volume", audio->getMaxGain());
            }

            if (!Mathd::Equal(audio->getMinGain(), 0.0F)) {
                soundElement->SetAttribute("min_volume", audio->getMinGain());
            }

            if (!Mathd::Equal(audio->getReferenceDistance(), 0.0F)) {
                soundElement->SetAttribute("ref_distance", audio->getReferenceDistance());
            }

            if (!Mathd::Equal(audio->getMaxDistance(), 0.0F)) {
                soundElement->SetAttribute("max_distance", audio->getMaxDistance());
            }

            if (!Mathd::Equal(audio->getRolloff(), 0.0F)) {
                soundElement->SetAttribute("rolloff", audio->getRolloff());
            }

            if (!Mathd::Equal(audio->getPitch(), 0.0F)) {
                soundElement->SetAttribute("pitch", audio->getPitch());
            }

            if (!Mathd::Equal(audio->getConeInnerAngle(), 360.0F)) {
                soundElement->SetAttribute("cone_inner_angle", audio->getConeInnerAngle());
            }

            if (!Mathd::Equal(audio->getConeOuterAngle(), 360.0F)) {
                soundElement->SetAttribute("cone_outer_angle", audio->getConeOuterAngle());
            }

            if (!Mathd::Equal(audio->getConeOuterGain(), 0.0F)) {
                soundElement->SetAttribute("cone_outer_gain", audio->getConeOuterGain());
            }

            if (audio->isLooping()) {
                soundElement->SetAttribute("looping", 1);
            }

            if (audio->isRelativePositioning()) {
                soundElement->SetAttribute("relative_position", 1);
            }

            if (audio->isDirection()) {
                soundElement->SetAttribute("direction", 1);
            }

            AudioSpaceCoordinate const & velocity = audio->getVelocity();
            if (!Mathd::Equal(velocity.x, 0.0) || !Mathd::Equal(velocity.y, 0.0) || !Mathd::Equal(velocity.z, 0.0)) {
                soundElement->SetAttribute("x_velocity", velocity.x);
                soundElement->SetAttribute("y_velocity", velocity.y);
                soundElement->SetAttribute("z_velocity", velocity.z);
            }

            parent->InsertEndChild(soundElement);
        }

        void writeActionAnimations(
            XML::Element* parent, Action const * action, AnimationSaverPtr const & animationSaver)
        {
            auto* actionVisual = action->getVisual<ActionVisual>();
            if (actionVisual == nullptr) {
                return;
            }

            XML::Document* doc = parent->GetDocument();

            std::vector<int32_t> angles;
            actionVisual->getActionImageAngles(angles);

            for (int32_t const angle : angles) {
                AnimationPtr const animation = actionVisual->getAnimationByAngle(angle);
                if (!animation) {
                    continue;
                }

                XML::Element* animElement = doc->NewElement("animation");
                animElement->SetAttribute("animation_id", animation->getName().c_str());

                if (animationSaver) {
                    animElement->SetAttribute("source", (animation->getName() + ".xml").c_str());
                }

                animElement->SetAttribute("direction", angle);

                parent->InsertEndChild(animElement);
            }
        }

        void writeActions(XML::Element* parent, Object const * object, AnimationSaverPtr const & animationSaver)
        {
            XML::Document* doc = parent->GetDocument();

            std::list<std::string> const actionIds = object->getActionIds();
            for (auto const & actionId : actionIds) {
                Action* action = object->getAction(actionId, false);
                if (action == nullptr) {
                    continue;
                }

                XML::Element* actionElement = doc->NewElement("action");
                actionElement->SetAttribute("id", action->getName().c_str());

                Action const * defaultAction = object->getDefaultAction();
                if (defaultAction == action) {
                    actionElement->SetAttribute("default", 1);
                }

                writeActionSound(actionElement, action);
                writeActionAnimations(actionElement, action, animationSaver);

                parent->InsertEndChild(actionElement);
            }
        }

        void writeObject(
            XML::Element* parent,
            Object const * object,
            ImageManager* imageManager,
            AnimationSaverPtr const & animationSaver)
        {
            XML::Document* doc = parent->GetDocument();

            XML::Element* objectElement = doc->NewElement("object");
            objectElement->SetAttribute("id", object->getName().c_str());
            objectElement->SetAttribute("namespace", object->getNamespace().c_str());

            Object const * inherited = object->getInherited();
            if (inherited != nullptr) {
                objectElement->SetAttribute("parent", inherited->getName().c_str());
            }

            objectElement->SetAttribute("blocking", object->isBlocking() ? 1 : 0);
            objectElement->SetAttribute("static", object->isStatic() ? 1 : 0);

            if (object->getPather() != nullptr) {
                objectElement->SetAttribute("pather", object->getPather()->getName().c_str());
            }

            if (object->isSpecialCost()) {
                objectElement->SetAttribute("cost_id", object->getCostId().c_str());
                objectElement->SetAttribute("cost", object->getCost());
            }

            if (object->isSpecialSpeed()) {
                objectElement->SetAttribute("speed", object->getSpeed());
            }

            objectElement->SetAttribute("cellstack", object->getCellStackPosition());

            ExactModelCoordinate const anchor = object->getRotationAnchor();
            if (!Mathd::Equal(anchor.x, 0.0) || !Mathd::Equal(anchor.y, 0.0)) {
                objectElement->SetAttribute("anchor_x", anchor.x);
                objectElement->SetAttribute("anchor_y", anchor.y);
            }

            if (object->isRestrictedRotation()) {
                objectElement->SetAttribute("restricted_rotation", 1);
            }

            if (object->getZStepRange() != 0) {
                objectElement->SetAttribute("z_step_limit", object->getZStepRange());
            }

            if (!object->getArea().empty()) {
                objectElement->SetAttribute("area_id", object->getArea().c_str());
            }

            writeWalkableAreas(objectElement, object);
            writeMultiParts(objectElement, object);
            writeImages(objectElement, object, imageManager);
            writeActions(objectElement, object, animationSaver);

            parent->InsertEndChild(objectElement);
        }
    } // anonymous namespace

    ObjectSaver::ObjectSaver(Model* model, ImageManager* imageManager) :
        m_object(nullptr), m_model(model), m_imageManager(imageManager)
    {
        assert("model must not be null" && m_model);
        assert("imageManager must not be null" && m_imageManager);
    }

    ObjectSaver::~ObjectSaver() = default;

    void ObjectSaver::setAnimationSaver(AnimationSaverPtr const & animationSaver)
    {
        m_animationSaver = animationSaver;
    }

    AnimationSaverPtr ObjectSaver::getAnimationSaver()
    {
        return m_animationSaver;
    }

    void ObjectSaver::setObject(Object const * object)
    {
        assert("object must not be null" && object);
        m_object = object;
    }

    void ObjectSaver::save(std::string const & filename)
    {
        assert("object must be set before save" && m_object);

        XML::Document doc;
        auto* declaration = doc.NewDeclaration(R"(xml version="1.0" encoding="ascii")");
        doc.InsertEndChild(declaration);

        auto* processing = doc.NewDeclaration(R"(fife type="object")");
        doc.InsertEndChild(processing);

        XML::Element* assetsElement = doc.NewElement("assets");
        doc.InsertEndChild(assetsElement);

        writeObject(assetsElement, m_object, m_imageManager, m_animationSaver);

        doc.SaveFile(filename.c_str());

        FL_DBG(_log, "Saved object '" + m_object->getName() + "' to " + filename);
    }
} // namespace FIFE
