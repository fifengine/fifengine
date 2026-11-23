// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "model/metamodel/grids/squaregrid.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/renderernode.h"
#include "util/structures/point.h"

using FIFE::ExactModelCoordinate;
using FIFE::Instance;
using FIFE::Layer;
using FIFE::Location;
using FIFE::ModelCoordinate;
using FIFE::Object;
using FIFE::Point;
using FIFE::RendererNode;
using FIFE::SquareGrid;

namespace
{
    struct RendererNodeFixture
    {
            SquareGrid grid;
            std::unique_ptr<Layer> layer1;
            std::unique_ptr<Layer> layer2;
            std::unique_ptr<Layer> layer3;
            std::unique_ptr<Object> obj;

            RendererNodeFixture() :
                layer1(std::make_unique<Layer>("rn_layer1", nullptr, &grid)),
                layer2(std::make_unique<Layer>("rn_layer2", nullptr, &grid)),
                layer3(std::make_unique<Layer>("rn_layer3", nullptr, &grid)),
                obj(std::make_unique<Object>("rn_obj", "test"))
            {
            }
    };
} // namespace

// ============================================================
// RendererNode::isDrawnOn — stage 1, node-level semantics
// ============================================================

TEST_CASE("RendererNode::isDrawnOn - pure-point node with one active layer", "[renderernode]")
{
    RendererNodeFixture f;
    RendererNode node(Point(3, 4));
    CHECK(node.isDrawnOn(f.layer1.get()));
}

TEST_CASE("RendererNode::isDrawnOn - pure-point node with several active layers", "[renderernode]")
{
    RendererNodeFixture f;
    RendererNode node(Point(3, 4));
    CHECK(node.isDrawnOn(f.layer1.get()));
    CHECK(node.isDrawnOn(f.layer2.get()));
    CHECK(node.isDrawnOn(f.layer3.get()));
}

TEST_CASE("RendererNode::isDrawnOn - layer-attached node", "[renderernode]")
{
    RendererNodeFixture f;
    RendererNode node(f.layer1.get(), Point(7, 8));
    CHECK(node.isDrawnOn(f.layer1.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer2.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer3.get()));
}

TEST_CASE("RendererNode::isDrawnOn - instance-attached node", "[renderernode]")
{
    RendererNodeFixture f;
    Instance* inst = f.layer1->createInstance(f.obj.get(), ModelCoordinate(2, 3, 0));
    RendererNode node(inst, Point(5, 6));
    CHECK(node.isDrawnOn(f.layer1.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer2.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer3.get()));
}

TEST_CASE("RendererNode::isDrawnOn - location-attached node", "[renderernode]")
{
    RendererNodeFixture f;
    Location loc(f.layer1.get());
    loc.setExactLayerCoordinates(ExactModelCoordinate(4, 5));
    RendererNode node(loc, Point(1, 2));
    CHECK(node.isDrawnOn(f.layer1.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer2.get()));
    CHECK_FALSE(node.isDrawnOn(f.layer3.get()));
}
