# local files
set(GAME
    main.cpp
    engine.cpp
    options.cpp

    ${CMAKE_SOURCE_DIR}/files/windows/openmw.rc
    ${CMAKE_SOURCE_DIR}/files/windows/openmw.exe.manifest
)

if (ANDROID)
    set(GAME ${GAME} android_main.cpp)
endif()

set(GAME_HEADER
    engine.hpp
)

source_group(game FILES ${GAME} ${GAME_HEADER})

add_openmw_dir (mwrender
    actors objects renderingmanager animation rotatecontroller sky skyutil npcanimation vismask
    creatureanimation effectmanager util renderinginterface pathgrid rendermode weaponanimation screenshotmanager
    bulletdebugdraw globalmap characterpreview camera localmap water terrainstorage ripplesimulation
    renderbin actoranimation landmanager navmesh actorspaths recastmesh fogmanager objectpaging groundcover
    postprocessor pingpongcull luminancecalculator pingpongcanvas transparentpass navmeshmode precipitationocclusion ripples
    )

add_openmw_dir (mwinput
    actions actionmanager bindingsmanager controllermanager controlswitch
    inputmanagerimp mousemanager keyboardmanager sensormanager gyromanager
    )

add_openmw_dir (mwgui
    layout textinput widgets race class birth review windowmanagerimp console dialogue
    windowbase statswindow messagebox journalwindow charactercreation
    mapwindow windowpinnablebase tooltips scrollwindow bookwindow resourceskin
    formatting inventorywindow container hud countdialog tradewindow settingswindow
    confirmationdialog alchemywindow referenceinterface spellwindow mainmenu quickkeysmenu
    itemselection spellbuyingwindow loadingscreen levelupdialog waitdialog spellcreationdialog
    enchantingdialog trainingwindow travelwindow exposedwindow cursor spellicons
    merchantrepair repair soulgemdialog companionwindow bookpage journalviewmodel journalbooks
    itemmodel containeritemmodel inventoryitemmodel sortfilteritemmodel itemview
    tradeitemmodel companionitemmodel pickpocketitemmodel controllers savegamedialog
    recharge mode videowidget backgroundimage itemwidget screenfader debugwindow spellmodel spellview
    draganddrop timeadvancer jailscreen itemchargeview keyboardnavigation textcolours statswatcher
    postprocessorhud settings
    )

add_openmw_dir (mwdialogue
    dialoguemanagerimp journalimp journalentry quest topic filter selectwrapper hypertextparser keywordsearch scripttest
    )

add_openmw_dir (mwscript
    locals scriptmanagerimp compilercontext interpretercontext cellextensions miscextensions
    guiextensions soundextensions skyextensions statsextensions containerextensions
    aiextensions controlextensions extensions globalscripts ref dialogueextensions
    animationextensions transformationextensions consoleextensions userextensions
    )

add_openmw_dir (mwlua
    luamanagerimp object worldview userdataserializer luaevents engineevents objectvariant
    context globalscripts localscripts playerscripts luabindings objectbindings cellbindings
    camerabindings uibindings inputbindings nearbybindings postprocessingbindings stats debugbindings
    types/types types/door types/actor types/container types/weapon types/npc types/creature types/activator types/book types/lockpick types/probe types/apparatus types/potion types/ingredient types/misc types/repair types/armor types/light types/static types/clothing
    worker magicbindings
    vrbindings
    )

add_openmw_dir (mwsound
    soundmanagerimp openal_output ffmpeg_decoder sound sound_buffer sound_decoder sound_output
    loudness movieaudiofactory alext efx efx-presets regionsoundselector watersoundupdater volumesettings
    )

add_openmw_dir (mwworld
    refdata worldimp scene globals class action nullaction actionteleport
    containerstore actiontalk actiontake manualref player cellvisitors failedaction
    worldmodel localscripts customdata inventorystore ptr actionopen actionread actionharvest
    actionequip timestamp actionalchemy cellstore actionapply actioneat
    store esmstore fallback actionrepair actionsoulgem livecellref actiondoor
    contentloader esmloader actiontrap cellreflist cellref weather projectilemanager
    cellpreloader datetimemanager groundcoverstore magiceffects cell
    )

add_openmw_dir (mwphysics
    physicssystem trace collisiontype actor convert object heightfield closestnotmerayresultcallback
    contacttestresultcallback deepestnotmecontacttestresultcallback stepper movementsolver projectile
    actorconvexcallback raycasting mtphysics contacttestwrapper projectileconvexcallback
    )

