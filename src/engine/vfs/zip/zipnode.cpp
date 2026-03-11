// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/filesystem.h"

#include "zipnode.h"

namespace
{
    /**
     * Helper function to find a value in a ZipNodeContainer
     *
     * @param container the ZipNodeContainer to search through
     * @param name the name to search for
     * @return the ZipNode pointer, or NULL if not found
     */
    FIFE::ZipNode* FindNameInContainer(const FIFE::ZipNodeContainer& container, const std::string& name)
    {
        auto it = std::find_if(container.begin(), container.end(), [&](const auto* node) {
            return node->getName() == name;
        });

        return (it != container.end()) ? *it : nullptr;
    }

    FIFE::ZipNodeContainer::iterator FindNameInContainer(FIFE::ZipNodeContainer& container, const std::string& name)
    {
        return std::find_if(container.begin(), container.end(), [&](const auto* node) {
            return node->getName() == name;
        });
    }
} // namespace

namespace FIFE
{
    ZipEntryData::ZipEntryData() : comp(0), crc32(0), size_comp(0), size_real(0), offset(0) { }

    ZipNode::ZipNode(std::string name, ZipNode* parent /*=0*/) : m_name(std::move(name)), m_parent(parent)
    {

        // set the content type based on whether
        // the name has an extension
        if (HasExtension(m_name)) {
            m_contentType = ZipContentType::File;
        } else {
            m_contentType = ZipContentType::Directory;
        }
    }

    ZipNode::~ZipNode()
    {
        ZipNodeContainer::iterator iter;
        for (iter = m_fileChildren.begin(); iter != m_fileChildren.end(); ++iter) {
            delete *iter;
        }
        m_fileChildren.clear();

        for (iter = m_directoryChildren.begin(); iter != m_directoryChildren.end(); ++iter) {
            delete *iter;
        }
        m_directoryChildren.clear();
    }

    std::string ZipNode::getName() const
    {
        return m_name;
    }

    std::string ZipNode::getFullName() const
    {
        // traverse up the hierarchy of parents
        // to build the full path
        if (m_parent != nullptr) {
            fs::path path(m_parent->getFullName());
            path /= m_name;
            return (path.string());
        } else {
            return m_name;
        }
    }

    ZipContentType::Enum ZipNode::getContentType() const
    {
        return m_contentType;
    }

    ZipNode* ZipNode::getParent() const
    {
        return m_parent;
    }

    std::vector<ZipNode*> ZipNode::getChildren(ZipContentType::Enum contentType /*=ZipContentType::All*/) const
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
        case ZipContentType::File: {
            return m_fileChildren;
        }
        case ZipContentType::Directory: {
            return m_directoryChildren;
        }
        }
    }

    ZipNode* ZipNode::getChild(const std::string& name, ZipContentType::Enum contentType /*=ZipContentType::All*/) const
    {
        bool hasExtension = HasExtension(name);

        switch (contentType) {
        default: // fall through on purpose
        case ZipContentType::All: {
            ZipNode* node = nullptr;
            if (hasExtension) {
                node = FindNameInContainer(m_fileChildren, name);
            } else {
                node = FindNameInContainer(m_directoryChildren, name);
            }

            return node;
        }
        case ZipContentType::File: {
            if (!hasExtension) {
                return nullptr;
            }

            return FindNameInContainer(m_fileChildren, name);
        }
        case ZipContentType::Directory: {
            if (hasExtension) {
                return nullptr;
            }

            return FindNameInContainer(m_directoryChildren, name);
        }
        }
    }

    ZipNode* ZipNode::addChild(const std::string& name)
    {
        auto* child = new ZipNode(name, this);
        if (child != nullptr) {
            if (child->getContentType() == ZipContentType::File) {
                m_fileChildren.push_back(child);
            } else if (child->getContentType() == ZipContentType::Directory) {
                m_directoryChildren.push_back(child);
            } else {
                // TODO - vtchill - error case here, maybe exception
            }
        }

        return child;
    }

    void ZipNode::removeChild(ZipNode* child)
    {
        if (child != nullptr) {
            if (child->getContentType() == ZipContentType::File) {
                ZipNodeContainer::iterator iter;
                iter = std::ranges::find(m_fileChildren, child);

                if (iter != m_fileChildren.end()) {
                    delete *iter;
                    m_fileChildren.erase(iter);
                }
            }
        }
    }

    void ZipNode::removeChild(const std::string& name)
    {
        if (HasExtension(name)) {
            auto iter = FindNameInContainer(m_fileChildren, name);

            if (iter != m_fileChildren.end()) {
                delete *iter;
                m_fileChildren.erase(iter);
            }
        } else {
            auto iter = FindNameInContainer(m_directoryChildren, name);

            if (iter != m_directoryChildren.end()) {
                delete *iter;
                m_directoryChildren.erase(iter);
            }
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

    void ZipNode::setZipEntryData(const ZipEntryData& entryData)
    {
        m_entryData = entryData;
    }

    const ZipEntryData& ZipNode::getZipEntryData() const
    {
        return m_entryData;
    }
} // namespace FIFE

std::ostream& operator<<(std::ostream& os, const FIFE::ZipNode& node)
{
    // print node name first
    os << node.getName() << '\n';

    // print all file children
    FIFE::ZipNodeContainer fileChildren = node.getChildren(FIFE::ZipContentType::File);
    FIFE::ZipNodeContainer::iterator iter;
    for (iter = fileChildren.begin(); iter != fileChildren.end(); ++iter) {
        os << *(*iter) << '\n';
    }

    // print all directory children (recursively)
    FIFE::ZipNodeContainer directoryChildren = node.getChildren(FIFE::ZipContentType::Directory);
    for (iter = directoryChildren.begin(); iter != directoryChildren.end(); ++iter) {
        os << *(*iter) << '\n';
    }

    return os;
}
