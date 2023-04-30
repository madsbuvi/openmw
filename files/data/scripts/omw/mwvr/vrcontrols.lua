    -- VR CONTROLS -- -- --
    -- When loaded, this file implements VR controls and overrides some of playercontrols.lua with VR specific behavior.
    
local core = require('openmw.core')
local input = require('openmw.input')
local self = require('openmw.self')
local util = require('openmw.util')
local ui = require('openmw.ui')
local Actor = require('openmw.types').Actor
local Player = require('openmw.types').Player
local vr = require('openmw.vr')

local storage = require('openmw.storage')
local I = require('openmw.interfaces')

local settingsGroup = 'SettingsOMWControls'
local settings = storage.playerSection(settingsGroup)

I.Controls.overrideMovementControls(true)

local attemptJump = false
local autoMove = false
local sneakToggledOn = false

local headPath = vr.stringToPath("/stage/user/head/input/pose");

    -- VR EXTENSION BEGIN/ -- -- --
local function processSneak()
    local sneak = input.isActionPressed(input.ACTION.Sneak);
    
    -- if physical sneak is enabled, we want to check height every frame and toggle sneak depending on the height
    if I.VR.physicalSneakEnabled() and vr.isStandingPlay() then
        local headPose = vr.getPose(headPath)
        local playerHeight = vr.getPlayerHeight()
        local sneakOffset = I.VR.physicalSneakOffset()
        if (playerHeight - headPose.position.z).meters > sneakOffset then
            sneak = true
        end
    end
    
    if not sneakToggledOn then
        self.controls.sneak = sneak;
    else
        self.controls.sneak = sneakToggledOn
    end
end
    -- -- -- / VR EXTENSION END

local function processMovement()
    local controllerMovement = -input.getAxisValue(input.CONTROLLER_AXIS.MoveForwardBackward)
    local controllerSideMovement = input.getAxisValue(input.CONTROLLER_AXIS.MoveLeftRight)
    if controllerMovement ~= 0 or controllerSideMovement ~= 0 then
        -- controller movement
        if util.vector2(controllerMovement, controllerSideMovement):length2() < 0.25
           and not self.controls.sneak and Actor.isOnGround(self) and not Actor.isSwimming(self) then
            self.controls.run = false
            self.controls.movement = controllerMovement * 2
            self.controls.sideMovement = controllerSideMovement * 2
        else
            self.controls.run = true
            self.controls.movement = controllerMovement
            self.controls.sideMovement = controllerSideMovement
        end
    else
        -- keyboard movement
        self.controls.movement = 0
        self.controls.sideMovement = 0
        if input.isActionPressed(input.ACTION.MoveLeft) then
            self.controls.sideMovement = self.controls.sideMovement - 1
        end
        if input.isActionPressed(input.ACTION.MoveRight) then
            self.controls.sideMovement = self.controls.sideMovement + 1
        end
        if input.isActionPressed(input.ACTION.MoveBackward) then
            self.controls.movement = self.controls.movement - 1
        end
        if input.isActionPressed(input.ACTION.MoveForward) then
            self.controls.movement = self.controls.movement + 1
        end
        self.controls.run = input.isActionPressed(input.ACTION.Run) ~= settings:get('alwaysRun')
    end
    if self.controls.movement ~= 0 or not Actor.canMove(self) then
        autoMove = false
    elseif autoMove then
        self.controls.movement = 1
    end
    self.controls.jump = attemptJump and input.getControlSwitch(input.CONTROL_SWITCH.Jumping)
    
    -- VR EXTENSION BEGIN/ -- -- --
    -- The toggleSneak setting is replaced by separate Sneak, ToggleSneak, and physical sneak actions.
    processSneak()
    -- -- -- / VR EXTENSION END
end

local function onFrame(dt)
    controlsAllowed = input.getControlSwitch(input.CONTROL_SWITCH.Controls) and not core.isWorldPaused()
    if controlsAllowed then
        processMovement()
    else
        self.controls.movement = 0
        self.controls.sideMovement = 0
        self.controls.jump = false
    end
    attemptJump = false
    startAttack = false
end

local function onInputAction(action)
    if core.isWorldPaused() or not input.getControlSwitch(input.CONTROL_SWITCH.Controls) then
        return
    end

    if action == input.ACTION.Jump then
        attemptJump = true
    elseif action == input.ACTION.AutoMove then
        autoMove = not autoMove
    elseif action == input.ACTION.AlwaysRun then
        settings:set('alwaysRun', not settings:get('alwaysRun'))
       
    -- VR EXTENSION BEGIN/ -- -- --
    -- the toggleSneak setting is replaced by separate Sneak and ToggleSneak actions.
    -- This is because the most common VR binding for sneak (right thumbstick down) is expected to be a toggle.
    elseif action == input.ACTION.Sneak then
        -- Cancel out toggle sneak if it was currently toggled on
        sneakToggledOn = false
    elseif action == input.ACTION.ToggleSneak then
        sneakToggledOn = not sneakToggledOn
    -- -- -- / VR EXTENSION END
        
    end
end

return {
    engineHandlers = {
        onFrame = onFrame,
        onInputAction = onInputAction,
    }
}

