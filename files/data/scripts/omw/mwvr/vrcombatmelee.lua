    -- VR CONTROLS -- -- --
    -- When loaded, this file implements VR controls and overrides some of playercontrols.lua with VR specific behavior.
    
local core = require('openmw.core')
local input = require('openmw.input')
local self = require('openmw.self')
local util = require('openmw.util')
local ui = require('openmw.ui')
local types = require('openmw.types')
local Player = types.Player
local Weapon = types.Weapon
local vr = require('openmw.vr')

local storage = require('openmw.storage')
local I = require('openmw.interfaces')

-- Use position and orientation in the VR stage to reason about hand motion, otherwise the character running fast will start registering as combat
local stageGripPath = vr.stringToPath("/stage/user/hand/right/input/grip/pose");
local stageAimPath = vr.stringToPath("/stage/user/hand/right/input/aim/pose");
local worldGripPath = vr.stringToPath("/world/user/hand/right/input/grip/pose");
local worldAimPath = vr.stringToPath("/world/user/hand/right/input/aim/pose");

 -- TODO: Placeholder enum for attack type
local ATTACK_TYPE = { Chop = 0, Slash = 1, Thrust = 2 }

local STATE = { Ready = 0, Launching = 1, Swinging = 2, Impact = 3, Cooldown = 4 }
 -- Current state of swinging.
local state = STATE.Ready
 -- How long (in seconds) we've been in the current state
local stateTime = 0
 -- How long (in ? units) we've moved since entering this state
local stateMovement = 0
 -- Highest velocity seen during ongoing swing
local maxVelocity = 0
 -- Current velocity
local velocity = 0
 --
local weaponPos = vr.PositionFromMeters(0,0,0)

local enabled = false
local isHandToHand = false
local weaponType = 0
local thrustVelocity = 0
local attackType = ATTACK_TYPE.Chop

local function changeState(newState)
    state = newState
    maxVelocity = 0
    stateTime = 0
    stateMovement = 0
end

local function resetCombat()
    changeState(STATE.Ready)
    velocity = 0
    weaponPos = vr.PositionFromMeters(0,0,0)
end

local function setEnabled(arg)
    if enabled ~= arg then
        resetCombat()
        enabled = arg
    end
end

local meleeTypes = {
    Weapon.TYPE.AxeOneHand, 
    Weapon.TYPE.AxeTwoHand, 
    Weapon.TYPE.BluntOneHand, 
    Weapon.TYPE.BluntTwoClose, 
    Weapon.TYPE.BluntTwoWide, 
    Weapon.TYPE.LongBladeOneHand, 
    Weapon.TYPE.LongBladeTwoHand, 
    Weapon.TYPE.ShortBladeOneHand, 
    Weapon.TYPE.SpearTwoWide}
local function isMeleeType(arg)
    for index = 1, #meleeTypes do

        if meleeTypes[index] == arg then
            return true
        end
    end
    return false
end

local function isMeleeEnabled()
    
    if Player.getStance(self) ~= Player.STANCE.Weapon then
        -- Weapon not drawn, ergo melee not enabled
        return false;
    end

    local carriedRight = Player.getEquipment(self, Player.EQUIPMENT_SLOT.CarriedRight)

    isHandToHand = carriedRight == nil
    if isHandToHand then
        return true
    end

    if Weapon.objectIsInstance(carriedRight) then
        -- Melee combat is enabled if the carried object is a melee weapon only
        weaponType = Weapon.record(carriedRight.recordId).type
        return isMeleeType(weaponType)
    end

    return false
end

local function processMovement(dt)
    local handPose

    if(isHandToHand) then
        handPose = vr.getPose(stageAimPath)
    else
        handPose = vr.getPose(stageGripPath)
    end
    
    local weaponDir  = handPose.orientation
    local thrustDirection = weaponDir * util.vector3(0, 1, 0)
    local slashChopDirection = weaponDir * util.vector3(0, 0, 1)
    -- TODO: Side attack unused. Should be usable for blunt weapons
    local sideDirection = weaponDir * util.vector3(1, 0, 0)

    -- Combat was reset or tracking was lost, ergo we don't have a previous position.
    -- Update positin and return
    if weaponPos == vr.PositionFromMeters(0,0,0) then
        print('resetting weaponPos')
        weaponPos = handPose.position
        return
    end
    
    -- Compute how much the hand moved since last frame
    local movement = handPose.position.meters - weaponPos.meters
    stateMovement = stateMovement + movement:length()
    weaponPos = handPose.position

    -- Compute speed by differentiating over time
    local swingVector = movement / dt;
    local swingDirection = swingVector:normalize();
    velocity = swingVector:length()
    maxVelocity = math.max(maxVelocity, math.abs(velocity))

    -- Break speed into thrust/slash components
    thrustVelocity = swingVector * thrustDirection
    local slashChopVelocity = math.abs(swingVector * slashChopDirection)
    local sideVelocity = math.abs(swingVector * sideDirection)

    -- Determine attack types by comparing velocities and verticality
    if math.abs(thrustVelocity) > slashChopVelocity then
        attackType = ATTACK_TYPE.Thrust
    else
        -- If the weapon is held upwards or is being swung more vertically than horizontally,
        -- then the attack is a chop, otherwise it is a slash.
        local weaponOrientationVerticality = math.abs(thrustDirection.z)
        local swingVerticality = math.abs(swingDirection.z)
        if weaponOrientationVerticality > 0.707 or swingVerticality > 0.707 then
            attackType = ATTACK_TYPE.Chop
        else
            attackType = ATTACK_TYPE.Slash
        end
    end