add_openmw_dir (mwclass
    classes activator creature npc weapon armor potion apparatus book clothing container door
    ingredient creaturelevlist itemlevlist light lockpick misc probe repair static actor bodypart
    esm4base light4
    )

add_openmw_dir (mwmechanics
    mechanicsmanagerimp stat creaturestats magiceffects movement actorutil spelllist
    drawstate spells activespells npcstats aipackage aisequence aipursue alchemy aiwander aitravel aifollow aiavoiddoor aibreathe
    aicast aiescort aiface aiactivate aicombat recharge repair enchanting pathfinding pathgrid security spellcasting spellresistance
    disease pickpocket levelledlist combat steering obstacle autocalcspell difficultyscaling aicombataction summoning
    character actors objects aistate trading weaponpriority spellpriority weapontype spellutil
    spelleffects
    )

add_openmw_dir (mwstate
    statemanagerimp charactermanager character quicksavemanager
    )

add_openmw_dir (mwbase
    environment world scriptmanager dialoguemanager journal soundmanager mechanicsmanager
    inputmanager windowmanager statemanager
    )

# Main executable

set(OPENMW_LINK_TARGETS 
    # CMake's built-in OSG finder does not use pkgconfig, so we have to
    # manually ensure the order is correct for inter-library dependencies.
    # This only makes a difference with `-DOPENMW_USE_SYSTEM_OSG=ON -DOSG_STATIC=ON`.
    # https://gitlab.kitware.com/cmake/cmake/-/issues/21701
    ${OSGPARTICLE_LIBRARIES}
    ${OSGVIEWER_LIBRARIES}
    ${OSGGA_LIBRARIES}
    ${OSGSHADOW_LIBRARIES}
    ${OSGDB_LIBRARIES}
    ${OSGUTIL_LIBRARIES}
    ${OSG_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${OPENAL_LIBRARY}
    ${FFmpeg_LIBRARIES}
    ${MyGUI_LIBRARIES}
    ${SDL2_LIBRARY}
    ${RecastNavigation_LIBRARIES}
    "osg-ffmpeg-videoplayer"
    "oics"
    components
)

set (OPENMW_PRECOMPILE_HEADERS
        <boost/program_options/options_description.hpp>

        <sol/sol.hpp>

        <osg/StateSet>
        <osg/Node>
        <osg/Drawable>
        <osg/Camera>

        <osgViewer/Viewer>

        <MyGUI_Widget.h>

        <algorithm>
        <filesystem>
        <fstream>
        <functional>
        <memory>
        <string>
        <string_view>
        <vector>)

if (USE_SYSTEM_TINYXML)
    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        ${TinyXML_LIBRARIES})
endif()
    
if (NOT UNIX)
    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        ${SDL2MAIN_LIBRARY})
endif()

if (ANDROID)
    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        EGL android log z)
endif (ANDROID)

# Fix for not visible pthreads functions for linker with glibc 2.15
if (UNIX AND NOT APPLE)
    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        ${CMAKE_THREAD_LIBS_INIT})
endif()

if(APPLE)
    set(BUNDLE_RESOURCES_DIR "${APP_BUNDLE_DIR}/Contents/Resources")

    set(OPENMW_RESOURCES_ROOT ${BUNDLE_RESOURCES_DIR})

    add_subdirectory(../../files/ ${CMAKE_CURRENT_BINARY_DIR}/files)

    configure_file("${OpenMW_BINARY_DIR}/overrides.bin" ${BUNDLE_RESOURCES_DIR} COPYONLY)
    configure_file("${OpenMW_BINARY_DIR}/defaults.bin" ${BUNDLE_RESOURCES_DIR} COPYONLY)
    configure_file("${OpenMW_BINARY_DIR}/openmw.cfg" ${BUNDLE_RESOURCES_DIR} COPYONLY)
    configure_file("${OpenMW_BINARY_DIR}/gamecontrollerdb.txt" ${BUNDLE_RESOURCES_DIR} COPYONLY)

    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)

    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK})

    if (FFmpeg_FOUND)
        target_link_libraries(openmw z)
        target_link_options(openmw PRIVATE "LINKER:SHELL:-framework CoreVideo"
                                           "LINKER:SHELL:-framework CoreMedia"
                                           "LINKER:SHELL:-framework VideoToolbox"
                                           "LINKER:SHELL:-framework AudioToolbox"
                                           "LINKER:SHELL:-framework VideoDecodeAcceleration")
    endif()
