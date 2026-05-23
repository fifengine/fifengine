// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "animationsaver.h"

// Standard C++ library includes
#include <cassert>
#include <string>
#include <vector>

// FIFE includes
#include "util/xml/xmlhelper.h"
#include "video/image.h"

namespace FIFE
{
    AnimationSaver::AnimationSaver() : m_animation(nullptr)
    {
    }

    AnimationSaver::~AnimationSaver() = default;

    void AnimationSaver::setAnimation(Animation* animation)
    {
        m_animation = animation;
    }

    void AnimationSaver::save(std::string const & filename)
    {
        assert("Animation must be set before saving" && m_animation != nullptr);

        XML::Document doc;
        auto* declaration = doc.NewDeclaration(R"(xml version="1.0" encoding="ascii")");
        doc.InsertEndChild(declaration);

        XML::Element* animationElement = doc.NewElement("animation");
        animationElement->SetAttribute("id", m_animation->getName().c_str());
        animationElement->SetAttribute("actionframe", m_animation->getActionFrame());

        uint32_t const frameCount = m_animation->getFrameCount();
        for (uint32_t i = 0; i < frameCount; ++i) {
            ImagePtr const frame   = m_animation->getFrame(static_cast<int32_t>(i));
            int32_t const duration = m_animation->getFrameDuration(static_cast<int32_t>(i));

            XML::Element* frameElement = doc.NewElement("frame");
            frameElement->SetAttribute("source", frame->getName().c_str());
            frameElement->SetAttribute("delay", static_cast<int>(duration));

            if (frame->getXShift() != 0) {
                frameElement->SetAttribute("x_offset", frame->getXShift());
            }
            if (frame->getYShift() != 0) {
                frameElement->SetAttribute("y_offset", frame->getYShift());
            }

            if (frame->isSharedImage()) {
                Rect const & rect = frame->getSubImageRect();
                frameElement->SetAttribute("xpos", rect.x);
                frameElement->SetAttribute("ypos", rect.y);
                frameElement->SetAttribute("width", static_cast<int>(rect.w));
                frameElement->SetAttribute("height", static_cast<int>(rect.h));
            }

            animationElement->InsertEndChild(frameElement);
        }

        doc.InsertEndChild(animationElement);
        doc.SaveFile(filename.c_str());
    }
} // namespace FIFE
