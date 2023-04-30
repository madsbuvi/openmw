-- VR Interfaces -- -- --
-- When loaded, this file defines VR specific setting groups and exposes these as an interface
-- Any other future interfaces go here too
    
local vr = require('openmw.vr')
local I = require('openmw.interfaces')
local storage = require('openmw.storage')

local function boolSetting(key, default)
    return {
        key = key,
        renderer = 'checkbox',
        name = key,
        description = key..'Description',
        default = default,
    }
end

local function floatSetting(key, default)
    return {
        key = key,
        renderer = 'number',
        name = key,
        description = key..'Description',
        default = default,
    }
end

local VRl10n = 'OMWVR'
local VRKey = 'OMWVR'
local VRSettingsKey= 'SettingsVR'

I.Settings.registerPage({
  key = VRKey,
  l10n = VRKey,
  name = 'VRPage',
  description = 'VRPageDescription',
})

local function registerGroup(grp, entries)
    I.Settings.registerGroup({
        key = VRSettingsKey..grp,
        page = VRKey,
        l10n = VRKey,
        name = VRSettingsKey..grp,
        permanentStorage = true,
        settings = entries,
    })
end

registerGroup('PhysicalSneak',{
        boolSetting('physicalSneakEnabled', false),
        floatSetting('physicalSneakOffset', 0.25),
    })

registerGroup('RealisticCombat',{
        floatSetting("realisticCombatMinVelocity", 1.0),
        floatSetting("realisticCombatMaxVelocity", 4.0),
        floatSetting("realisticCombatMinInterval", 0.25),
        floatSetting("realisticCombatMinLength", 0.25),
    })

local settingsPhysicalSneak = storage.playerSection(VRSettingsKey..'PhysicalSneak')
local settingsRealisticCombat = storage.playerSection(VRSettingsKey..'RealisticCombat')

return {
    interfaceName = 'VR',
    interface = {

        physicalSneakEnabled = function() return settingsPhysicalSneak:get('physicalSneakEnabled') end,
        physicalSneakOffset = function() return settingsPhysicalSneak:get('physicalSneakOffset') end,

        realisticCombatMinVelocity = function() return settingsRealisticCombat:get('realisticCombatMinVelocity') end,
        realisticCombatMaxVelocity = function() return settingsRealisticCombat:get('realisticCombatMaxVelocity') end,
        realisticCombatMinInterval = function() return settingsRealisticCombat:get('realisticCombatMinInterval') end,
        realisticCombatMinLength = function() return settingsRealisticCombat:get('realisticCombatMinLength') end,

    }
}
