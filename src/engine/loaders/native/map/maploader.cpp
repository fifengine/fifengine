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
#include "model/metamodel/action.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/modelcoords.h"
#include "model/model.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/trigger.h"
#include "model/structures/triggercontroller.h"
#include "util/base/exception.h"
#include "util/base/stringutils.h"
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/structures/rect.h"
#include "util/xml/xmlhelper.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "video/animationmanager.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"
#include "view/camera.h"
#include "view/renderers/instancerenderer.h"
#include "view/visual.h"

#include "animationloader.h"
#include "atlasloader.h"
#include "maploader.h"
#include "objectloader.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_NATIVE_LOADERS);

    MapLoader::MapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend) :
        m_model(model),
        m_vfs(vfs),
        m_imageManager(imageManager),
        m_animationManager(AnimationManager::instance()),
        m_renderBackend(renderBackend),
        m_loaderName("fife")

    {
        AnimationLoaderPtr animationLoader(new AnimationLoader(m_vfs, m_imageManager, m_animationManager));
        AtlasLoaderPtr atlasLoader(new AtlasLoader(m_model, m_vfs, m_imageManager, m_animationManager));
        m_objectLoader.reset(
            new ObjectLoader(m_model, m_vfs, m_imageManager, m_animationManager, animationLoader, atlasLoader));
    }

    MapLoader::~MapLoader() = default;

    Map* MapLoader::load(const std::string& filename)
    {
        Map* map = nullptr;

        // reset percent done listener just in case
        // it has residual data from last load
        m_percentDoneListener.reset();

        fs::path mapPath(filename);

        if (HasParentPath(mapPath)) {
            if (GetParentPath(mapPath).string() != m_mapDirectory) {
                // save the directory where the map file is located
                m_mapDirectory = GetParentPath(mapPath).string();
            }
        }

        XML::Document mapFile;

        std::string mapFilename = mapPath.string();

        try {
            RawData* data = m_vfs->open(mapFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(mapFile, xml)) {
                        std::ostringstream oss;
                        oss << " Failed to load" << mapFilename << " : " << __FILE__ << " [" << __LINE__ << "]" << '\n';
                        FL_ERR(_log, oss.str());

                        delete data;
                        return map;
                    }
                }

                // done with data delete resource
                delete data;
                data = nullptr;
            }
        } catch (NotFound& e) {
            FL_ERR(_log, e.what());

            // TODO - should we abort here
            //        or rethrow the exception
            //        or just keep going

            return map;
        }

        // if we get here then everything loaded properly
        // so we can just parse out the contents
        const XML::Element* root = mapFile.RootElement();

        if (root != nullptr) {
            const char* loaderName = XML::Attribute(root, "loaderName");

            if (loaderName != nullptr) {
                m_loaderName = loaderName;
            }

            int numElements = 0;
            XML::QueryAttribute(root, "elements", &numElements);
            m_percentDoneListener.setTotalNumberOfElements(numElements);

            const char* mapName = XML::Attribute(root, "id");

            if (mapName != nullptr) {
                try {
                    map = m_model->createMap(mapName);
                } catch (NameClash& e) {
                    FL_ERR(_log, e.what());

                    // just rethrow to client
                    throw;
                }

                if (map != nullptr) {
                    map->setFilename(mapFilename);

                    std::string ns;
                    for (const XML::Element* importElement = root->FirstChildElement("import");
                         importElement != nullptr;
                         importElement = importElement->NextSiblingElement("import")) {
                        const char* importDir  = XML::Attribute(importElement, "dir");
                        const char* importFile = XML::Attribute(importElement, "file");

                        std::string directory;
                        if (importDir != nullptr) {
                            directory = importDir;
                        }

                        std::string file;
                        if (importFile != nullptr) {
                            file = importFile;
                        }

                        if ((importDir != nullptr) && (importFile == nullptr)) {
                            fs::path fullPath(m_mapDirectory);
                            fullPath /= directory;
                            loadImportDirectory(fullPath.string());
                        } else if (importFile != nullptr) {
                            fs::path fullFilePath(file);
                            fs::path fullDirPath(directory);
                            if (importDir != nullptr) {
                                fullDirPath = fs::path(m_mapDirectory);
                                fullDirPath /= directory;
                            } else {
                                fullFilePath = fs::path(m_mapDirectory);
                                fullFilePath /= file;
                            }
                            loadImportFile(fullFilePath.string(), fullDirPath.string());
                        }
                    }
                    // converts multiobject part id to object pointer
                    std::list<std::string> namespaces = m_model->getNamespaces();
                    auto name_it                      = namespaces.begin();
                    for (; name_it != namespaces.end(); ++name_it) {
                        std::list<Object*> objects = m_model->getObjects(*name_it);
                        auto object_it             = objects.begin();
                        for (; object_it != objects.end(); ++object_it) {
                            if ((*object_it)->isMultiObject()) {
                                const std::list<std::string>& multiParts = (*object_it)->getMultiPartIds();
                                auto multi_it                            = multiParts.begin();
                                for (; multi_it != multiParts.end(); ++multi_it) {
                                    Object* partObj = m_model->getObject(*multi_it, *name_it);
                                    if (partObj != nullptr) {
                                        partObj->setMultiPart(true);
                                        (*object_it)->addMultiPart(partObj);
                                    }
                                }
                            }
                        }
                    }

                    // iterate over elements looking for layers
                    for (const XML::Element* layerElement = root->FirstChildElement("layer"); layerElement != nullptr;
                         layerElement                     = layerElement->NextSiblingElement("layer")) {
                        // defaults
                        double xOffset  = 0.0;
                        double yOffset  = 0.0;
                        double zOffset  = 0.0;
                        double xScale   = 1.0;
                        double yScale   = 1.0;
                        double zScale   = 1.0;
                        double rotation = 0.0;

                        int xOffsetRetVal = XML::QueryAttribute(layerElement, "x_offset", &xOffset);
                        int yOffsetRetVal = XML::QueryAttribute(layerElement, "y_offset", &yOffset);
                        XML::QueryAttribute(layerElement, "z_offset", &zOffset);
                        int xScaleRetVal = XML::QueryAttribute(layerElement, "x_scale", &xScale);
                        int yScaleRetVal = XML::QueryAttribute(layerElement, "y_scale", &yScale);
                        XML::QueryAttribute(layerElement, "z_scale", &zScale);
                        int rotationRetVal = XML::QueryAttribute(layerElement, "rotation", &rotation);

                        const char* layerName     = XML::Attribute(layerElement, "id");
                        const char* pathing       = XML::Attribute(layerElement, "pathing");
                        const char* sorting       = XML::Attribute(layerElement, "sorting");
                        const char* gridType      = XML::Attribute(layerElement, "grid_type");
                        const char* layerType     = XML::Attribute(layerElement, "layer_type");
                        const char* layerTypeName = XML::Attribute(layerElement, "layer_type_id");

                        if (xOffsetRetVal == XML::SUCCESS && yOffsetRetVal == XML::SUCCESS &&
                            xScaleRetVal == XML::SUCCESS && yScaleRetVal == XML::SUCCESS &&
                            rotationRetVal == XML::SUCCESS && (layerName != nullptr) && (pathing != nullptr) &&
                            (gridType != nullptr)) {

                            PathingStrategy pathStrategy = CELL_EDGES_ONLY;
                            if (std::string(pathing) == "cell_edges_and_diagonals") {
                                pathStrategy = CELL_EDGES_AND_DIAGONALS;
                            }

                            SortingStrategy sortStrategy = SORTING_CAMERA;
                            if (sorting != nullptr) {
                                if (std::string(sorting) == "location") {
                                    sortStrategy = SORTING_LOCATION;
                                } else if (std::string(sorting) == "camera_and_location") {
                                    sortStrategy = SORTING_CAMERA_AND_LOCATION;
                                }
                            }

                            CellGrid* grid = nullptr;
                            if (gridType != nullptr) {
                                grid = m_model->getCellGrid(gridType);
                            } else {
                                grid = m_model->getCellGrid("square");
                            }

                            if (grid != nullptr) {
                                grid->setXShift(xOffset);
                                grid->setXScale(xScale);
                                grid->setYShift(yOffset);
                                grid->setYScale(yScale);
                                grid->setZShift(zOffset);
                                grid->setZScale(zScale);
                                grid->setRotation(rotation);

                                Layer* layer = nullptr;
                                try {
                                    layer = map->createLayer(layerName, grid);
                                } catch (NameClash&) {
                                    // TODO - handle exception
                                    assert(false);
                                }

                                if (layer != nullptr) {
                                    layer->setPathingStrategy(pathStrategy);
                                    layer->setSortingStrategy(sortStrategy);
                                    if (layerType != nullptr) {
                                        if (std::string(layerType) == "walkable") {
                                            layer->setWalkable(true);
                                        } else if (std::string(layerType) == "interact") {
                                            layer->setInteract(true, (layerTypeName != nullptr) ? layerTypeName : "");
                                        }
                                    }

                                    double curr_x = 0;
                                    double curr_y = 0;

                                    for (const XML::Element* instances = layerElement->FirstChildElement("instances");
                                         instances != nullptr;
                                         instances = instances->NextSiblingElement("instances")) {
                                        for (const XML::Element* instance = instances->FirstChildElement("i");
                                             instance != nullptr;
                                             instance = instance->NextSiblingElement("i")) {
                                            double x      = 0;
                                            double y      = 0;
                                            double z      = 0;
                                            int r         = 0;
                                            int stackpos  = 0;
                                            int cellStack = 0;

                                            const char* instanceId = XML::Attribute(instance, "id");
                                            const char* objectId   = XML::Attribute(instance, "o");
                                            const char* costId     = XML::Attribute(instance, "cost_id");

                                            if (objectId == nullptr) {
                                                objectId = XML::Attribute(instance, "object");
                                            }

                                            if (objectId == nullptr) {
                                                objectId = XML::Attribute(instance, "obj");
                                            }

                                            const char* namespaceId = XML::Attribute(instance, "ns");

                                            if (namespaceId == nullptr) {
                                                namespaceId = XML::Attribute(instance, "namespace");
                                            }

                                            int xRetVal = XML::QueryAttribute(instance, "x", &x);
                                            int yRetVal = XML::QueryAttribute(instance, "y", &y);
                                            XML::QueryAttribute(instance, "z", &z);
                                            int rRetVal = XML::QueryAttribute(instance, "r", &r);

                                            if (xRetVal == XML::SUCCESS) {
                                                curr_x = x;
                                            } else {
                                                x = ++curr_x;
                                            }

                                            if (yRetVal == XML::SUCCESS) {
                                                curr_y = y;
                                            } else {
                                                y = curr_y;
                                            }

                                            if (rRetVal != XML::SUCCESS) {
                                                rRetVal = XML::QueryAttribute(instance, "rotation", &r);
                                            }

                                            int stackRetVal = XML::QueryAttribute(instance, "stackpos", &stackpos);
                                            int cellStackRetVal =
                                                XML::QueryAttribute(instance, "cellstack", &cellStack);

                                            if (objectId != nullptr) {
                                                if (namespaceId != nullptr) {
                                                    ns = namespaceId;
                                                }

                                                Object* object = m_model->getObject(objectId, ns);

                                                if (object != nullptr) {
                                                    Instance* inst = nullptr;
                                                    if (instanceId != nullptr) {
                                                        inst = layer->createInstance(
                                                            object, ExactModelCoordinate(x, y, z), instanceId);
                                                    } else {
                                                        inst = layer->createInstance(
                                                            object, ExactModelCoordinate(x, y, z));
                                                    }

                                                    if (inst != nullptr) {
                                                        if (rRetVal != XML::SUCCESS) {
                                                            auto* objVisual = object->getVisual<ObjectVisual>();
                                                            std::vector<int> angles;
                                                            objVisual->getStaticImageAngles(angles);
                                                            if (!angles.empty()) {
                                                                r = angles[0];
                                                            }
                                                        }

                                                        inst->setRotation(r);

                                                        InstanceVisual* instVisual = InstanceVisual::create(inst);

                                                        if ((instVisual != nullptr) && (stackRetVal == XML::SUCCESS)) {
                                                            instVisual->setStackPosition(stackpos);
                                                        }

                                                        if (cellStackRetVal == XML::SUCCESS) {
                                                            inst->setCellStackPosition(cellStack);
                                                        }

                                                        if (costId != nullptr) {
                                                            double cost = 0;
                                                            int costRetVal =
                                                                XML::QueryAttribute(instance, "cost", &cost);
                                                            if (costRetVal == XML::SUCCESS) {
                                                                inst->setCost(costId, cost);
                                                            }
                                                        }

                                                        if (object->getAction("default") != nullptr) {
                                                            Location target(layer);

                                                            inst->actRepeat("default", target);
                                                        }
                                                    } else {
                                                        std::ostringstream oss;
                                                        oss << " Failed to create instance of object " << objectId
                                                            << " : " << __FILE__ << " [" << __LINE__ << "]" << '\n';
                                                        FL_ERR(_log, oss.str());
                                                    }
                                                }
                                            }

                                            // increment % done counter
                                            m_percentDoneListener.incrementCount();
                                        }
                                    }
                                }
                            }
                        }

                        // increment % done counter
                        m_percentDoneListener.incrementCount();
                    }

                    // init CellCaches
                    map->initializeCellCaches();
                    // add Cells from xml File
                    for (const XML::Element* cacheElements = root->FirstChildElement("cellcaches");
                         cacheElements != nullptr;
                         cacheElements = cacheElements->NextSiblingElement("cellcaches")) {
                        for (const XML::Element* cacheElement = cacheElements->FirstChildElement("cellcache");
                             cacheElement != nullptr;
                             cacheElement = cacheElement->NextSiblingElement("cellcache")) {
                            double cacheCost    = 1.0;
                            double cacheSpeed   = 1.0;
                            const char* layerId = XML::Attribute(cacheElement, "id");

                            if (layerId != nullptr) {
                                cacheElement->QueryDoubleAttribute("default_cost", &cacheCost);
                                cacheElement->QueryDoubleAttribute("default_speed", &cacheSpeed);

                                Layer* layer = map->getLayer(layerId);
                                if (layer != nullptr) {
                                    CellCache* cache = layer->getCellCache();
                                    if (cache != nullptr) {
                                        int searchNarrow = 0;
                                        cacheElement->QueryIntAttribute("search_narrow", &searchNarrow);
                                        cache->setSearchNarrowCells(searchNarrow != 0);

                                        cache->setDefaultCostMultiplier(cacheCost);
                                        cache->setDefaultSpeedMultiplier(cacheSpeed);
                                        for (const XML::Element* cellElement = cacheElement->FirstChildElement("cell");
                                             cellElement != nullptr;
                                             cellElement = cellElement->NextSiblingElement("cell")) {
                                            int cellX   = 0;
                                            int cellY   = 0;
                                            int success = cellElement->QueryIntAttribute("x", &cellX);
                                            success &= cellElement->QueryIntAttribute("y", &cellY);
                                            if (success == XML::SUCCESS) {
                                                ModelCoordinate mc(cellX, cellY);
                                                Cell* cell = cache->createCell(mc);

                                                const char* cellBlocker = XML::Attribute(cellElement, "blocker_type");
                                                if (cellBlocker != nullptr) {
                                                    if (std::string(cellBlocker) == "no_blocker") {
                                                        CellTypeInfo cti = CTYPE_CELL_NO_BLOCKER;
                                                        cell->setCellType(cti);
                                                    } else if (std::string(cellBlocker) == "blocker") {
                                                        CellTypeInfo cti = CTYPE_CELL_BLOCKER;
                                                        cell->setCellType(cti);
                                                    }
                                                }

                                                double cellCost  = 1.0;
                                                double cellSpeed = 1.0;
                                                success = cellElement->QueryDoubleAttribute("default_cost", &cellCost);
                                                if (success == XML::SUCCESS) {
                                                    cell->setCostMultiplier(cellCost);
                                                }
                                                success =
                                                    cellElement->QueryDoubleAttribute("default_speed", &cellSpeed);
                                                if (success == XML::SUCCESS) {
                                                    cell->setSpeedMultiplier(cellSpeed);
                                                }

                                                int isNarrow = 0;
                                                cellElement->QueryIntAttribute("narrow", &isNarrow);
                                                if (isNarrow != 0) {
                                                    cache->addNarrowCell(cell);
                                                }
                                                // add cost with given id to cell
                                                for (const XML::Element* costElement =
                                                         cellElement->FirstChildElement("cost");
                                                     costElement != nullptr;
                                                     costElement = costElement->NextSiblingElement("cost")) {
                                                    const char* costId = XML::Attribute(costElement, "id");
                                                    double cost        = 1.0;
                                                    success = costElement->QueryDoubleAttribute("value", &cost);
                                                    if ((costId != nullptr) && success == XML::SUCCESS) {
                                                        cache->registerCost(costId, cost);
                                                        cache->addCellToCost(costId, cell);
                                                    }
                                                }
                                                // add area to cell
                                                for (const XML::Element* areaElement =
                                                         cellElement->FirstChildElement("area");
                                                     areaElement != nullptr;
                                                     areaElement = areaElement->NextSiblingElement("area")) {
                                                    const char* areaId = XML::Attribute(areaElement, "id");
                                                    if (areaId != nullptr) {
                                                        cache->addCellToArea(areaId, cell);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // finalize CellCaches
                    map->finalizeCellCaches();
                    // add Transistions
                    for (const XML::Element* cacheElements = root->FirstChildElement("cellcaches");
                         cacheElements != nullptr;
                         cacheElements = cacheElements->NextSiblingElement("cellcaches")) {
                        for (const XML::Element* cacheElement = cacheElements->FirstChildElement("cellcache");
                             cacheElement != nullptr;
                             cacheElement = cacheElement->NextSiblingElement("cellcache")) {
                            const char* layerId = XML::Attribute(cacheElement, "id");
                            if (layerId != nullptr) {
                                Layer* layer = map->getLayer(layerId);
                                if (layer != nullptr) {
                                    CellCache* cache = layer->getCellCache();
                                    if (cache != nullptr) {
                                        for (const XML::Element* cellElement = cacheElement->FirstChildElement("cell");
                                             cellElement != nullptr;
                                             cellElement = cellElement->NextSiblingElement("cell")) {
                                            int cellX   = 0;
                                            int cellY   = 0;
                                            int success = cellElement->QueryIntAttribute("x", &cellX);
                                            success &= cellElement->QueryIntAttribute("y", &cellY);
                                            if (success == XML::SUCCESS) {
                                                ModelCoordinate mc(cellX, cellY);
                                                Cell* cell = cache->getCell(mc);
                                                if (cell == nullptr) {
                                                    continue;
                                                }
                                                for (const XML::Element* transitionElement =
                                                         cellElement->FirstChildElement("transition");
                                                     transitionElement != nullptr;
                                                     transitionElement =
                                                         transitionElement->NextSiblingElement("transition")) {
                                                    int targetX = 0;
                                                    int targetY = 0;
                                                    int targetZ = 0;
                                                    success     = transitionElement->QueryIntAttribute("x", &targetX);
                                                    success &= transitionElement->QueryIntAttribute("y", &targetY);
                                                    transitionElement->QueryIntAttribute("z", &targetZ);
                                                    if (success == XML::SUCCESS) {
                                                        ModelCoordinate mc(targetX, targetY, targetZ);
                                                        Layer* targetLayer = nullptr;
                                                        const char* targetLayerId =
                                                            XML::Attribute(transitionElement, "id");
                                                        if (targetLayerId != nullptr) {
                                                            targetLayer = map->getLayer(targetLayerId);
                                                        }
                                                        if (targetLayer == nullptr) {
                                                            targetLayer = layer;
                                                        }

                                                        int immediate = 0;
                                                        transitionElement->QueryIntAttribute("immediate", &immediate);
                                                        cell->createTransition(targetLayer, mc, immediate != 0);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    for (const XML::Element* triggerElements = root->FirstChildElement("triggers");
                         triggerElements != nullptr;
                         triggerElements = triggerElements->NextSiblingElement("triggers")) {
                        TriggerController* triggerController = map->getTriggerController();
                        for (const XML::Element* triggerElement = triggerElements->FirstChildElement("trigger");
                             triggerElement != nullptr;
                             triggerElement = triggerElement->NextSiblingElement("trigger")) {
                            const char* triggerName = XML::Attribute(triggerElement, "name");
                            int triggered           = 0;
                            int allInstances        = 0;
                            triggerElement->QueryIntAttribute("triggered", &triggered);
                            triggerElement->QueryIntAttribute("all_instances", &allInstances);

                            Trigger* trigger = triggerController->createTrigger(triggerName);
                            if (triggered > 0) {
                                trigger->setTriggered();
                            }
                            if (allInstances > 0) {
                                trigger->enableForAllInstances();
                            }

                            const char* instanceId = XML::Attribute(triggerElement, "attached_instance");
                            const char* layerId    = XML::Attribute(triggerElement, "attached_layer");
                            if ((instanceId != nullptr) && (layerId != nullptr)) {
                                Layer* layer = map->getLayer(layerId);
                                if (layer != nullptr) {
                                    Instance* instance = layer->getInstance(instanceId);
                                    if (instance != nullptr) {
                                        trigger->attach(instance);
                                    }
                                }
                            }
                            for (const XML::Element* assignElement = triggerElement->FirstChildElement("assign");
                                 assignElement != nullptr;
                                 assignElement = assignElement->NextSiblingElement("assign")) {
                                layerId = XML::Attribute(assignElement, "layer_id");
                                if (layerId == nullptr) {
                                    continue;
                                }
                                int x = 0;
                                int y = 0;
                                assignElement->QueryIntAttribute("x", &x);
                                assignElement->QueryIntAttribute("y", &y);
                                Layer* layer = map->getLayer(layerId);
                                if (layer != nullptr) {
                                    trigger->assign(layer, ModelCoordinate(x, y));
                                }
                            }
                            for (const XML::Element* enabledElement = triggerElement->FirstChildElement("enabled");
                                 enabledElement != nullptr;
                                 enabledElement = enabledElement->NextSiblingElement("enabled")) {
                                layerId    = XML::Attribute(enabledElement, "layer_id");
                                instanceId = XML::Attribute(enabledElement, "instance_id");
                                if ((instanceId == nullptr) || (layerId == nullptr)) {
                                    continue;
                                }
                                Layer* layer = map->getLayer(layerId);
                                if (layer != nullptr) {
                                    Instance* instance = layer->getInstance(instanceId);
                                    if (instance != nullptr) {
                                        trigger->enableForInstance(instance);
                                    }
                                }
                            }
                            for (const XML::Element* conditionElement = triggerElement->FirstChildElement("condition");
                                 conditionElement != nullptr;
                                 conditionElement = conditionElement->NextSiblingElement("condition")) {
                                int conditionId = -1;
                                conditionElement->QueryIntAttribute("id", &conditionId);
                                if (conditionId != -1) {
                                    trigger->addTriggerCondition(static_cast<TriggerCondition>(conditionId));
                                }
                            }
                        }
                    }

                    for (const XML::Element* cameraElement = root->FirstChildElement("camera");
                         cameraElement != nullptr;
                         cameraElement = cameraElement->NextSiblingElement("camera")) {
                        const char* cameraId = XML::Attribute(cameraElement, "id");

                        int refCellWidth  = 0;
                        int refCellHeight = 0;
                        int success       = cameraElement->QueryIntAttribute("ref_cell_width", &refCellWidth);
                        success &= cameraElement->QueryIntAttribute("ref_cell_height", &refCellHeight);

                        if ((cameraId != nullptr) && success == XML::SUCCESS) {
                            double tilt     = 0.0;
                            double zoom     = 1.0;
                            double rotation = 0.0;
                            double zToY     = 0.0;
                            cameraElement->QueryDoubleAttribute("tilt", &tilt);
                            cameraElement->QueryDoubleAttribute("zoom", &zoom);
                            cameraElement->QueryDoubleAttribute("rotation", &rotation);
                            success = cameraElement->QueryDoubleAttribute("ztoy", &zToY);

                            const char* viewport = XML::Attribute(cameraElement, "viewport");

                            Camera* cam = nullptr;

                            if (viewport != nullptr) {
                                // parse out the viewport parameters
                                IntVector viewportParameters = tokenize(viewport, ',');

                                // make sure the right number of viewport parameters were parsed
                                if (viewportParameters.size() == 4) {
                                    Rect rect(
                                        viewportParameters[0],
                                        viewportParameters[1],
                                        viewportParameters[2],
                                        viewportParameters[3]);

                                    try {
                                        cam = map->addCamera(cameraId, rect);
                                    } catch (NameClash&) {
                                        // TODO - handle exception
                                        assert(false);
                                    }
                                }
                            } else {
                                Rect rect(0, 0, m_renderBackend->getScreenWidth(), m_renderBackend->getScreenHeight());

                                try {
                                    cam = map->addCamera(cameraId, rect);
                                } catch (NameClash&) {
                                    // TODO - handle exception
                                    assert(false);
                                }
                            }

                            if (cam != nullptr) {
                                cam->setCellImageDimensions(refCellWidth, refCellHeight);
                                cam->setRotation(rotation);
                                cam->setTilt(tilt);
                                cam->setZoom(zoom);
                                if (success == XML::SUCCESS) {
                                    cam->setZToY(zToY);
                                }

                                // active instance renderer for camera
                                InstanceRenderer* instanceRenderer = InstanceRenderer::getInstance(cam);
                                if (instanceRenderer != nullptr) {
                                    instanceRenderer->activateAllLayers(map);
                                }
                            }
                        }

                        // increment % done counter
                        m_percentDoneListener.incrementCount();
                    }
                }
            }
        }

        return map;
    }

    void MapLoader::setObjectLoader(const FIFE::ObjectLoaderPtr& objectLoader)
    {
        assert(objectLoader);

        m_objectLoader = objectLoader;
    }

    FIFE::ObjectLoaderPtr MapLoader::getObjectLoader()
    {
        return m_objectLoader;
    }

    void MapLoader::setAnimationLoader(const FIFE::AnimationLoaderPtr& animationLoader)
    {
        assert(animationLoader);

        m_objectLoader->setAnimationLoader(animationLoader);
    }

    FIFE::AnimationLoaderPtr MapLoader::getAnimationLoader()
    {
        if (m_objectLoader) {
            return m_objectLoader->getAnimationLoader();
        }
        return {};
    }

    void MapLoader::setAtlasLoader(const FIFE::AtlasLoaderPtr& atlasLoader)
    {
        assert(atlasLoader);

        m_objectLoader->setAtlasLoader(atlasLoader);
    }

    FIFE::AtlasLoaderPtr MapLoader::getAtlasLoader()
    {
        if (m_objectLoader) {
            return m_objectLoader->getAtlasLoader();
        }
        return {};
    }

    bool MapLoader::isLoadable(const std::string& filename) const
    {
        fs::path mapPath(filename);

        XML::Document mapFile;

        std::string mapFilename = mapPath.string();

        try {
            RawData* data = m_vfs->open(mapFilename);

            if (data != nullptr) {
                if (data->getDataLength() != 0) {
                    const std::string xml = data->readString(data->getDataLength());

                    if (!XML::Parse(mapFile, xml)) {
                        delete data;
                        return false;
                    }

                    const XML::Element* root = mapFile.RootElement();

                    if (root != nullptr) {
                        const char* loaderName = XML::Attribute(root, "loader");

                        // if the file does not specify a loader but was opened and parsed
                        // correctly then we know we have a compatible extension so we will
                        // attempt to load it, if it does specify a loader then the loader
                        // name will be checked
                        if ((loaderName == nullptr) ||
                            ((loaderName != nullptr) && std::string(loaderName) == getLoaderName())) {
                            return true;
                        }
                    }
                }

                // done with file delete the resource
                delete data;
                data = nullptr;
            }
        } catch (NotFound& e) {
            FL_ERR(_log, e.what());

            return false;
        }

        return false;
    }

    void MapLoader::loadImportFile(const std::string& file, const std::string& directory)
    {
        if (!file.empty()) {
            fs::path importFilePath(directory);
            importFilePath /= file;

            std::string importFileString = importFilePath.string();
            if (m_objectLoader && m_objectLoader->getAtlasLoader() &&
                m_objectLoader->getAtlasLoader()->isLoadable(importFileString)) {
                m_objectLoader->getAtlasLoader()->loadMultiple(importFileString);
            }
            if (m_objectLoader && m_objectLoader->getAnimationLoader() &&
                m_objectLoader->getAnimationLoader()->isLoadable(importFileString)) {
                m_objectLoader->getAnimationLoader()->loadMultiple(importFileString);
            }
            if (m_objectLoader && m_objectLoader->isLoadable(importFileString)) {
                m_objectLoader->load(importFileString);
            }
        }
    }

    void MapLoader::loadImportDirectory(const std::string& directory)
    {
        if (!directory.empty()) {
            fs::path importDirectory(directory);
            std::string importDirectoryString = importDirectory.string();

            std::set<std::string> files = m_vfs->listFiles(importDirectoryString);

            // load all xml files in the directory
            std::set<std::string>::iterator iter;
            for (iter = files.begin(); iter != files.end(); ++iter) {
                // TODO - vtchill - may need a way to allow clients to load things other
                // than .xml and .zip files
                std::string ext = GetExtension(*iter);
                if (ext == ".xml" || ext == ".zip") {
                    loadImportFile(*iter, importDirectoryString);
                }
            }

            std::set<std::string> nestedDirectories = m_vfs->listDirectories(importDirectoryString);
            for (iter = nestedDirectories.begin(); iter != nestedDirectories.end(); ++iter) {
                // do not attempt to load anything from a .svn directory
                if ((*iter).find(".svn") == std::string::npos) {
                    loadImportDirectory(importDirectoryString + "/" + *iter);
                }
            }
        }
    }

    void MapLoader::addPercentDoneListener(PercentDoneListener* listener)
    {
        m_percentDoneListener.addListener(listener);
    }

    const std::string& MapLoader::getLoaderName() const
    {
        return m_loaderName;
    }

    MapLoader* createDefaultMapLoader(Model* model, VFS* vfs, ImageManager* imageManager, RenderBackend* renderBackend)
    {
        return (new MapLoader(model, vfs, imageManager, renderBackend));
    }
} // namespace FIFE
