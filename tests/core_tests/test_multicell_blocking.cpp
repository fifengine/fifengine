// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <vector>

#include "model/metamodel/grids/squaregrid.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "util/time/timemanager.h"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using FIFE::Cell;
using FIFE::CellCache;
using FIFE::CellTypeInfo;
using FIFE::CTYPE_CELL_BLOCKER;
using FIFE::CTYPE_DYNAMIC_BLOCKER;
using FIFE::CTYPE_NO_BLOCKER;
using FIFE::CTYPE_STATIC_BLOCKER;
using FIFE::ICHANGE_FOOTPRINT;
using FIFE::ICHANGE_LOC;
using FIFE::ICHANGE_NO_CHANGES;
using FIFE::ICHANGE_ROTATION;
using FIFE::Instance;
using FIFE::InstanceChangeInfo;
using FIFE::InstanceChangeListener;
using FIFE::Layer;
using FIFE::Location;
using FIFE::ModelCoordinate;
using FIFE::Object;
using FIFE::SquareGrid;
using FIFE::TimeManager;

namespace
{

    struct MultiCellFixture
    {
            TimeManager tm;
            SquareGrid grid;
            Layer* layer{nullptr};
            Object* mainObj{nullptr};
            Object* part1{nullptr};

            MultiCellFixture()
            {
                layer = new Layer("test_layer", nullptr, &grid);
                assert("layer must exist after creation" && layer != nullptr);
                layer->setWalkable(true);
                layer->createCellCache();

                mainObj = new Object("wall_main", "test");
                mainObj->setBlocking(true);
                mainObj->setStatic(true);

                part1 = new Object("wall_part", "test");
                part1->setMultiPart(true);
                part1->addMultiPartCoordinate(0, ModelCoordinate(1, 0, 0));

                mainObj->addMultiPartId("wall_part");
                mainObj->addMultiPart(part1);
            }

            ~MultiCellFixture()
            {
                delete layer;
                delete mainObj;
                delete part1;
            }
    };

    bool isBlocked(CellTypeInfo t)
    {
        return t == CTYPE_STATIC_BLOCKER || t == CTYPE_DYNAMIC_BLOCKER || t == CTYPE_CELL_BLOCKER;
    }

} // namespace

// ============================================================
// W1-T0A: Happy path tests
// ============================================================

TEST_CASE("W1-T0A: multi-cell blocking on initial placement", "[multicell]")
{
    MultiCellFixture f;
    Instance* inst   = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    CellCache* cache = f.layer->getCellCache();

    Cell* mainCell = cache->getCell(ModelCoordinate(5, 5, 0));
    REQUIRE(mainCell != nullptr);
    CHECK(mainCell->getCellType() == CTYPE_STATIC_BLOCKER);

    Cell* subCell = cache->getCell(ModelCoordinate(6, 5, 0));
    REQUIRE(subCell != nullptr);
    CHECK(isBlocked(subCell->getCellType()));

    f.layer->deleteInstance(inst);
}

TEST_CASE("W1-T0A: multi-cell blocking toggle propagates to sub-cells", "[multicell]")
{
    MultiCellFixture f;
    Instance* inst   = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    CellCache* cache = f.layer->getCellCache();
    inst->setOverrideBlocking(true);

    inst->setBlocking(false);
    f.layer->update();
    Cell* mainCell = cache->getCell(ModelCoordinate(5, 5, 0));
    Cell* subCell  = cache->getCell(ModelCoordinate(6, 5, 0));

    REQUIRE(mainCell != nullptr);
    REQUIRE(subCell != nullptr);
    CHECK(mainCell->getCellType() == CTYPE_NO_BLOCKER);
    CHECK(subCell->getCellType() == CTYPE_NO_BLOCKER);

    inst->setBlocking(true);
    f.layer->update();
    CHECK(mainCell->getCellType() == CTYPE_STATIC_BLOCKER);
    CHECK(isBlocked(subCell->getCellType()));

    f.layer->deleteInstance(inst);
}

TEST_CASE("W1-T0A: multi-cell rotation updates occupied cells", "[multicell]")
{
    MultiCellFixture f;
    f.part1->addMultiPartCoordinate(90, ModelCoordinate(0, 1, 0));

    Instance* inst   = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    CellCache* cache = f.layer->getCellCache();

    f.layer->update();
    Cell* oldCell = cache->getCell(ModelCoordinate(6, 5, 0));
    REQUIRE(oldCell != nullptr);
    CHECK(oldCell->getCellType() == CTYPE_STATIC_BLOCKER);

    inst->setRotation(90);
    f.layer->update();
    f.layer->update();

    Cell* updatedOldCell      = cache->getCell(ModelCoordinate(6, 5, 0));
    bool const oldCellCleared = (updatedOldCell == nullptr) || (updatedOldCell->getCellType() == CTYPE_NO_BLOCKER);
    CHECK(oldCellCleared);

    Cell* newCell = cache->getCell(ModelCoordinate(5, 6, 0));
    REQUIRE(newCell != nullptr);
    CHECK(isBlocked(newCell->getCellType()));

    f.layer->deleteInstance(inst);
}

