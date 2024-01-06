include(source/ui/CMakeLists.txt)
include(source/states/CMakeLists.txt)
include(source/entities/CMakeLists.txt)
include(source/components/CMakeLists.txt)

set(ASTEROID_SOURCE
    source/main.cpp
    source/utils.cpp
    ${ASTEROID_SOURCE_UI}
    ${ASTEROID_SOURCE_STATES}
    ${ASTEROID_SOURCE_ENTITIES}
    ${ASTEROID_SOURCE_COMPONENTS}
)