// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <memory>
#include <string>
#include <vector>

#include "fife_unittest.h"
#include "vfs/directoryprovider.h"
#include "vfs/filesystemassetprovider.h"
#include "vfs/vfsassetprovider.h"
#include "vfs/vfsdirectory.h"
#include "video/fonts/assetresolver.h"

using FIFE::AssetHandle;
using FIFE::AssetProvider;
using FIFE::AssetRequest;
using FIFE::AssetResolver;
using FIFE::FilesystemAssetProvider;
using FIFE::VFS;
using FIFE::VfsAssetProvider;

class MockProvider : public AssetProvider
{
    public:
        explicit MockProvider(int prio, bool can, AssetHandle handle) : m_prio(prio), m_can(can), m_handle(handle)
        {
        }
        bool canResolve(AssetRequest const &) const override
        {
            return m_can;
        }
        AssetHandle resolve(AssetRequest const &) const override
        {
            return m_handle;
        }
        int priority() const override
        {
            return m_prio;
        }

    private:
        int m_prio;
        bool m_can;
        AssetHandle m_handle;
};

TEST_CASE("AssetResolver returns handle from single provider", "[asset][resolver]")
{
    AssetResolver resolver;
    resolver.addProvider(std::make_unique<MockProvider>(0, true, AssetHandle{42}));

    AssetRequest req;
    req.id      = "test";
    req.source  = "test.txt";
    auto handle = resolver.resolve(req);
    CHECK_EQ(handle.id, 42);
}

TEST_CASE("AssetResolver tries providers in priority order", "[asset][resolver]")
{
    AssetResolver resolver;
    resolver.addProvider(std::make_unique<MockProvider>(200, false, AssetHandle{1}));
    resolver.addProvider(std::make_unique<MockProvider>(100, true, AssetHandle{2}));

    AssetRequest req;
    req.id      = "test";
    auto handle = resolver.resolve(req);
    CHECK_EQ(handle.id, 2);
}

TEST_CASE("AssetResolver returns invalid handle when no provider canResolve", "[asset][resolver]")
{
    AssetResolver resolver;
    resolver.addProvider(std::make_unique<MockProvider>(0, false, AssetHandle{1}));

    AssetRequest req;
    req.id      = "test";
    auto handle = resolver.resolve(req);
    CHECK_EQ(handle.id, 0);
}

TEST_CASE("VfsAssetProvider returns correct priority", "[asset][vfs]")
{
    auto vfs = std::make_shared<VFS>();
    VfsAssetProvider provider(vfs.get());
    CHECK_EQ(provider.priority(), 100);
}

TEST_CASE("FilesystemAssetProvider returns correct priority", "[asset][filesystem]")
{
    FilesystemAssetProvider provider(std::vector<std::string>{});
    CHECK_EQ(provider.priority(), 200);
}

TEST_CASE("AssetResolver sorts providers by priority regardless of add order", "[asset][resolver]")
{
    AssetResolver resolver;
    resolver.addProvider(std::make_unique<MockProvider>(300, true, AssetHandle{3}));
    resolver.addProvider(std::make_unique<MockProvider>(100, false, AssetHandle{1}));
    resolver.addProvider(std::make_unique<MockProvider>(200, true, AssetHandle{2}));

    AssetRequest req;
    req.id      = "test";
    auto handle = resolver.resolve(req);

    CHECK_EQ(handle.id, 2);
}

TEST_CASE("Empty AssetResolver returns invalid handle", "[asset][resolver]")
{
    AssetResolver resolver;

    AssetRequest req;
    req.id      = "test";
    auto handle = resolver.resolve(req);
    CHECK_EQ(handle.id, 0);
}

TEST_CASE("AssetHandle IDs are unique across calls", "[asset][resolver]")
{
    std::vector<uint64_t> ids;
    AssetRequest req1, req2, req3;
    req1.source = "unique1.txt";
    req2.source = "unique2.txt";
    req3.source = "unique3.txt";

    {
        FilesystemAssetProvider provider(std::vector<std::string>{});
        auto h1 = provider.resolve(req1);
        auto h2 = provider.resolve(req2);
        auto h3 = provider.resolve(req3);
        ids     = {h1.id, h2.id, h3.id};
    }

    CHECK_NE(ids[0], ids[1]);
    CHECK_NE(ids[1], ids[2]);
    CHECK_NE(ids[0], ids[2]);
}

TEST_CASE("VfsAssetProvider canResolve with existing VFS file", "[asset][vfs][integration]")
{
    auto vfs = std::make_shared<VFS>();
    vfs->addSource(std::make_unique<FIFE::VFSDirectory>(vfs.get()));
    vfs->addProvider(std::make_unique<FIFE::DirectoryProvider>());

    VfsAssetProvider provider(vfs.get());

    AssetRequest req;
    req.source = "tests/data/FreeMono.ttf";
    CHECK(provider.canResolve(req));
}

TEST_CASE("VfsAssetProvider canResolve with missing VFS file returns false", "[asset][vfs]")
{
    auto vfs = std::make_shared<VFS>();
    VfsAssetProvider provider(vfs.get());

    AssetRequest req;
    req.source = "nonexistent.ttf";
    CHECK_FALSE(provider.canResolve(req));
}

TEST_CASE("FilesystemAssetProvider canResolve with existing file", "[asset][filesystem][integration]")
{
    FilesystemAssetProvider provider({"."});

    AssetRequest req;
    req.source = "tests/data/FreeMono.ttf";
    CHECK(provider.canResolve(req));
}

TEST_CASE("FilesystemAssetProvider canResolve with absolute path", "[asset][filesystem]")
{
    FilesystemAssetProvider provider({});

    AssetRequest req;
    req.source = "/workspaces/fifengine_bugfixc/tests/data/FreeMono.ttf";
    CHECK(provider.canResolve(req));
}

TEST_CASE("FilesystemAssetProvider canResolve with missing file returns false", "[asset][filesystem]")
{
    FilesystemAssetProvider provider({});

    AssetRequest req;
    req.source = "nonexistent.ttf";
    CHECK_FALSE(provider.canResolve(req));
}

TEST_CASE("FilesystemAssetProvider canResolve with search path", "[asset][filesystem]")
{
    FilesystemAssetProvider provider({"tests/data"});

    AssetRequest req;
    req.source = "FreeMono.ttf";
    CHECK(provider.canResolve(req));
}
