#ifndef OPENMW_COMPONENTS_SETTINGS_CATEGORIES_VR_H
#define OPENMW_COMPONENTS_SETTINGS_CATEGORIES_VR_H

#include "components/settings/sanitizerimpl.hpp"
#include "components/settings/settingvalue.hpp"

#include <osg/Math>
#include <osg/Vec2f>
#include <osg/Vec3f>

#include <cstdint>
#include <string>
#include <string_view>

namespace Settings
{
    struct VRCategory
    {
        SettingValue<float> mPlayerHeight { "VR", "player height", makeMaxStrictSanitizerFloat(0.1) };
        SettingValue<bool> mMirrorTexture{ "VR", "mirror texture" };
        SettingValue<std::string> mMirrorTextureEye{ "VR", "mirror texture eye",
            makeEnumSanitizerString({ "left", "right", "both" }) };
        SettingValue<bool> mFlipMirrorTextureOrder{ "VR", "flip mirror texture order" };
        SettingValue<std::string> mLeftEyeResolutionX{ "VR", "left eye resolution x" };
        SettingValue<std::string> mLeftEyeResolutionY{ "VR", "left eye resolution y" };
        SettingValue<std::string> mRightEyeResolutionX{ "VR", "right eye resolution x" };
        SettingValue<std::string> mRightEyeResolutionY{ "VR", "right eye resolution y" };
        SettingValue<float> mRealisticCombatMinimumSwingVelocity{ "VR", "realistic combat minimum swing velocity",
            makeMaxStrictSanitizerFloat(0.1) };
        SettingValue<float> mRealisticCombatMaximumSwingVelocity{ "VR", "realistic combat maximum swing velocity",
            makeMaxStrictSanitizerFloat(0.1) };
        SettingValue<bool> mHapticsEnabled{ "VR", "haptics enabled" };
        SettingValue<bool> mHandDirectedMovement{ "VR", "hand directed movement" };
        SettingValue<std::string> mHudPosition{ "VR", "hud position",
            makeEnumSanitizerString({ "left wrist inner", "right wrist inner", "left wrist top", "right wrist top",
                "top left", "top right", "bottom left", "bottom right" }) };
        SettingValue<std::string> mTooltipPosition{ "VR", "tooltip position",
            makeEnumSanitizerString({ "left wrist inner", "right wrist inner", "left wrist top", "right wrist top",
                "top left", "top right", "bottom left", "bottom right" }) };
        SettingValue<float> mHudOffsetX{ "VR", "hud offset x" };
        SettingValue<float> mHudOffsetY{ "VR", "hud offset y" };
        SettingValue<float> mHudoffsetZ{ "VR", "hud offset z" };
        SettingValue<float> mTooltipOffsetX{ "VR", "tooltip offset x" };
        SettingValue<float> mTooltipOffsetY{ "VR", "tooltip offset y" };
        SettingValue<float> mTooltipOffsetZ{ "VR", "tooltip offset z" };
        SettingValue<float> mHandsOffsetX{ "VR", "hands offset x" };
        SettingValue<float> mHandsOffsetY{ "VR", "hands offset y" };
        SettingValue<float> mHandsOffsetZ{ "VR", "hands offset z" };
        SettingValue<bool> mSeatedPlay{ "VR", "seated play" };
        SettingValue<bool> mSmoothTurning{ "VR", "smooth turning" };
        SettingValue<float> mSnapAngle{ "VR", "snap angle" };
        SettingValue<float> mSmoothTurnRate{ "VR", "smooth turn rate" };
        SettingValue<bool> mLeftHandedMode{ "VR", "left handed mode" };
        SettingValue<bool> mIntroSequenceComplete{ "VR", "intro sequence complete" };
        SettingValue<std::string> mUtilityAxisUpAction{ "VR", "utility axis up action" };
        SettingValue<std::string> mUtilityAxisDownAction{ "VR", "utility axis down action" };
        SettingValue<bool> mPhysicalSneakEnabled{ "VR", "physical sneak enabled" };
        SettingValue<float> mPhysicalSneakHeightOffset{ "VR", "physical sneak height offset" };
        SettingValue<bool> mShow3DCrosshairs{ "VR", "show 3D crosshairs" };
        SettingValue<bool> mUseXrLayerForHuds{ "VR", "use xr layer for huds" };
    };
    struct VRDebugCategory
    {
        SettingValue<bool> mLogAllOpenxrCalls{ "VR Debug", "log all openxr calls" };
        SettingValue<bool> mContinueOnErrors{ "VR Debug", "continue on errors" };
        SettingValue<bool> mDisableXR_KHR_opengl_enable{ "VR Debug", "disable XR_KHR_opengl_enable" };
        SettingValue<bool> mDisableXR_KHR_D3D11_enable{ "VR Debug", "disable XR_KHR_D3D11_enable" };
        SettingValue<bool> mDisableXR_KHR_composition_layer_depth{ "VR Debug",
            "disable XR_KHR_composition_layer_depth" };
        SettingValue<bool> mDisableXR_EXT_debug_utils{ "VR Debug", "disable XR_EXT_debug_utils" };
        SettingValue<bool> mDisableXR_EXT_hp_mixed_reality_controller{ "VR Debug",
            "disable XR_EXT_hp_mixed_reality_controller" };
        SettingValue<bool> mDisableXR_HTC_vive_cosmos_controller_interaction{ "VR Debug",
            "disable XR_HTC_vive_cosmos_controller_interaction" };
        SettingValue<bool> mDisableXR_HUAWEI_controller_interaction{ "VR Debug",
            "disable XR_HUAWEI_controller_interaction" };
        SettingValue<bool> mDisableXR_MSFT_composition_layer_reprojection{ "VR Debug",
            "disable XR_MSFT_composition_layer_reprojection" };
        SettingValue<bool> mDisableXR_FB_space_warp{ "VR Debug", "disable XR_FB_space_warp" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageLevelVerbose{ "VR Debug",
            "XR_EXT_debug_utils message level verbose" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageLevelInfo{ "VR Debug", "XR_EXT_debug_utils message level info" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageLevelWarning{ "VR Debug",
            "XR_EXT_debug_utils message level warning" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageLevelError{ "VR Debug",
            "XR_EXT_debug_utils message level error" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageTypeGeneral{ "VR Debug",
            "XR_EXT_debug_utils message type general" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageTypeValidation{ "VR Debug",
            "XR_EXT_debug_utils message type validation" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageTypePerformance{ "VR Debug",
            "XR_EXT_debug_utils message type performance" };
        SettingValue<bool> mXR_EXT_debug_utilsMessageTypeConformance{ "VR Debug",
            "XR_EXT_debug_utils message type conformance" };
        SettingValue<float> mCharacterBaseHeight{ "VR Debug", "character base height",
            makeMaxStrictSanitizerFloat(0.1) };
        SettingValue<bool> mSubmitStencilFormats{ "VR Debug", "submit stencil formats" };
        SettingValue<bool> mDisableTextureFormat{ "VR Debug", "disable texture format" };
        SettingValue<bool> mSkywindBlasterWorkaround{ "VR Debug", "skywind blaster workaround" };
    };
}

#endif
