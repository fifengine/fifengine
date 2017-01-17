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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/fife_boost_filesystem.h"

#include "zipnode.h"

namespace {
    /** helper function to find a value in a ZipNodeContainer
     *  @param container the ZipNodeContainer to search through
        @param the name to search for
        @return the ZipNode pointer, or NULL if not found
     */
    FIFE::ZipNode* FindNameInContainer(const FIFE::ZipNodeContainer& container, 
                                       const std::string& name) {
        for (FIFE::ZipNodeContainer::const_iterator iter = container.begin(); iter != container.end(); ++iter) {
            if ((*iter)->getName() == name) {
                return *iter;
            }
        }

        return 0;
    }

    FIFE::ZipNodeContainer::iterator FindNameInContainer(FIFE::ZipNodeContainer& container, 
                                                         const std::string& name)
    {
        for (FIFE::ZipNodeContainer::iterator iter = container.begin(); iter != container.end(); ++iter) {
            if ((*iter)->getName() == name) {
                return iter;
            }
        }

        return container.end();
    }
}

namespace FIFE {
    ZipEntryData::ZipEntryData() 
    : comp(0), crc32(0), size_comp(0), size_real(0), offset(0) { 
    }

    ZipNode::ZipNode(const std::string& name, ZipNode* parent/*=0*/)
    : m_name(name), m_parent(parent) {

        // set the content type based on whether
        // the name has an extension
        if (HasExtension(m_name))
        {
            m_contentType = ZipContentType::File;
        }
        else
        {
            m_contentType = ZipContentType::Directory;
        }
    }

    ZipNode::~ZipNode() {
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

    std::string ZipNode::getName() const {
        return m_name;
    }

    std::string ZipNode::getFullName() const {
        // traverse up the hierarchy of parents
        // to build the full path
        if (m_parent)
        {
            bfs::path path(m_parent->getFullName());
            path /= m_name;
            return (path.string());
        }
        else
        {
            return m_name;
        }
    }

    ZipContentType::Enum ZipNode::getContentType() const {
        return m_contentType;
    }

    ZipNode* ZipNode::getParent() const {
        return m_parent;
    }

    std::vector<ZipNode*> ZipNode::getChildren(ZipContentType::Enum contentType/*=ZipContentType::All*/) const {
        switch (contentType) {
            default:                    // fall through on purpose
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

    ZipNode* ZipNode::getChild(const std::string& name, 
                               ZipContentType::Enum contentType/*=ZipContentType::All*/) const {
        bool hasExtension = HasExtension(name);
        
        switch (contentType) {
            default:                    // fall through on purpose
            case ZipContentType::All: {
                ZipNode* node = 0;
                if (hasExtension) {
                    node = FindNameInContainer(m_fileChildren, name);
                }
                else {
                    node = FindNameInContainer(m_directoryChildren, name);
                }

                return node;
            }
            case ZipContentType::File: {
                if (!hasExtension) {
                    return 0;
                }

                return FindNameInContainer(m_fileChildren, name);
            }
            case ZipContentType::Directory: {
                if (hasExtension) {
                    return 0;
                }

                return FindNameInContainer(m_directoryChildren, name);
            }
        }
    }

    ZipNode* ZipNode::addChild(const std::string& name) {
        ZipNode* child = new ZipNode(name, this);
        if (child) {
            if (child->getContentType() == ZipContentType::File) {
                m_fileChildren.push_back(child);
            }
            else if (child->getContentType() == ZipContentType::Directory) {
                m_directoryChildren.push_back(child);
            }
            else {
                // TODO - vtchill - error case here, maybe exception
            }
        }

        return child;
    }

    void ZipNode::removeChild(ZipNode* child) {
        if (child) {
            if (child->getContentType() == ZipContentType::File) {
                ZipNodeContainer::iterator iter;
                iter = std::find(m_fileChildren.begin(), m_fileChildren.end(), child);
                
                if (iter != m_fileChildren.end()) {
                    delete *iter;
                    m_fileChildren.erase(iter);
                }
            }
        }
    }

    void ZipNode::removeChild(const std::string& name) {
        if (HasExtension(name)) {
            ZipNodeContainer::iterator iter = FindNameInContainer(m_fileChildren, name);

            if (iter != m_fileChildren.end()) {
                delete *iter;
                m_fileChildren.erase(iter);
            }
        }
        else {
            ZipNodeContainer::iterator iter = FindNameInContainer(m_directoryChildren, name);

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
}

std::ostream& operator<<(std::ostream& os, const FIFE::ZipNode& node) {
    // print node name first
    os << node.getName() << std::endl;

    // print all file children
    FIFE::ZipNodeContainer fileChildren = node.getChildren(FIFE::ZipContentType::File);
    FIFE::ZipNodeContainer::iterator iter;
    for (iter = fileChildren.begin(); iter != fileChildren.end(); ++iter) {
        os << *(*iter) << std::endl;
    }

    // print all directory children (recursively)
    FIFE::ZipNodeContainer directoryChildren = node.getChildren(FIFE::ZipContentType::Directory);
    for (iter = directoryChildren.begin(); iter != directoryChildren.end(); ++iter) {
        os << *(*iter) << std::endl;
    }

    return os;
}