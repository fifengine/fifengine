/***************************************************************************
*   Copyright (C) 2005-2011 by the FIFE team                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ext/tinyxml/fife_tinyxml.h"
#include "model/structures/map.h"
#include "model/structures/layer.h"
#include "model/structures/instance.h"
#include "model/metamodel/object.h"
#include "model/metamodel/grids/cellgrid.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "view/visual.h"
#include "view/camera.h"

#include "mapsaver.h"

namespace FIFE {
    static Logger _log(LM_NATIVE_SAVERS);

    MapSaver::MapSaver() {
        //m_objectSaver = new ObjectSaver();
        //m_animationSaver = new AnimationSaver();
        //m_atlasSaver = new AtlasSaver();
    }

    MapSaver::~MapSaver()
    {

    }

    void MapSaver::setObjectSaver(const FIFE::ObjectSaverPtr& objectSaver) {
        m_objectSaver = objectSaver;
    }


    void MapSaver::setAnimationSaver(const FIFE::AnimationSaverPtr& animationSaver) {
        m_animationSaver = animationSaver;
    }


    void MapSaver::setAtlasSaver(const FIFE::AtlasSaverPtr& atlasSaver) {
        m_atlasSaver = atlasSaver;
    }

    void MapSaver::save(const Map& map, const std::string& filename, const std::vector<std::string>& importFiles) {
        TiXmlDocument doc;

        // add xml declaration
        TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "ascii", "");
        doc.LinkEndChild(decl);

        // add map element
        TiXmlElement* mapElement = new TiXmlElement("map");
        mapElement->SetAttribute("id", map.getId());
        mapElement->SetAttribute("format", "1.0");
        doc.LinkEndChild(mapElement);

        for (std::vector<std::string>::const_iterator iter = importFiles.begin(); iter != importFiles.end(); ++iter)
        {
            TiXmlElement* importElement = new TiXmlElement("import");
            importElement->SetAttribute("file", *iter);

            // link to map element
            mapElement->LinkEndChild(importElement);
        }

        typedef std::list<Layer*> LayerList;
        LayerList layers = map.getLayers();
        for (LayerList::iterator iter = layers.begin(); iter != layers.end(); ++iter)
        {
            TiXmlElement* layerElement = new TiXmlElement("layer");
            CellGrid* grid = (*iter)->getCellGrid();
            layerElement->SetAttribute("id", (*iter)->getId());
            layerElement->SetDoubleAttribute("x_offset", grid->getXShift());
            layerElement->SetDoubleAttribute("y_offset", grid->getYShift());
            layerElement->SetDoubleAttribute("z_offset", grid->getZShift());
            layerElement->SetDoubleAttribute("x_scale", grid->getXScale());
            layerElement->SetDoubleAttribute("y_scale", grid->getYScale());
            layerElement->SetDoubleAttribute("rotation", grid->getRotation());
            layerElement->SetAttribute("grid_type", grid->getType());
            layerElement->SetAttribute("transparency", (*iter)->getLayerTransparency());

            std::string pathingStrategy;
            switch ((*iter)->getPathingStrategy())
            {
                case CELL_EDGES_ONLY:
                {
                    pathingStrategy = "cell_edges_only";
                }
                break;
                case CELL_EDGES_AND_DIAGONALS:
                {
                    pathingStrategy = "cell_edges_and_diagonals";
                }
                break;
                case FREEFORM:
                {
                    pathingStrategy = "freeform";
                }
                break;
                default:
                {
                    pathingStrategy = "cell_edges_only";
                }
                break;
            }
            layerElement->SetAttribute("pathing", pathingStrategy);
            
            // add layer to document
            mapElement->LinkEndChild(layerElement);
            
            // add instances tag to document
            TiXmlElement* instancesElement = new TiXmlElement("instances");
            layerElement->LinkEndChild(instancesElement);

            std::string currentNamespace = "";
            typedef std::vector<Instance*> InstancesContainer;
            InstancesContainer instances = (*iter)->getInstances();
            for (InstancesContainer::iterator iter = instances.begin(); iter != instances.end(); ++iter)
            {
                // create instance element
                TiXmlElement* instanceElement = new TiXmlElement("i");

                Object* obj = (*iter)->getObject();
                if (!obj->getNamespace().empty() && currentNamespace != obj->getNamespace())
                {
                    instanceElement->SetAttribute("ns", obj->getNamespace());
                    
                    // update current namespace
                    currentNamespace = obj->getNamespace();
                }

                if (!(*iter)->getId().empty())
                {
                    instanceElement->SetAttribute("id", (*iter)->getId());
                }

                instanceElement->SetAttribute("o", obj->getId());

                ExactModelCoordinate position = (*iter)->getLocationRef().getExactLayerCoordinates();
                instanceElement->SetDoubleAttribute("x", position.x);
                instanceElement->SetDoubleAttribute("y", position.y);
                instanceElement->SetDoubleAttribute("z", position.z);
                instanceElement->SetAttribute("r", (*iter)->getRotation());

                if ((*iter)->isBlocking())
                {
                    instanceElement->SetAttribute("blocking", (*iter)->isBlocking());    
                }

                InstanceVisual* instanceVisual = (*iter)->getVisual<InstanceVisual>();
                instanceElement->SetAttribute("stackpos", instanceVisual->getStackPosition());

                instancesElement->LinkEndChild(instanceElement);
            }
        }

        typedef std::vector<Camera*> CameraContainer;
        CameraContainer cameras = map.getCameras();
        for (CameraContainer::iterator iter = cameras.begin(); iter != cameras.end(); ++iter)
        {
            if ((*iter)->getLocationRef().getMap()->getId() == map.getId())
            {
                TiXmlElement* cameraElement = new TiXmlElement("camera");

                cameraElement->SetAttribute("id", (*iter)->getId());
                cameraElement->SetAttribute("ref_layer_id", (*iter)->getLocation().getLayer()->getId());
                cameraElement->SetDoubleAttribute("zoom", (*iter)->getZoom());
                cameraElement->SetDoubleAttribute("tilt", (*iter)->getTilt());
                cameraElement->SetDoubleAttribute("rotation", (*iter)->getRotation());
                
                Rect viewport = (*iter)->getViewPort();
                std::ostringstream viewportString;
                viewportString << viewport.x << "," 
                               << viewport.y << ","
                               << viewport.w << ","
                               << viewport.h;

                cameraElement->SetAttribute("viewport", viewportString.str());

                Point p = (*iter)->getCellImageDimensions();
                cameraElement->SetAttribute("ref_cell_width", p.x);
                cameraElement->SetAttribute("ref_cell_height", p.y);

                std::vector<float> lightingColor = (*iter)->getLightingColor();
                bool writeLightingColor = false;
                for (uint32_t i=0; i < lightingColor.size(); ++i)
                {
                    if (lightingColor[i] < 1.0)
                    {
                        writeLightingColor = true;
                        break;
                    }
                }

                if (writeLightingColor)
                {
                    std::ostringstream lightingColorString;
                    for (uint32_t i=0; i < lightingColor.size(); ++i)
                    {
                        if (i > 0)
                        {
                            lightingColorString << ",";
                        }
                        
                        lightingColorString << lightingColor[i];

                        cameraElement->SetAttribute("light_color", lightingColorString.str());
                    }
                }
                
                mapElement->LinkEndChild(cameraElement);
            }
        }

        // save the map xml file
        doc.SaveFile(filename);
    }
}