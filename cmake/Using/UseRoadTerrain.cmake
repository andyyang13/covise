MACRO(USE_ROADTERRAIN)
set(OPT "")
if ("${ARGN}" STREQUAL "optional")
    set(OPT "optional")
endif()

if(NOT COVISE_BUILD_DRIVINGSIM)
    if (NOT "${OPT}" STREQUAL "optional")
        return()
    endif()
else()
    if (NOT ROADTERRAIN_USED)
        set(ROADTERRAIN_USED TRUE)
        use_vehicleutil(${OPT})
        use_gdal(${OPT})

        set(EXTRA_LIBS ${EXTRA_LIBS} coRoadTerrain)
        include_directories(${COVISEDIR}/src/OpenCOVER/DrivingSim)
    endif()
endif()
ENDMACRO(USE_ROADTERRAIN)
