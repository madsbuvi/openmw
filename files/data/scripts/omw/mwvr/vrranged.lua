    -- VR CONTROLS -- -- --
    -- When loaded, this file implements VR controls and overrides some of playercontrols.lua with VR specific behavior.
    
local core = require('openmw.core')
local async = require('openmw.async')
local self = require('openmw.self')
local types = require('openmw.types')
local Actor = types.Actor
local vr = require('openmw.vr')


local worldAimPath = vr.stringToPath("/world/user/hand/right/input/aim/pose");

local function spellcastHandler()
    if not vr.isKeyboardMouseMode() then
        local worldPose = vr.getPose(worldAimPath)
        local spell = Actor.getSelectedSpell(self)
        vr.TEMPINTERFACE_launchMagicBolt(self, spell, worldPose.position.mwunits, worldPose.orientation, 0)
        return true
    end
    return false
end

local haveAddedTkHandlers = false

local function addTkHandlers()
    Actor.addAnimationTextKeyHandler(self, 'spellcast', 'target release', async:callback(spellcastHandler))
    haveAddedTkHandlers = true
end

-- Is there a better way to ensure the CharacterController object has been created before adding textKeyHandlers?
-- If i do it directly in the script (outside of any functions / engine handlers), it is executed before the player actor's charactercontroller is created, and
-- then attaching the textkey handlers fails.
local function onUpdate(dt)
    if not haveAddedTkHandlers then
        addTkHandlers()
    end
end

return {
    engineHandlers = {
        onUpdate = onUpdate
    }
}

