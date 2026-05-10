// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "zipnode.h"

// Standard C++ library includes
#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

// FIFE includes
#include "vfs/filesystem.h"

namespace
{
    /**
     * Helper function to find a value in a ZipNodeContainer
     *
     * @param container the ZipNodeContainer to search through
     * @param name the name to search for
     * @return the ZipNode pointer, or NULL if not found
     */
    FIFE::ZipNode* FindNameInContainer(FIFE::ZipNodeContainer const & container, std::string const & name)
    {
        auto it = std::ranges::find_if(container, [&](auto const * node) {
            return node->getName() == name;
        });
        return (it != container.end()) ? *it : nullptr;
    }

    FIFE::ZipNodeContainer::iterator FindNameInContainer(FIFE::ZipNodeContainer& container, std::string const & name)
    {
        return std::ranges::find_if(container, [&](auto const * node) {
            return node->getName() == name;
        });
    }
} // namespace

namespace FIFE
{
    ZipEntryData::ZipEntryData() : comp(0), crc32(0), size_comp(0), size_real(0), offset(0)
    {
    }

    ZipNode::ZipNode(std::string name, ZipNode* parent) : m_name(std::move(name)), m_parent(parent)
    {
        if (HasExtension(m_name)) {
            m_contentType = ZipContentType::File;
        } else {
            m_contentType = ZipContentType::Directory;
        }
    }

    ZipNode::ZipNode(std::string name, ZipNode* parent, ZipEntryType entryType) :
        m_name(std::move(name)), m_parent(parent)
    {
        switch (entryType) {
        case ZipEntryType::File:
            m_contentType = ZipContentType::File;
            break;
        case ZipEntryType::Directory:
        case ZipEntryType::Symlink:
            m_contentType = ZipContentType::Directory;
            break;
        }
    }

    ZipNode::~ZipNode()
    {
        for (auto* child : m_fileChildren) {
            delete child;
        }
        m_fileChildren.clear();

        for (auto* child : m_directoryChildren) {
            delete child;
        }
        m_directoryChildren.clear();
    }

    std::string const & ZipNode::getName() const
    {
        return m_name;
    }

    std::string ZipNode::getFullName() const
    {
        if (m_parent != nullptr) {
            fs::path path(m_parent->getFullName());
            path /= m_name;
            return path.string();
        }
        return m_name;
    }

    ZipContentType::Enum ZipNode::getContentType() const
    {
        return m_contentType;
    }

    ZipNode* ZipNode::getParent() const
    {
        return m_parent;
    }

    std::vector<ZipNode*> ZipNode::getChildren(ZipContentType::Enum contentType) const
    {
        switch (contentType) {
        default: // fall through on purpose
        case ZipContentType::All: {
            // concatenate directory and file children
            // putting all directories before files
            // reserve space in destination vector to avoid
            // tons of vector resizing overhead
            ZipNodeContainer allNodes;
            allNodes.reserve(m_directoryChildren.size() + m_fileChildren.size());
            allNodes.insert(allNodes.end(), m_directoryChildren.begin(), m_directoryChildren.end());
            allNodes.insert(allNodes.end(), m_fileChildren.begin(), m_fileChildren.end());
            return allNodes;
        }
        case ZipContentType::File:
            return m_fileChildren;
        case ZipContentType::Directory:
            return m_directoryChildren;
        }
    }

    ZipNode* ZipNode::getChild(std::string const & name, ZipContentType::Enum contentType) const
    {
        switch (contentType) {
        default: // fall through on purpose
        case ZipContentType::All: {
            ZipNode* node = FindNameInContainer(m_directoryChildren, name);
            if (node == nullptr) {
                node = FindNameInContainer(m_fileChildren, name);
            }
            return node;
        }
        case ZipContentType::File:
            return FindNameInContainer(m_fileChildren, name);
        case ZipContentType::Directory:
            return FindNameInContainer(m_directoryChildren, name);
        }
    }

    ZipNode* ZipNode::addChild(std::string const & name)
    {
        auto* child = new ZipNode(name, this);
        if (child != nullptr) {
            if (child->getContentType() == ZipContentType::File) {
                m_fileChildren.push_back(child);
            } else {
                m_directoryChildren.push_back(child);
            }
        }
        return child;
    }

    ZipNode* ZipNode::addChild(std::string const & name, ZipEntryType entryType)
    {
        auto* child = new ZipNode(name, this, entryType);
        if (child != nullptr) {
            if (child->getContentType() == ZipContentType::File) {
                m_fileChildren.push_back(child);
            } else {
                m_directoryChildren.push_back(child);
            }
        }
        return child;
    }

    void ZipNode::removeChild(ZipNode* child)
    {
        if (child == nullptr) {
            return;
        }

        auto it = std::ranges::find(m_fileChildren, child);
        if (it != m_fileChildren.end()) {
            delete *it;
            m_fileChildren.erase(it);
            return;
        }

        it = std::ranges::find(m_directoryChildren, child);
        if (it != m_directoryChildren.end()) {
            delete *it;
            m_directoryChildren.erase(it);
        }
    }

    void ZipNode::removeChild(std::string const & name)
    {
        auto it = FindNameInContainer(m_fileChildren, name);
        if (it != m_fileChildren.end()) {
            delete *it;
            m_fileChildren.erase(it);
            return;
        }

        it = FindNameInContainer(m_directoryChildren, name);
        if (it != m_directoryChildren.end()) {
            delete *it;
            m_directoryChildren.erase(it);
        }
    }

    bool ZipNode::isLeaf() const
    {
        return (m_fileChildren.empty() && m_directoryChildren.empty());
    }

    bool ZipNode::isBranch() const
    {
        return (!isLeaf());
    }

    void ZipNode::setZipEntryData(ZipEntryData const & entryData)
    {
        m_entryData = entryData;
    }

    ZipEntryData const & ZipNode::getZipEntryData() const
    {
        return m_entryData;
    }
} // namespace FIFE

std::ostream& operator<<(std::ostream& os, FIFE::ZipNode const & node)
{
    // print node name first
    os << node.getName() << '\n';

    // print all file children
    FIFE::ZipNodeContainer fileChildren = node.getChildren(FIFE::ZipContentType::File);
    for (auto* child : fileChildren) {
        os << *child << '\n';
    }

    FIFE::ZipNodeContainer directoryChildren = node.getChildren(FIFE::ZipContentType::Directory);
    for (auto* child : directoryChildren) {
        os << *child << '\n';
    }

    return os;
}