endif(APPLE)

if (BUILD_WITH_CODE_COVERAGE)
    target_compile_options(openmw PRIVATE --coverage)
    set(OPENMW_LINK_TARGETS ${OPENMW_LINK_TARGETS}
        gcov)
endif()

if (MSVC)
    # Debug version needs increased number of sections beyond 2^16
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
        set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
    endif ()
endif (MSVC)
    
if(BUILD_OPENMW)
    if (NOT ANDROID)
        openmw_add_executable(openmw
            ${OPENMW_FILES}
            ${GAME} ${GAME_HEADER}
            ${APPLE_BUNDLE_RESOURCES}
        )
    else ()
        add_library(openmw
            SHARED
            ${OPENMW_FILES}
            ${GAME} ${GAME_HEADER}
        )
    endif ()

    target_link_libraries(openmw ${OPENMW_LINK_TARGETS})
    
    if(APPLE)
        add_custom_command(TARGET openmw
            POST_BUILD
            COMMAND cp "${OpenMW_BINARY_DIR}/resources/version" "${BUNDLE_RESOURCES_DIR}/resources")
    endif(APPLE)
    

    if (MSVC AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
        target_precompile_headers(openmw PRIVATE ${SOL_INCLUDE_DIR}/sol/sol.hpp)
    endif ()

    if (WIN32)
        INSTALL(TARGETS openmw RUNTIME DESTINATION ".")
    endif (WIN32)
    
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.16 AND MSVC)
        target_precompile_headers(openmw PRIVATE ${OPENMW_PRECOMPILE_HEADERS})
    endif()
endif()

if(BUILD_OPENMW_VR)
# Use of FetchContent to include the OpenXR SDK requires CMake 3.11
    if(${CMAKE_VERSION} VERSION_LESS "3.11") 
        message(FATAL_ERROR "Building openmw_vr requires CMake version 3.11 or later.")
    endif()

# TODO: Openmw and openmw_vr should preferrably share game code as a static or shared library
# instead of being compiled separately, though for now that's not possible as i depend on
# USE_OPENXR preprocessor switches.
    add_openmw_dir (mwvr
         openxrinput
         radialmenu 
         vranimation vrgui vrinputmanager vrinput vrlistbox vrmetamenu vrpingpongcallback vrpointer vrtracking vrtypes vrutil vrvirtualkeyboard
        )

    openmw_add_executable(openmw_vr
        ${OPENMW_FILES}
        ${GAME} ${GAME_HEADER}
        ${APPLE_BUNDLE_RESOURCES}
    )

    if (MSVC AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
        target_precompile_headers(openmw_vr PRIVATE ${SOL_INCLUDE_DIR}/sol/sol.hpp)
    endif ()

    configure_resource_file(${OpenMW_SOURCE_DIR}/files/xrcontrollersuggestions.xml
        "${OpenMW_BINARY_DIR}" "xrcontrollersuggestions.xml")

    target_link_libraries(openmw_vr openxr_loader)

    # Preprocessor variables used to control code paths to vr code
    if (WIN32)
        target_compile_options(openmw_vr PUBLIC -DUSE_OPENXR -DXR_USE_GRAPHICS_API_OPENGL -DXR_USE_GRAPHICS_API_D3D11 -DXR_USE_PLATFORM_WIN32)
    elseif(UNIX)
        target_compile_options(openmw_vr PUBLIC -DUSE_OPENXR -DXR_USE_GRAPHICS_API_OPENGL -DXR_USE_PLATFORM_XLIB)
    endif()
    target_link_libraries(openmw_vr ${OPENMW_LINK_TARGETS})
    if(APPLE)
        add_custom_command(TARGET openmw_vr
            POST_BUILD
            COMMAND cp "${OpenMW_BINARY_DIR}/resources/version" "${BUNDLE_RESOURCES_DIR}/resources")
    endif(APPLE)

    if (WIN32)
        INSTALL(TARGETS openmw_vr RUNTIME DESTINATION ".")
    endif (WIN32)
    
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.16 AND MSVC)
        target_precompile_headers(openmw_vr PRIVATE ${OPENMW_PRECOMPILE_HEADERS})
    endif()
endif()

# Sound stuff - here so CMake doesn't stupidly recompile EVERYTHING
# when we change the backend.
include_directories(
    ${FFmpeg_INCLUDE_DIRS}
)
