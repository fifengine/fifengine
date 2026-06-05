// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "mapsaver.h"

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// FIFE includes
#include "model/metamodel/action.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/object.h"
#include "model/model.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/map.h"
#include "model/structures/trigger.h"
#include "model/structures/triggercontroller.h"
#include "savers/native/map/animationsaver.h"
#include "savers/native/map/atlassaver.h"
#include "savers/native/map/objectsaver.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "util/xml/xmlhelper.h"
#include "video/imagemanager.h"
#include "view/camera.h"
#include "view/renderers/lightrenderer.h"
#include "view/visual.h"

namespace FIFE
{
    namespace
    {
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_NATIVE_SAVERS);
            return log;
        }
    } // namespace

    MapSaver::MapSaver() = default;

    MapSaver::~MapSaver() = default;

    void MapSaver::setObjectSaver(FIFE::ObjectSaverPtr const & objectSaver)
    {
        m_objectSaver = objectSaver;
    }

    void MapSaver::setAnimationSaver(FIFE::AnimationSaverPtr const & animationSaver)
    {
        m_animationSaver = animationSaver;
    }

    void MapSaver::setAtlasSaver(FIFE::AtlasSaverPtr const & atlasSaver)
    {
        m_atlasSaver = atlasSaver;
    }

    void MapSaver::save(Map const & map, std::string const & filename, std::vector<std::string> const & importFiles)
    {
        XML::Document doc;
        auto* declaration = doc.NewDeclaration(R"(xml version="1.0" encoding="ascii")");
        doc.InsertEndChild(declaration);

        // add map element
        XML::Element* mapElement = doc.NewElement("map");
        mapElement->SetAttribute("id", map.getName().c_str());
        mapElement->SetAttribute("format", "1.0");
        doc.InsertEndChild(mapElement);

        for (auto const & importFile : importFiles) {
            XML::Element* importElement = doc.NewElement("import");
            importElement->SetAttribute("file", importFile.c_str());

            // link to map element
            mapElement->InsertEndChild(importElement);
        }

        auto layers = map.getLayers();
        for (auto& layer : layers) {
            XML::Element* layerElement = doc.NewElement("layer");
            CellGrid const * grid      = layer->getCellGrid();
            layerElement->SetAttribute("id", layer->getName().c_str());
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
            using InstancesContainer     = std::vector<Instance*>;
            InstancesContainer instances = layer->getInstances();
            for (auto& instance : instances) {
                Object const * obj = instance->getObject();
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

                if (!instance->getName().empty()) {
                    instanceElement->SetAttribute("id", instance->getName().c_str());
                }

                instanceElement->SetAttribute("o", obj->getName().c_str());

                ExactModelCoordinate const position = instance->getLocationRef().getExactLayerCoordinates();
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

                if (instance->isSpecialCost() && (!obj->isSpecialCost() || instance->getCostId() != obj->getCostId() ||
                                                  !Mathd::Equal(instance->getCost(), obj->getCost()))) {
                    instanceElement->SetAttribute("cost_id", instance->getCostId().c_str());
                    instanceElement->SetAttribute("cost", instance->getCost());
                }

                auto* instanceVisual = instance->getVisual<InstanceVisual>();
                instanceElement->SetAttribute("stackpos", instanceVisual->getStackPosition());

                instancesElement->InsertEndChild(instanceElement);
            }

            writeLayerLights(map, *layer, doc, layerElement);
        }
        // add cellcaches tag to document
        XML::Element* cellcachesElement = doc.NewElement("cellcaches");
        mapElement->InsertEndChild(cellcachesElement);
        for (auto& layer : layers) {
            CellCache* cache = layer->getCellCache();
            if (cache == nullptr) {
                continue;
            }
            // add cellcache tag to document
            XML::Element* cellcacheElement = doc.NewElement("cellcache");
            cellcacheElement->SetAttribute("id", layer->getName().c_str());
            cellcacheElement->SetAttribute("default_cost", cache->getDefaultCostMultiplier());
            cellcacheElement->SetAttribute("default_speed", cache->getDefaultSpeedMultiplier());
            cellcacheElement->SetAttribute("search_narrow", cache->isSearchNarrowCells());

            std::set<Cell*> const & narrowCells = cache->getNarrowCells();
            bool const saveNarrows              = !cache->isSearchNarrowCells() && !narrowCells.empty();

            std::vector<std::vector<Cell*>> const & cells = cache->getCells();
            auto it                                       = cells.begin();
            for (; it != cells.end(); ++it) {
                auto cit = (*it).begin();
                for (; cit != (*it).end(); ++cit) {
                    Cell* cell                     = *cit;
                    std::list<std::string> costIds = cache->getCosts();
                    bool const costsEmpty          = costIds.empty();
                    bool const defaultCost         = cell->defaultCost();
                    bool const defaultSpeed        = cell->defaultSpeed();

                    // check if area is part of the cell or object
                    std::vector<std::string> areaIds = cache->getCellAreas(cell);
                    std::vector<std::string> cellAreaIds;
                    bool areasEmpty = areaIds.empty();
                    if (!areasEmpty) {
                        std::set<Instance*> const & cellInstances = cell->getInstances();
                        if (!cellInstances.empty()) {
                            auto area_it = areaIds.begin();
                            for (; area_it != areaIds.end(); ++area_it) {
                                bool objectArea  = false;
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

                    CellTypeInfo const cti            = cell->getCellType();
                    bool const cellBlocker            = (cti != CTYPE_CELL_NO_BLOCKER && cti != CTYPE_CELL_BLOCKER);
                    TransitionInfo const * transition = cell->getTransition();
                    bool isNarrow                     = false;
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
                    ModelCoordinate const cellCoord = cell->getLayerCoordinates();
                    XML::Element* cellElement       = doc.NewElement("cell");
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
                        transitionElement->SetAttribute("id", transition->m_layer->getName().c_str());
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
            for (auto& trigger : triggers) {
                // add trigger tag to document
                XML::Element* triggerElement = doc.NewElement("trigger");
                triggerElement->SetAttribute("name", trigger->getName().c_str());
                triggerElement->SetAttribute("triggered", trigger->isTriggered());
                triggerElement->SetAttribute("all_instances", trigger->isEnabledForAllInstances());
                if (trigger->getAttached() != nullptr) {
                    triggerElement->SetAttribute("attached_instance", trigger->getAttached()->getName().c_str());
                    triggerElement->SetAttribute(
                        "attached_layer", trigger->getAttached()->getLocationRef().getLayer()->getName().c_str());
                }
                std::vector<Cell*> const & cells = trigger->getAssignedCells();
                if (!cells.empty()) {
                    for (auto* cell : cells) {
                        XML::Element* cellElement = doc.NewElement("assign");
                        cellElement->SetAttribute("layer_id", cell->getLayer()->getName().c_str());
                        cellElement->SetAttribute("x", cell->getLayerCoordinates().x);
                        cellElement->SetAttribute("y", cell->getLayerCoordinates().y);
                        triggerElement->InsertEndChild(cellElement);
                    }
                }
                std::vector<Instance*> const & instances = trigger->getEnabledInstances();
                if (!instances.empty()) {
                    for (auto* instance : instances) {
                        XML::Element* instanceElement = doc.NewElement("enabled");
                        instanceElement->SetAttribute(
                            "layer_id", instance->getLocationRef().getLayer()->getName().c_str());
                        instanceElement->SetAttribute("instance_id", instance->getName().c_str());
                        triggerElement->InsertEndChild(instanceElement);
                    }
                }
                std::vector<TriggerCondition> const & conditions = trigger->getTriggerConditions();
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

        auto const & cameras = map.getCameras();
        for (auto& camera : cameras) {
            if (camera->getMap()->getName() == map.getName()) {
                XML::Element* cameraElement = doc.NewElement("camera");

                cameraElement->SetAttribute("id", camera->getName().c_str());
                cameraElement->SetAttribute("zoom", camera->getZoom());
                cameraElement->SetAttribute("tilt", camera->getTilt());
                cameraElement->SetAttribute("rotation", camera->getRotation());
                if (camera->isZToYEnabled()) {
                    cameraElement->SetAttribute("ztoy", camera->getZToY());
                }

                Rect const viewport = camera->getViewPort();
                std::ostringstream viewportString;
                viewportString << viewport.x << "," << viewport.y << "," << viewport.w << "," << viewport.h;

                cameraElement->SetAttribute("viewport", viewportString.str().c_str());

                Point const p = camera->getCellImageDimensions();
                cameraElement->SetAttribute("ref_cell_width", p.x);
                cameraElement->SetAttribute("ref_cell_height", p.y);

                std::vector<float> lightingColor = camera->getLightingColor();
                bool writeLightingColor          = false;

                writeLightingColor = std::ranges::any_of(lightingColor, [](float v) {
                    return v < 1.0F;
                });

                if (writeLightingColor) {
                    std::ostringstream lightingColorString;
                    for (uint32_t i = 0; i < lightingColor.size(); ++i) {
                        if (i > 0) {
                            lightingColorString << ",";
                        }

                        lightingColorString << lightingColor.at(i);
                    }

                    cameraElement->SetAttribute("light_color", lightingColorString.str().c_str());
                }

                mapElement->InsertEndChild(cameraElement);
            }
        }

        doc.SaveFile(filename.c_str());
    }

    void MapSaver::writeLayerLights(
        Map const & map, Layer const & layer, XML::Document& doc, XML::Element* layerElement)
    {
        auto const & cameras = map.getCameras();

        bool hasLights = false;
        for (auto const & camera : cameras) {
            LightRenderer* renderer = LightRenderer::getInstance(camera.get());
            if (renderer == nullptr) {
                continue;
            }

            bool const layerActive = std::any_of(
                renderer->getActiveLayers().begin(), renderer->getActiveLayers().end(), [&](Layer const * activeLayer) {
                    return activeLayer->getName() == layer.getName();
                });
            if (!layerActive) {
                continue;
            }

            if (!renderer->getGroups().empty()) {
                hasLights = true;
                break;
            }
        }

        if (!hasLights) {
            return;
        }

        XML::Element* lightsElement = doc.NewElement("lights");
        layerElement->InsertEndChild(lightsElement);

        for (auto const & camera : cameras) {
            LightRenderer* renderer = LightRenderer::getInstance(camera.get());
            if (renderer == nullptr) {
                continue;
            }

            bool const layerActive = std::any_of(
                renderer->getActiveLayers().begin(), renderer->getActiveLayers().end(), [&](Layer const * activeLayer) {
                    return activeLayer->getName() == layer.getName();
                });
            if (!layerActive) {
                continue;
            }

            for (auto const & group : renderer->getGroups()) {
                for (auto* info : renderer->getLightInfo(group)) {
                    Instance* instance = info->getNode()->getInstance();
                    if (instance == nullptr) {
                        continue;
                    }

                    std::string const type = info->getName();
                    if (type != "simple" && type != "image" && type != "animation") {
                        continue;
                    }

                    XML::Element* lightElement = doc.NewElement("l");
                    lightElement->SetAttribute("group", group.c_str());
                    lightElement->SetAttribute("type", type.c_str());
                    lightElement->SetAttribute("instance", instance->getName().c_str());

                    if (info->getSrcBlend() > -1) {
                        lightElement->SetAttribute("src", info->getSrcBlend());
                    }
                    if (info->getDstBlend() > -1) {
                        lightElement->SetAttribute("dst", info->getDstBlend());
                    }
                    if (info->getStencil() > -1) {
                        lightElement->SetAttribute("s_ref", info->getStencil());
                        lightElement->SetAttribute("a_ref", info->getStencil());
                    }

                    if (type == "simple") {
                        auto* simpleInfo = dynamic_cast<LightRendererSimpleLightInfo*>(info);
                        if (simpleInfo != nullptr) {
                            if (simpleInfo->getRadius() > 0.0) {
                                lightElement->SetAttribute("radius", simpleInfo->getRadius());
                            }
                            std::vector<uint8_t> color = simpleInfo->getColor();
                            if (!color.empty()) {
                                std::ostringstream colorStr;
                                colorStr << static_cast<int>(color.at(0)) << "," << static_cast<int>(color.at(1)) << ","
                                         << static_cast<int>(color.at(2));
                                lightElement->SetAttribute("color", colorStr.str().c_str());
                                lightElement->SetAttribute("intensity", static_cast<int>(color.at(3)));
                            }
                            if (simpleInfo->getSubdivisions() != 32) {
                                lightElement->SetAttribute("subdivisions", simpleInfo->getSubdivisions());
                            }
                            if (simpleInfo->getXStretch() > 1.001 || simpleInfo->getXStretch() < 0.999) {
                                lightElement->SetAttribute("xstretch", simpleInfo->getXStretch());
                            }
                            if (simpleInfo->getYStretch() > 1.001 || simpleInfo->getYStretch() < 0.999) {
                                lightElement->SetAttribute("ystretch", simpleInfo->getYStretch());
                            }
                        }
                    } else if (type == "image") {
                        auto* imageInfo = dynamic_cast<LightRendererImageInfo*>(info);
                        if (imageInfo != nullptr && imageInfo->getImage()) {
                            lightElement->SetAttribute("image", imageInfo->getImage()->getName().c_str());
                        }
                    } else if (type == "animation") {
                        auto* animInfo = dynamic_cast<LightRendererAnimationInfo*>(info);
                        if (animInfo != nullptr && animInfo->getAnimation()) {
                            lightElement->SetAttribute("animation", animInfo->getAnimation()->getName().c_str());
                        }
                    }

                    lightsElement->InsertEndChild(lightElement);
                }
            }
        }
    }

    std::unique_ptr<MapSaver> createDefaultMapSaver(Model* model, ImageManager* imageManager)
    {
        auto saver = std::make_unique<MapSaver>();

        AnimationSaverPtr const animSaver(new AnimationSaver());
        AtlasSaverPtr const atlasSaver(new AtlasSaver());
        ObjectSaverPtr const objSaver(new ObjectSaver(model, imageManager));
        objSaver->setAnimationSaver(animSaver);

        saver->setObjectSaver(objSaver);
        saver->setAnimationSaver(animSaver);
        saver->setAtlasSaver(atlasSaver);

        return saver;
    }
} // namespace FIFE
