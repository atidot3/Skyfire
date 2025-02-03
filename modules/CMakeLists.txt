# Fichier source temporaire généré
set(MODULES_LOADER_TEMP "${CMAKE_BINARY_DIR}/ModulesLoader.cpp")
set(MODULES_LOADER_FINAL "${CMAKE_SOURCE_DIR}/modules/ModulesLoader.cpp")
set(MODULES_LOADER_TEMPLATE "${CMAKE_SOURCE_DIR}/modules/ModulesLoader.cpp.in.cmake")

# Détection automatique des modules
file(GLOB MODULE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# Détecter les modules valides
set(VALID_MODULES "")
foreach(MODULE ${MODULE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE})
        # Vérifie qu'il contient un dossier "src" avec au moins un fichier .cpp
        file(GLOB MODULE_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.cpp")
        if(MODULE_SRC_FILES)
            list(APPEND VALID_MODULES ${MODULE})
        endif()
    endif()
endforeach()

message(STATUS "Modules valides détectés : ${VALID_MODULES}")

# Ajout des fichiers dans les groupes Visual Studio
set(MODULES_SOURCES "")
foreach(MODULE ${VALID_MODULES})
    string(REPLACE "-" "_" MODULE_FUNC_NAME ${MODULE})

    # Récupération des fichiers source du module
    file(GLOB MODULE_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.h")
    list(APPEND MODULES_SOURCES ${MODULE_SRC_FILES})

    # Créer un groupe Visual Studio pour le module
    source_group("${MODULE}" FILES ${MODULE_SRC_FILES})
endforeach()

set(MODULES_INCLUDES "")
set(MODULES_DECLARATIONS "")
set(MODULES_CALLS "")
set(MODULE_SOURCES "")

foreach(MODULE ${MODULE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE})
        string(REPLACE "-" "_" MODULE_FUNC_NAME ${MODULE})

        message("Is directory ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}")
        # Ajout des includes des modules
        set(MODULES_INCLUDES "${MODULES_INCLUDES}void AddSC_${MODULE_FUNC_NAME}();\n")

        # Ajout des déclarations et appels pour ModulesLoader.cpp
        set(MODULES_DECLARATIONS "${MODULES_DECLARATIONS}void AddSC_${MODULE_FUNC_NAME}();\n")
        set(MODULES_CALLS "${MODULES_CALLS}AddSC_${MODULE_FUNC_NAME}();\n")

        # Récupération des fichiers source du module
        file(GLOB MODULE_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.h")
        list(APPEND MODULE_SOURCES ${MODULE_SRC_FILES})

        message("Sources ${MODULE_SRC_FILES}")
    endif()
endforeach()

# Génération temporaire du fichier ModulesLoader.cpp
configure_file(${MODULES_LOADER_TEMPLATE} ${MODULES_LOADER_TEMP} @ONLY)

# Copie du fichier généré dans le dossier source réel (remplace l'ancien)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MODULES_LOADER_TEMP} ${MODULES_LOADER_FINAL})

# Inclure explicitement ModulesLoader.cpp et ModulesLoader.h
set(MODULES_LOADER_FILES
    ${MODULES_LOADER_FINAL}
    ${CMAKE_SOURCE_DIR}/modules/ModulesLoader.h
)

# Ajouter ces fichiers dans les sources
list(APPEND MODULES_SOURCES ${MODULES_LOADER_FILES})
source_group("ModulesLoader" FILES ${MODULES_LOADER_FILES})
message(STATUS "ModulesLoader.cpp: ${MODULES_LOADER_FINAL}")
message(STATUS "ModulesLoader.h: ${CMAKE_SOURCE_DIR}/modules/ModulesLoader.h")
message(STATUS "Final MODULES_SOURCES: ${MODULES_SOURCES}")
# Création de la bibliothèque statique "modules"
add_library(modules STATIC ${MODULES_SOURCES})

# Ajout des chemins d'inclusion nécessaires pour accéder aux fichiers comme "Player.h"
target_include_directories(modules PRIVATE
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography/Authentication
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic/LinkedReference
  ${CMAKE_SOURCE_DIR}/src/server/shared/Dynamic
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Management
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/game/Accounts
  ${CMAKE_SOURCE_DIR}/src/server/game/Achievements
  ${CMAKE_SOURCE_DIR}/src/server/game/Addons
  ${CMAKE_SOURCE_DIR}/src/server/game/AI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/CoreAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/ScriptedAI
  ${CMAKE_SOURCE_DIR}/src/server/game/AI/SmartScripts
  ${CMAKE_SOURCE_DIR}/src/server/game/AuctionHouse
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlefield/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds
  ${CMAKE_SOURCE_DIR}/src/server/game/Battlegrounds/Zones
  ${CMAKE_SOURCE_DIR}/src/server/game/BlackMarket
  ${CMAKE_SOURCE_DIR}/src/server/game/Calendar
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat
  ${CMAKE_SOURCE_DIR}/src/server/game/Chat/Channels
  ${CMAKE_SOURCE_DIR}/src/server/game/Cinematic
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
  ${CMAKE_SOURCE_DIR}/src/server/game/Combat
  ${CMAKE_SOURCE_DIR}/src/server/game/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/game/DungeonFinding
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/AreaTrigger
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Corpse
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Creature
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/DynamicObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Item/Container
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/GameObject
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object/Updates
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Pet
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Player
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Transport
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Unit
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Vehicle
  ${CMAKE_SOURCE_DIR}/src/server/game/Events
  ${CMAKE_SOURCE_DIR}/src/server/game/Globals
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Cells
  ${CMAKE_SOURCE_DIR}/src/server/game/Grids/Notifiers
  ${CMAKE_SOURCE_DIR}/src/server/game/Groups
  ${CMAKE_SOURCE_DIR}/src/server/game/Guilds
  ${CMAKE_SOURCE_DIR}/src/server/game/Handlers
  ${CMAKE_SOURCE_DIR}/src/server/game/Instances
  ${CMAKE_SOURCE_DIR}/src/server/game/LookingForGroup
  ${CMAKE_SOURCE_DIR}/src/server/game/Loot
  ${CMAKE_SOURCE_DIR}/src/server/game/Mails
  ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
  ${CMAKE_SOURCE_DIR}/src/server/game/Maps
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/MovementGenerators
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Spline
  ${CMAKE_SOURCE_DIR}/src/server/game/Movement/Waypoints
  ${CMAKE_SOURCE_DIR}/src/server/game/Opcodes
  ${CMAKE_SOURCE_DIR}/src/server/game/OutdoorPvP
  ${CMAKE_SOURCE_DIR}/src/server/game/Pools
  ${CMAKE_SOURCE_DIR}/src/server/game/PrecompiledHeaders
  ${CMAKE_SOURCE_DIR}/src/server/game/Quests
  ${CMAKE_SOURCE_DIR}/src/server/game/Reputation
  ${CMAKE_SOURCE_DIR}/src/server/game/Scripting
  ${CMAKE_SOURCE_DIR}/src/server/game/Server
  ${CMAKE_SOURCE_DIR}/src/server/game/Server/Protocol
  ${CMAKE_SOURCE_DIR}/src/server/game/Skills
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells/Auras
  ${CMAKE_SOURCE_DIR}/src/server/game/Texts
  ${CMAKE_SOURCE_DIR}/src/server/game/Tickets
  ${CMAKE_SOURCE_DIR}/src/server/game/Tools
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden
  ${CMAKE_SOURCE_DIR}/src/server/game/Warden/Modules
  ${CMAKE_SOURCE_DIR}/src/server/game/Weather
  ${CMAKE_SOURCE_DIR}/src/server/game/World
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_dependencies(modules revision.h)