end

local function validSwingMotion()
    if velocity > I.VR.realisticCombatMinVelocity() then
        if attackType == ATTACK_TYPE.Thrust then
            -- Backwards thrust should not trigger an attack (the player is pulling back)
            return thrustVelocity > 0
        end
        return true
    end
    return false

end

local function computeAttackStrength()
    local min = I.VR.realisticCombatMinVelocity()
    local max = I.VR.realisticCombatMaxVelocity()
    return math.max(0, math.min(1, (maxVelocity - min) / (max - min)))
end

local function update_readyState()
    -- We go from ready to launch as soon as the player's motion is fast enough
    -- to be considered for a swing
    if validSwingMotion() then
        changeState(STATE.Launching)
    end
end

local function update_launchingState()
    -- Once the player has maintained a valid swing motion for long enough,
    -- transition to STATE.Swinging which will start checking for impact.
    if not validSwingMotion() then
        changeState(STATE.Ready)
    elseif stateMovement > I.VR.realisticCombatMinLength() then
        local strength = computeAttackStrength()
    -- TODO: Using temporary interface
        vr.TEMPINTERFACE_playSwish(strength, self)
        changeState(STATE.Swinging)
    end
end

local function makeImpact(victim, hitPosition, success)
    local strength = computeAttackStrength()
    -- TODO: Using temporary interface
    vr.TEMPINTERFACE_hit(self, strength, attackType, victim, hitPosition, success)
    changeState(STATE.Impact)
end

local function update_swingingState()
    if not validSwingMotion() then
        -- Player is no longer maintaining a valid swing motion, end the swing as a miss.
        makeImpact(nil, util.vector3(0,0,0), false)
    else
        
        -- We are now checking for an actual hit, ergo we have to use the hand's position in the world rather than the stage
        local worldPose
        if(isHandToHand) then
            worldPose = vr.getPose(worldAimPath)
        else
            worldPose = vr.getPose(worldGripPath)
        end
        
        -- TODO: Using temporary interface
        -- This interface forwards a call to player ptr . evaluateHit() which in this fork has been extended to take an optional custom origin
        local success, victim, hitPosition = vr.TEMPINTERFACE_evaluateHit(self, worldPose.position.mwunits, worldPose.orientation)
        if victim then
            makeImpact(victim, hitPosition, success)
        end
    end
end

local function update_impactState()
    if(velocity < I.VR.realisticCombatMinVelocity()) then
        changeState(STATE.Cooldown)
    end
end

local function update_cooldownState()
    if stateTime >= I.VR.realisticCombatMinInterval() then
        changeState(STATE.Ready)
    end
end


local function processMeleeCombat(dt)
    local meleeEnabled = isMeleeEnabled()
    if meleeEnabled ~= enabled then
        resetCombat()
        enabled = meleeEnabled
    end

    if not meleeEnabled then return end
    
    processMovement(dt)

    if state == STATE.Ready then
        update_readyState()
    elseif state == STATE.Launching then
        update_launchingState()
    elseif state == STATE.Swinging then
        update_swingingState()
    elseif state == STATE.Impact then
        update_impactState()
    elseif state == STATE.Cooldown then
        update_cooldownState()
    end

end

local function onUpdate(dt)
    fightingAllowed = input.getControlSwitch(input.CONTROL_SWITCH.Fighting) and not core.isWorldPaused()
    if fightingAllowed then
        stateTime = stateTime + dt
        processMeleeCombat(dt)
    else
        resetCombat()
    end
end

return {
    engineHandlers = {
        onUpdate = onUpdate
    }
}