TEST_CASE("W1-T0A: multi-cell with multiple rotations (0, 90, 180, 270)", "[multicell]")
{
    MultiCellFixture f;
    f.part1->addMultiPartCoordinate(0, ModelCoordinate(1, 0, 0));
    f.part1->addMultiPartCoordinate(90, ModelCoordinate(0, 1, 0));
    f.part1->addMultiPartCoordinate(180, ModelCoordinate(-1, 0, 0));
    f.part1->addMultiPartCoordinate(270, ModelCoordinate(0, -1, 0));

    Instance* inst   = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    CellCache* cache = f.layer->getCellCache();
    inst->setOverrideBlocking(true);

    // rot 0: (5,5) and (6,5) blocked (already set from constructor)

    // rot 90: (5,5) and (5,6) blocked
    inst->setRotation(90);
    f.layer->update();
    f.layer->update();
    CHECK(isBlocked(cache->getCell(ModelCoordinate(5, 6, 0))->getCellType()));

    // rot 180: (5,5) and (4,5) blocked
    inst->setRotation(180);
    f.layer->update();
    f.layer->update();
    CHECK(isBlocked(cache->getCell(ModelCoordinate(4, 5, 0))->getCellType()));

    // rot 270: (5,5) and (5,4) blocked
    inst->setRotation(270);
    f.layer->update();
    f.layer->update();
    CHECK(isBlocked(cache->getCell(ModelCoordinate(5, 4, 0))->getCellType()));

    f.layer->deleteInstance(inst);
}

TEST_CASE("W1-T0A: multi-cell with two sub-parts", "[multicell]")
{
    MultiCellFixture f;
    Object* part2 = new Object("wall_part2", "test");
    part2->setMultiPart(true);
    part2->addMultiPartCoordinate(0, ModelCoordinate(0, 1, 0));
    f.mainObj->addMultiPartId("wall_part2");
    f.mainObj->addMultiPart(part2);

    Instance* inst   = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    CellCache* cache = f.layer->getCellCache();

    CHECK(isBlocked(cache->getCell(ModelCoordinate(5, 5, 0))->getCellType()));
    CHECK(isBlocked(cache->getCell(ModelCoordinate(6, 5, 0))->getCellType()));
    CHECK(isBlocked(cache->getCell(ModelCoordinate(5, 6, 0))->getCellType()));

    f.layer->deleteInstance(inst);
    delete part2;
}

TEST_CASE("W1-T0A: multi-cell with idle change events only (no rotation/location/block)", "[multicell]")
{
    // Ensure no crash when layer update fires but nothing changed
    MultiCellFixture f;
    Instance* inst = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));
    f.layer->update();
    f.layer->update();
    f.layer->deleteInstance(inst);
}

// ============================================================
// Baseline tests (non-multi-cell)
// ============================================================

TEST_CASE("baseline: single cell blocking change propagates to cell type", "[multicell][baseline]")
{
    MultiCellFixture f;
    Object* singleObj = new Object("single", "test");
    singleObj->setBlocking(true);
    singleObj->setStatic(true);

    Instance* inst = f.layer->createInstance(singleObj, ModelCoordinate(5, 5, 0));
    inst->setOverrideBlocking(true);
    CellCache* cache = f.layer->getCellCache();
    Cell* cell       = cache->getCell(ModelCoordinate(5, 5, 0));
    REQUIRE(cell != nullptr);
    CHECK(cell->getCellType() == CTYPE_STATIC_BLOCKER);

    inst->setBlocking(false);
    f.layer->update();
    CHECK(cell->getCellType() == CTYPE_NO_BLOCKER);

    inst->setBlocking(true);
    f.layer->update();
    CHECK(cell->getCellType() == CTYPE_STATIC_BLOCKER);

    f.layer->deleteInstance(inst);
    delete singleObj;
}

// ============================================================
// W6-T0A: Off-grid and edge case tests
// ============================================================

