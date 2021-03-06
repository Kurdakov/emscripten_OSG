#include <osgDB/Registry>

USE_SERIALIZER_WRAPPER(osgTerrain_CompositeLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_ContourLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_GeometryTechnique)
USE_SERIALIZER_WRAPPER(osgTerrain_HeightFieldLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_ImageLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_Layer)
USE_SERIALIZER_WRAPPER(osgTerrain_Locator)
USE_SERIALIZER_WRAPPER(osgTerrain_ProxyLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_SwitchLayer)
USE_SERIALIZER_WRAPPER(osgTerrain_Terrain)
USE_SERIALIZER_WRAPPER(osgTerrain_TerrainTechnique)
USE_SERIALIZER_WRAPPER(osgTerrain_TerrainTile)

extern "C" void wrapper_serializer_library_osgTerrain(void) {}

