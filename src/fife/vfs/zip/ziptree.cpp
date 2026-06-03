// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "ziptree.h"

#include <cassert>
#include <filesystem>
#include <string>

#include "vfs/filesystem.h"
#include "zipnode.h"

namespace fs = std::filesystem;

namespace FIFE
{

    ZipTree::ZipTree() : m_rootNode(std::make_unique<ZipNode>("/"))
    {
    }

    ZipNode* ZipTree::addNode(std::string const & nodePath, ZipEntryType entryType)
    {
        std::string cleaned = nodePath;
        if (!cleaned.empty() && cleaned.back() == '/') {
            cleaned.pop_back();
        }

        fs::path const filePath(cleaned);
        ZipNode* node       = m_rootNode.get();
        ZipNode* returnNode = nullptr;

        for (auto it = filePath.begin(); it != filePath.end(); ++it) {
            std::string const component = GetPathIteratorAsString(it);
            bool const isLast           = (std::next(it) == filePath.end());

            ZipNode* child = node->getChild(component);
            if (child == nullptr) {
                if (isLast) {
                    node = node->addChild(component, entryType);
                } else {
                    node = node->addChild(component, ZipEntryType::Directory);
                }
            } else {
                node = child;
            }

            if (node != nullptr) {
                returnNode = node;
            } else {
                return nullptr;
            }
        }

        return returnNode;
    }

    ZipNode* ZipTree::getNode(std::string const & name) const
    {
        fs::path const filePath(name);
        ZipNode* returnNode = nullptr;
        ZipNode* node       = getRootNode();

        for (auto it = filePath.begin(); it != filePath.end(); ++it) {
            std::string const pathString = GetPathIteratorAsString(it);

            if (pathString == ".." && node != getRootNode()) {
                node = node->getParent();
            } else {
                ZipNode* found = node->getChild(pathString);
                if (found != nullptr) {
                    node       = found;
                    returnNode = node;
                }
            }
        }

        return returnNode;
    }

    ZipNode* ZipTree::getRootNode() const
    {
        return m_rootNode.get();
    }
} // namespace FIFE

std::ostream& operator<<(std::ostream& os, FIFE::ZipTree const & zipTree)
{
    assert(zipTree.getRootNode() != nullptr);
    os << *(zipTree.getRootNode());
    return os;
}