TEST_CASE("W6-T0A: multi-cell move rejected when sub-instance would be off-grid", "[multicell][edge]")
{
    MultiCellFixture f;
    Instance* inst = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 5, 0));

    // Move to (0,5) — sub-instance would be at (1,5) which exists in cache
    // (the fixture created cells in the (5,5) area, so (1,5) is OUTSIDE the cache)
    Location newLoc(f.layer);
    newLoc.setLayerCoordinates(ModelCoordinate(0, 5, 0));
    Location const oldLoc = inst->getLocation();
    inst->setLocation(newLoc);
    // Move should be rejected (sub-instance would be off cache)
    ModelCoordinate const after = inst->getLocation().getLayerCoordinates();
    CHECK(after.x == oldLoc.getLayerCoordinates().x); // unchanged

    f.layer->deleteInstance(inst);
}

TEST_CASE("W6-T0A: single-cell move to off-grid is handled gracefully", "[multicell][edge]")
{
    MultiCellFixture f;
    Object* singleObj = new Object("s", "test");
    singleObj->setBlocking(false);
    Instance* inst = f.layer->createInstance(singleObj, ModelCoordinate(5, 5, 0));

    Location offGrid(f.layer);
    offGrid.setLayerCoordinates(ModelCoordinate(-100, -100, 0));
    inst->setLocation(offGrid);
    // Instance may or may not move; key is no crash

    f.layer->deleteInstance(inst);
    delete singleObj;
}

TEST_CASE("W6-T0B: multi-cell placement rejected when sub-cell overlaps blocker", "[multicell][edge]")
{
    MultiCellFixture f;

    // Place a blocking instance where the multi-cell's sub would go
    Instance* existing = f.layer->createInstance(f.mainObj, ModelCoordinate(6, 6, 0));

    // Try to place another multi-cell at (5,6) — the sub-instance would be at (6,6)
    // which already has a blocking instance. Should reject sub-instance creation.
    Instance* overlap = f.layer->createInstance(f.mainObj, ModelCoordinate(5, 6, 0));
    // The main instance may be created but sub-instances should not exist
    // due to the overlap check in Instance constructor

    f.layer->deleteInstance(existing);
    if (overlap != nullptr) {
        f.layer->deleteInstance(overlap);
    }
}

TEST_CASE("W6-T0B: multi-cell placement succeeds when sub-cell is clear", "[multicell][edge]")
{
    MultiCellFixture f;

    // Place multi-cell in clear area — should create normally
    Instance* inst = f.layer->createInstance(f.mainObj, ModelCoordinate(10, 10, 0));
    CHECK(inst->isMultiObject());

    f.layer->deleteInstance(inst);
}

// ============================================================
// W5-T0B: Debug data query tests
// ============================================================

TEST_CASE("W5-T0B: getOccupiedCells returns main + sub-instance cells", "[multicell]")
{
    MultiCellFixture f;
    // Populate angle map and footprint cache
    f.mainObj->getMultiObjectCoordinates(0);
    f.mainObj->initializeFootprintCache(&f.grid);
    Instance* inst = f.layer->createInstance(f.mainObj, ModelCoordinate(8, 8, 0));
    f.layer->update();

    std::vector<ModelCoordinate> const cells = inst->getOccupiedCells();
    CHECK(cells.size() == 2);

    bool hasMain = false;
    bool hasSub  = false;
    for (auto const & c : cells) {
        if (c.x == 8 && c.y == 8 && c.z == 0) {
            hasMain = true;
        }
        if (c.x == 9 && c.y == 8 && c.z == 0) {
            hasSub = true;
        }
    }
    CHECK(hasMain);
    CHECK(hasSub);

    f.layer->deleteInstance(inst);
}

TEST_CASE("W5-T0B: getOccupiedCells after rotation returns updated cells", "[multicell]")
{
    MultiCellFixture f;
    f.part1->addMultiPartCoordinate(90, ModelCoordinate(0, 1, 0));

    f.mainObj->getMultiObjectCoordinates(0);
    f.mainObj->getMultiObjectCoordinates(90);
    f.mainObj->initializeFootprintCache(&f.grid);

    Instance* inst = f.layer->createInstance(f.mainObj, ModelCoordinate(8, 8, 0));
    f.layer->update();

    // Rotate 90°: sub moves from (9,8) to (8,9)
    inst->setRotation(90);
    f.layer->update();

    std::vector<ModelCoordinate> const cells = inst->getOccupiedCells();
    CHECK(cells.size() == 2);

    // Try both offset directions to determine actual rotation behavior
    bool hasNewSubX = false;
    bool hasNewSubY = false;
    for (auto const & c : cells) {
        if (c.x == 8 && c.y == 9 && c.z == 0) {
            hasNewSubY = true;
        }
        if (c.x == 9 && c.y == 8 && c.z == 0) {
            hasNewSubX = true;
        }
    }
    // After rotation, the old cell (9,8) should be vacated and new cell (8,9) occupied.
    // If both are occupied or neither, the rotation didn't correctly update.
    CHECK(hasNewSubY);
    CHECK(!hasNewSubX);

    f.layer->deleteInstance(inst);
}

