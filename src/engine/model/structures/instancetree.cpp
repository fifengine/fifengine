// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instance.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"

#include "instancetree.h"

namespace FIFE
{
    static Logger _log(LM_STRUCTURES);

    InstanceTree::InstanceTree() = default;

    InstanceTree::~InstanceTree() = default;

    void InstanceTree::addInstance(Instance* instance)
    {
        const ModelCoordinate coords = instance->getLocationRef().getLayerCoordinates();
        InstanceTreeNode* node       = m_tree.find_container(coords.x, coords.y, 0, 0);
        InstanceList& list           = node->data();
        list.push_back(instance);
        if (m_reverse.contains(instance)) {
            FL_WARN(_log, "InstanceTree::addInstance() - Duplicate Instance.  Ignoring.");
            return;
        }
        m_reverse[instance] = node;
    }

    void InstanceTree::removeInstance(Instance* instance)
    {
        InstanceTreeNode* node = m_reverse[instance];
        if (node == nullptr) {
            FL_WARN(_log, "InstanceTree::removeInstance() - Instance not part of tree.");
            return;
        }
        m_reverse.erase(instance);
        InstanceList& list = node->data();

        auto it = std::ranges::find_if(list, [instance](const Instance* i) {
            return i == instance;
        });
        if (it != list.end()) {
            list.erase(it);
            return;
        }
        FL_WARN(
            _log, "InstanceTree::removeInstance() - Instance part of tree but not found in the expected tree node.");
    }

    class InstanceListCollector
    {
    public:
        InstanceTree::InstanceList* instanceList;
        Rect searchRect;
        InstanceListCollector(InstanceTree::InstanceList* a_instanceList, const Rect& rect) :
            instanceList(a_instanceList), searchRect(rect)
        {
        }
        bool visit(InstanceTree::InstanceTreeNode* node, int32_t d);
    };

    bool InstanceListCollector::visit(InstanceTree::InstanceTreeNode* node, int32_t d)
    {
        InstanceTree::InstanceList& list = node->data();
        for (InstanceTree::InstanceList::const_iterator it(list.begin()); it != list.end(); ++it) {
            ModelCoordinate coords = (*it)->getLocationRef().getLayerCoordinates();
            if (searchRect.contains(Point(coords.x, coords.y))) {
                instanceList->push_back(*it);
            }
        }
        return true;
    }

    void InstanceTree::findInstances(
        const ModelCoordinate& point, int32_t w, int32_t h, InstanceTree::InstanceList& list)
    {
        list.clear();
        InstanceTreeNode* node = m_tree.find_container(point.x, point.y, w, h);
        const Rect rect(point.x, point.y, w, h);
        InstanceListCollector collector(&list, rect);

        node->apply_visitor(collector);

        node = node->parent();
        while (node != nullptr) {
            for (InstanceList::const_iterator it(node->data().begin()); it != node->data().end(); ++it) {
                const ModelCoordinate coords = (*it)->getLocationRef().getLayerCoordinates();
                if (rect.contains(Point(coords.x, coords.y))) {
                    list.push_back(*it);
                }
            }
            node = node->parent();
        }
    }

} // namespace FIFE
