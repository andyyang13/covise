/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef _NEWPROJECTNAME_PLUGIN_H_
#define _NEWPROJECTNAME_PLUGIN_H_

#include <cover/coVRPlugin.h>
#include <vector>

#include <cover/coVRPluginSupport.h>
#include <cover/VRSceneGraph.h>
#include <cover/coVRFileManager.h>
#include <config/CoviseConfig.h>
#include "Points.h"
//#include "PointCloudDrawable.h"
#include "PointCloudGeometry.h"
#include "PointCloudInteractor.h"

using namespace opencover;

class nodeInfo
{
public:
    osg::Node *node;
};

class fileInfo
{
public:
    std::string filename;
    std::vector<nodeInfo> nodes;
    int pointSetSize;
    PointSet *pointSet;
};

/** Plugin
  @author 
*/
class PointCloudPlugin : public coVRPlugin, public ui::Owner
{

    /** File entry class for Image Plugin
   **/
    class ImageFileEntry
    {
    public:
        string menuName;
        string fileName;
        ui::Element *fileMenuItem;

        ImageFileEntry(const char *menu, const char *file, ui::Element *menuitem)
        {
            menuName = menu;
            fileName = file;
            fileMenuItem = menuitem;
        }
    };

private:
    std::vector<fileInfo> files;
    int num_points;
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;
    void createGeodes(osg::Group *, const std::string &);
    int pointSetSize;
    PointSet *pointSet;
    osg::Vec3 vecBase;
    std::vector<ImageFileEntry> pointVec;
    void clearData();
    void selectedMenuButton(ui::Element *);
    void readMenuConfigData(const char *, std::vector<ImageFileEntry> &, ui::Group *);
    bool intensityOnly;
    float intensityScale;
    bool intColor;
    bool polar;
    float pointSizeValue;
    bool adaptLOD;
    static PointCloudInteractor *s_pointCloudInteractor;
    std::vector<ScannerPosition> positions;

protected:
    osg::MatrixTransform *planetTrans;

    //coSubMenuItem *imanPluginInstanceMenuItem;
    //coRowMenu *imanPluginInstanceMenu;
    // //coCheckboxMenuItem* enablePointCloudPlugin;
    //coRowMenu *loadMenu;
    //coSubMenuItem *loadMenuItem;
    //coButtonMenuItem *deleteMenuItem;
    ui::Menu *pointCloudMenu = nullptr;
    ui::Menu *loadMenu = nullptr;
    ui::Group *fileGroup = nullptr;
    ui::Group *loadGroup = nullptr;
    ui::Group *selectionGroup = nullptr;
    ui::Button *singleSelectButton = nullptr;
    ui::Button *deselectButton = nullptr;
    //ui::Button *deleteButton = nullptr;
    ui::ButtonGroup *selectionButtonGroup = nullptr;
    ui::Group *viewGroup = nullptr;
    ui::Button *adaptLODButton = nullptr;
    ui::Slider *pointSizeSlider = nullptr;

public:
    PointCloudPlugin();
    ~PointCloudPlugin();
    bool init();
    void preFrame();
    void postFrame();
    float pointSize()
    {
        return pointSizeValue;
    };
    static int loadPTS(const char *filename, osg::Group *loadParent, const char *covise_key);
    static int unloadPTS(const char *filename, const char *covise_key);
    int unloadFile(std::string filename);
    static PointCloudPlugin *plugin;
    ui::Group *FileGroup;
};

#endif
