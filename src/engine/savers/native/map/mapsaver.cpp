// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <list>
#include <set>
#include <string>
#include <vector>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/object.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/map.h"
#include "model/structures/trigger.h"
#include "model/structures/triggercontroller.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "util/xml/xmlhelper.h"
#include "view/camera.h"
#include "view/visual.h"

#include "mapsaver.h"

namespace FIFE
{
    static Logger _log(LM_NATIVE_SAVERS);

    MapSaver::MapSaver()
    {
        // m_objectSaver = new ObjectSaver();
        // m_animationSaver = new AnimationSaver();
        // m_atlasSaver = new AtlasSaver();
    }

    MapSaver::~MapSaver() = default;

    void MapSaver::setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver)
    {
        m_objectSaver = objectSaver;
    }

    void MapSaver::setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver)
    {
        m_animationSaver = animationSaver;
    }

    void MapSaver::setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver)
    {
        m_atlasSaver = atlasSaver;
    }

    void MapSaver::save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles)
    {
        XML::Document doc;
        auto* declaration = doc.NewDeclaration(R"(xml version="1.0" encoding="ascii")");
        doc.InsertEndChild(declaration);

        // add map element
        XML::Element* mapElement = doc.NewElement("map");
        mapElement->SetAttribute("id", map.getId().c_str());
        mapElement->SetAttribute("format", "1.0");
        doc.InsertEndChild(mapElement);

        for (const auto & importFile : importFiles) {
            XML::Element* importElement = doc.NewElement("import");
            importElement->SetAttribute("file", importFile.c_str());

            // link to map element
            mapElement->InsertEndChild(importElement);
        }

        using LayerList = std::list<Layer*>;
        LayerList layers = map.getLayers();
        for (auto & layer : layers) {
            XML::Element* layerElement = doc.NewElement("layer");
            CellGrid* grid             = layer->getCellGrid();
            layerElement->SetAttribute("id", layer->getId().c_str());
            layerElement->SetAttribute("x_offset", grid->getXShift());
            layerElement->SetAttribute("y_offset", grid->getYShift());
            layerElement->SetAttribute("z_offset", grid->getZShift());
            layerElement->SetAttribute("x_scale", grid->getXScale());
            layerElement->SetAttribute("y_scale", grid->getYScale());
            layerElement->SetAttribute("rotation", grid->getRotation());
            layerElement->SetAttribute("grid_type", grid->getType().c_str());
            layerElement->SetAttribute("transparency", layer->getLayerTransparency());

            std::string pathingStrategy;
            switch (layer->getPathingStrategy()) {
            case CELL_EDGES_ONLY: {
                pathingStrategy = "cell_edges_only";
            } break;
            case CELL_EDGES_AND_DIAGONALS: {
                pathingStrategy = "cell_edges_and_diagonals";
            } break;
            default: {
                pathingStrategy = "cell_edges_only";
            } break;
            }
            layerElement->SetAttribute("pathing", pathingStrategy.c_str());

            std::string sortingStrategy;
            switch (layer->getSortingStrategy()) {
            case SORTING_CAMERA: {
                sortingStrategy = "camera";
            } break;
            case SORTING_LOCATION: {
                sortingStrategy = "location";
            } break;
            case SORTING_CAMERA_AND_LOCATION: {
                sortingStrategy = "camera_and_location";
            } break;
            default: {
                sortingStrategy = "camera";
            } break;
            }
            layerElement->SetAttribute("sorting", sortingStrategy.c_str());

            if (layer->isWalkable()) {
                layerElement->SetAttribute("layer_type", "walkable");
            } else if (layer->isInteract()) {
                layerElement->SetAttribute("layer_type", "interact");
                layerElement->SetAttribute("layer_type_id", layer->getWalkableId().c_str());
            }

            // add layer to document
            mapElement->InsertEndChild(layerElement);

            // add instances tag to document
            XML::Element* instancesElement = doc.NewElement("instances");
            layerElement->InsertEndChild(instancesElement);

            std::string currentNamespace;
            using InstancesContainer = std::vector<Instance*>;
            InstancesContainer instances = layer->getInstances();
            for (auto & instance : instances) {
                Object* obj = instance->getObject();
                // don't save part instances
                if (obj->isMultiPart()) {
                    continue;
                }

                // create instance element
                XML::Element* instanceElement = doc.NewElement("i");

                if (!obj->getNamespace().empty() && currentNamespace != obj->getNamespace()) {
                    instanceElement->SetAttribute("ns", obj->getNamespace().c_str());

                    // update current namespace
                    currentNamespace = obj->getNamespace();
                }

                if (!instance->getId().empty()) {
                    instanceElement->SetAttribute("id", instance->getId().c_str());
                }

                instanceElement->SetAttribute("o", obj->getId().c_str());

                ExactModelCoordinate position = instance->getLocationRef().getExactLayerCoordinates();
                instanceElement->SetAttribute("x", position.x);
                instanceElement->SetAttribute("y", position.y);
                instanceElement->SetAttribute("z", position.z);
                instanceElement->SetAttribute("r", instance->getRotation());

                if (instance->isBlocking()) {
                    instanceElement->SetAttribute("blocking", instance->isBlocking());
                }

                if (instance->getCellStackPosition() != obj->getCellStackPosition()) {
                    instanceElement->SetAttribute("cellstack", instance->getCellStackPosition());
                }

                if (instance->isSpecialCost()) {
                    if (!obj->isSpecialCost()) {
                        instanceElement->SetAttribute("cost_id", instance->getCostId().c_str());
                        instanceElement->SetAttribute("cost", instance->getCost());
                    } else if (
                        instance->getCostId() != obj->getCostId() || !Mathd::Equal(instance->getCost(), obj->getCost())) {
                        instanceElement->SetAttribute("cost_id", instance->getCostId().c_str());
                        instanceElement->SetAttribute("cost", instance->getCost());
                    }
                }

                auto* instanceVisual = instance->getVisual<InstanceVisual>();
                instanceElement->SetAttribute("stackpos", instanceVisual->getStackPosition());

                instancesElement->InsertEndChild(instanceElement);
            }
        }
        // add cellcaches tag to document
        XML::Element* cellcachesElement = doc.NewElement("cellcaches");
        mapElement->InsertEndChild(cellcachesElement);
        for (auto & layer : layers) {
            CellCache* cache = layer->getCellCache();
            if (cache == nullptr) {
                continue;
            }
            // add cellcache tag to document
            XML::Element* cellcacheElement = doc.NewElement("cellcache");
            cellcacheElement->SetAttribute("id", layer->getId().c_str());
            cellcacheElement->SetAttribute("default_cost", cache->getDefaultCostMultiplier());
            cellcacheElement->SetAttribute("default_speed", cache->getDefaultSpeedMultiplier());
            cellcacheElement->SetAttribute("search_narrow", cache->isSearchNarrowCells());

            const std::set<Cell*>& narrowCells = cache->getNarrowCells();
            bool saveNarrows                   = !cache->isSearchNarrowCells() && !narrowCells.empty();

            const std::vector<std::vector<Cell*>>& cells       = cache->getCells();
            auto it = cells.begin();
            for (; it != cells.end(); ++it) {
                auto cit = (*it).begin();
                for (; cit != (*it).end(); ++cit) {
                    Cell* cell                     = *cit;
                    std::list<std::string> costIds = cache->getCosts();
                    bool costsEmpty                = costIds.empty();
                    bool defaultCost               = cell->defaultCost();
                    bool defaultSpeed              = cell->defaultSpeed();

                    // check if area is part of the cell or object
                    std::vector<std::string> areaIds = cache->getCellAreas(cell);
                    std::vector<std::string> cellAreaIds;
                    bool areasEmpty = areaIds.empty();
                    if (!areasEmpty) {
                        const std::set<Instance*>& cellInstances = cell->getInstances();
                        if (!cellInstances.empty()) {
                            auto area_it = areaIds.begin();
                            for (; area_it != areaIds.end(); ++area_it) {
                                bool objectArea                                 = false;
                                auto instance_it = cellInstances.begin();
                                for (; instance_it != cellInstances.end(); ++instance_it) {
                                    if ((*instance_it)->getObject()->getArea() == *area_it) {
                                        objectArea = true;
                                        break;
                                    }
                                }
                                if (!objectArea) {
                                    cellAreaIds.push_back(*area_it);
                                }
                            }
                        } else {
                            cellAreaIds = areaIds;
                        }
                        areasEmpty = cellAreaIds.empty();
                    }

                    CellTypeInfo cti           = cell->getCellType();
                    bool cellBlocker           = (cti != CTYPE_CELL_NO_BLOCKER && cti != CTYPE_CELL_BLOCKER);
                    TransitionInfo* transition = cell->getTransition();
                    bool isNarrow              = false;
                    if (saveNarrows) {
                        auto narrow_it = narrowCells.find(cell);
                        if (narrow_it != narrowCells.end()) {
                            isNarrow = true;
                        }
                    }
                    if (costsEmpty && defaultCost && defaultSpeed && areasEmpty && cellBlocker &&
                        (transition == nullptr) && !isNarrow) {
                        continue;
                    }
                    // add cell tag to document
                    ModelCoordinate cellCoord = cell->getLayerCoordinates();
                    XML::Element* cellElement = doc.NewElement("cell");
                    cellElement->SetAttribute("x", cellCoord.x);
                    cellElement->SetAttribute("y", cellCoord.y);
                    if (!defaultCost) {
                        cellElement->SetAttribute("default_cost", cell->getCostMultiplier());
                    }
                    if (!defaultSpeed) {
                        cellElement->SetAttribute("default_speed", cell->getSpeedMultiplier());
                    }

                    if (!cellBlocker) {
                        if (cti == CTYPE_CELL_NO_BLOCKER) {
                            cellElement->SetAttribute("blocker_type", "no_blocker");
                        } else {
                            cellElement->SetAttribute("blocker_type", "blocker");
                        }
                    }
                    if (isNarrow) {
                        cellElement->SetAttribute("narrow", true);
                    }
                    // add cost tag
                    if (!costsEmpty) {
                        auto cost_it = costIds.begin();
                        for (; cost_it != costIds.end(); ++cost_it) {
                            if (cache->existsCostForCell(*cost_it, cell)) {
                                XML::Element* costElement = doc.NewElement("cost");
                                costElement->SetAttribute("id", cost_it->c_str());
                                costElement->SetAttribute("value", cache->getCost(*cost_it));
                                cellElement->InsertEndChild(costElement);
                            }
                        }
                    }
                    // add area tag
                    if (!areasEmpty) {
                        auto area_it = cellAreaIds.begin();
                        for (; area_it != cellAreaIds.end(); ++area_it) {
                            XML::Element* areaElement = doc.NewElement("area");
                            areaElement->SetAttribute("id", area_it->c_str());
                            cellElement->InsertEndChild(areaElement);
                        }
                    }
                    // add transition tag
                    if (transition != nullptr) {
                        XML::Element* transitionElement = doc.NewElement("transition");
                        transitionElement->SetAttribute("id", transition->m_layer->getId().c_str());
                        transitionElement->SetAttribute("x", transition->m_mc.x);
                        transitionElement->SetAttribute("y", transition->m_mc.y);
                        if (transition->m_mc.z != 0) {
                            transitionElement->SetAttribute("z", transition->m_mc.z);
                        }
                        if (transition->m_immediate) {
                            transitionElement->SetAttribute("immediate", true);
                        } else {
                            transitionElement->SetAttribute("immediate", false);
                        }
                        cellElement->InsertEndChild(transitionElement);
                    }
                    cellcacheElement->InsertEndChild(cellElement);
                }
            }
            cellcachesElement->InsertEndChild(cellcacheElement);
        }

        TriggerController* triggerController = map.getTriggerController();
        std::vector<Trigger*> triggers       = triggerController->getAllTriggers();
        if (!triggers.empty()) {
            // add triggers tag to document
            XML::Element* triggersElement = doc.NewElement("triggers");
            mapElement->InsertEndChild(triggersElement);
            for (auto & trigger : triggers) {
                // add trigger tag to document
                XML::Element* triggerElement = doc.NewElement("trigger");
                triggerElement->SetAttribute("name", trigger->getName().c_str());
                triggerElement->SetAttribute("triggered", trigger->isTriggered());
                triggerElement->SetAttribute("all_instances", trigger->isEnabledForAllInstances());
                if (trigger->getAttached() != nullptr) {
                    triggerElement->SetAttribute("attached_instance", trigger->getAttached()->getId().c_str());
                    triggerElement->SetAttribute(
                        "attached_layer", trigger->getAttached()->getLocationRef().getLayer()->getId().c_str());
                }
                const std::vector<Cell*>& cells = trigger->getAssignedCells();
                if (!cells.empty()) {
                    for (auto cell : cells) {
                        XML::Element* cellElement = doc.NewElement("assign");
                        cellElement->SetAttribute("layer_id", cell->getLayer()->getId().c_str());
                        cellElement->SetAttribute("x", cell->getLayerCoordinates().x);
                        cellElement->SetAttribute("y", cell->getLayerCoordinates().y);
                        triggerElement->InsertEndChild(cellElement);
                    }
                }
                const std::vector<Instance*>& instances = trigger->getEnabledInstances();
                if (!instances.empty()) {
                    for (auto instance : instances) {
                        XML::Element* instanceElement = doc.NewElement("enabled");
                        instanceElement->SetAttribute(
                            "layer_id", instance->getLocationRef().getLayer()->getId().c_str());
                        instanceElement->SetAttribute("instance_id", instance->getId().c_str());
                        triggerElement->InsertEndChild(instanceElement);
                    }
                }
                const std::vector<TriggerCondition>& conditions = trigger->getTriggerConditions();
                if (!conditions.empty()) {
                    for (auto condition : conditions) {
                        XML::Element* conditionElement = doc.NewElement("condition");
                        conditionElement->SetAttribute("id", condition);
                        triggerElement->InsertEndChild(conditionElement);
                    }
                }
                triggersElement->InsertEndChild(triggerElement);
            }
        }

        using CameraContainer = std::vector<Camera*>;
        CameraContainer cameras = map.getCameras();
        for (auto & camera : cameras) {
            if (camera->getMap()->getId() == map.getId()) {
                XML::Element* cameraElement = doc.NewElement("camera");

                cameraElement->SetAttribute("id", camera->getId().c_str());
                cameraElement->SetAttribute("zoom", camera->getZoom());
                cameraElement->SetAttribute("tilt", camera->getTilt());
                cameraElement->SetAttribute("rotation", camera->getRotation());
                if (camera->isZToYEnabled()) {
                    cameraElement->SetAttribute("ztoy", camera->getZToY());
                }

                Rect viewport = camera->getViewPort();
                std::ostringstream viewportString;
                viewportString << viewport.x << "," << viewport.y << "," << viewport.w << "," << viewport.h;

                cameraElement->SetAttribute("viewport", viewportString.str().c_str());

                Point p = camera->getCellImageDimensions();
                cameraElement->SetAttribute("ref_cell_width", p.x);
                cameraElement->SetAttribute("ref_cell_height", p.y);

                std::vector<float> lightingColor = camera->getLightingColor();
                bool writeLightingColor          = false;
                for (float i : lightingColor) {
                    if (i < 1.0) {
                        writeLightingColor = true;
                        break;
                    }
                }

                if (writeLightingColor) {
                    std::ostringstream lightingColorString;
                    for (uint32_t i = 0; i < lightingColor.size(); ++i) {
                        if (i > 0) {
                            lightingColorString << ",";
                        }

                        lightingColorString << lightingColor[i];
                    }

                    cameraElement->SetAttribute("light_color", lightingColorString.str().c_str());
                }

                mapElement->InsertEndChild(cameraElement);
            }
        }

        doc.SaveFile(filename.c_str());
    }
} // namespace FIFE