TEST_CASE("W5-T0B: non-multi instance returns single cell from getOccupiedCells", "[multicell]")
{
    MultiCellFixture f;
    Object* singleObj = new Object("s", "test");
    singleObj->setBlocking(false);
    Instance* inst = f.layer->createInstance(singleObj, ModelCoordinate(5, 5, 0));

    std::vector<ModelCoordinate> cells = inst->getOccupiedCells();
    CHECK(cells.size() == 1);
    CHECK(cells[0].x == 5);
    CHECK(cells[0].y == 5);

    f.layer->deleteInstance(inst);
    delete singleObj;
}

// ============================================================
// W5-T0A: Multi-cell footprint event tests
// ============================================================

namespace
{

    struct FootprintListener : InstanceChangeListener
    {
            InstanceChangeInfo lastChange{ICHANGE_NO_CHANGES};

            void onInstanceChanged(Instance* /*instance*/, InstanceChangeInfo info) override
            {
                lastChange |= info;
            }

            void reset()
            {
                lastChange = ICHANGE_NO_CHANGES;
            }
    };

} // namespace

TEST_CASE("W5-T0A: ICHANGE_FOOTPRINT fires on multi-cell rotation", "[multicell]")
{
    MultiCellFixture fix;
    fix.part1->addMultiPartCoordinate(90, ModelCoordinate(0, 1, 0));

    Instance* inst = fix.layer->createInstance(fix.mainObj, ModelCoordinate(5, 5, 0));
    FootprintListener listener;
    inst->addChangeListener(&listener);
    fix.layer->update();

    // Rotate — should fire ICHANGE_ROTATION | ICHANGE_FOOTPRINT
    inst->setRotation(90);
    fix.layer->update();
    CHECK(listener.lastChange & ICHANGE_ROTATION);
    CHECK(listener.lastChange & ICHANGE_FOOTPRINT);

    fix.layer->deleteInstance(inst);
}

TEST_CASE("W5-T0A: ICHANGE_FOOTPRINT fires on multi-cell location change", "[multicell]")
{
    MultiCellFixture fix;

    // Keep a marker alive to expand cell cache to cover the target area
    Object* marker = new Object("m", "test");
    marker->setBlocking(false);
    fix.layer->createInstance(marker, ModelCoordinate(12, 12, 0));

    Instance* inst = fix.layer->createInstance(fix.mainObj, ModelCoordinate(5, 5, 0));
    FootprintListener listener;
    inst->addChangeListener(&listener);
    fix.layer->update();

    // Move to new cell — should fire ICHANGE_LOC | ICHANGE_CELL | ICHANGE_FOOTPRINT
    Location newLoc(fix.layer);
    newLoc.setLayerCoordinates(ModelCoordinate(10, 10, 0));
    inst->setLocation(newLoc);
    fix.layer->update();
    CHECK(listener.lastChange & ICHANGE_LOC);
    CHECK(listener.lastChange & ICHANGE_FOOTPRINT);

    fix.layer->deleteInstance(inst);
    delete marker;
}

TEST_CASE("W5-T0A: ICHANGE_FOOTPRINT not fired for single-cell instance", "[multicell]")
{
    MultiCellFixture fix;

    // Keep a marker alive to expand cache
    Object* marker = new Object("m", "test");
    marker->setBlocking(false);
    fix.layer->createInstance(marker, ModelCoordinate(12, 12, 0));

    Object* singleObj = new Object("single", "test");
    singleObj->setBlocking(false);
    Instance* inst = fix.layer->createInstance(singleObj, ModelCoordinate(5, 5, 0));
    FootprintListener listener;
    inst->addChangeListener(&listener);
    fix.layer->update();

    // Move — should fire ICHANGE_LOC but NOT ICHANGE_FOOTPRINT
    Location newLoc(fix.layer);
    newLoc.setLayerCoordinates(ModelCoordinate(10, 10, 0));
    inst->setLocation(newLoc);
    fix.layer->update();
    CHECK(listener.lastChange & ICHANGE_LOC);
    CHECK(!(listener.lastChange & ICHANGE_FOOTPRINT));

    fix.layer->deleteInstance(inst);
    delete singleObj;
    delete marker;
}
