#include "vrbindings.hpp"

#include <tuple>

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>

#include <osg/io_utils>

#include <components/lua/luastate.hpp>
#include <components/lua/utilpackage.hpp>
#include <components/sdlutil/events.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/inputmanager.hpp"
#include "../mwinput/actions.hpp"

#include <components/vr/vr.hpp>
#include <components/vr/trackingmanager.hpp>

// TODO: Temp includes for temp interfaces
#include <components/esm3/loadsoun.hpp>
#include <components/esm3/loadweap.hpp>
#include "../mwbase/soundmanager.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/weapontype.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwlua/object.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/class.hpp"

#ifdef USE_OPENXR
#endif

namespace sol
{
    template <>
    struct is_automagical<SDL_Keysym> : std::false_type
    {
    };
}

namespace MWLua
{

    sol::table initVRPackage(const Context& context)
    {
        sol::table api(context.mLua->sol(), sol::create);
        
        api["TRACKING_STATUS"] = LuaUtil::makeStrictReadOnly(context.mLua->tableFromPairs<std::string_view, VR::TrackingStatus>({
            { "Unknown", VR::TrackingStatus::Unknown },
            { "Good", VR::TrackingStatus::Good },
            { "Stale", VR::TrackingStatus::Stale },
            { "NotTracked", VR::TrackingStatus::NotTracked },
            { "TimeInvalid", VR::TrackingStatus::TimeInvalid },
            { "Lost", VR::TrackingStatus::Lost },
            { "RuntimeFailure", VR::TrackingStatus::RuntimeFailure },
            }));

        sol::usertype<Stereo::Unit> lengthType = context.mLua->sol().new_usertype<Stereo::Unit>("Length");
        lengthType["meters"] = sol::readonly_property([](Stereo::Unit unit) -> float { return unit.asMeters(); });
        lengthType["mwunits"] = sol::readonly_property([](Stereo::Unit unit) -> float { return unit.asMWUnits(); });
        lengthType[sol::meta_function::unary_minus] = [](const Stereo::Unit& a) { return -a; };
        lengthType[sol::meta_function::addition] = [](const Stereo::Unit& a, const Stereo::Unit& b) { return a + b; };
        lengthType[sol::meta_function::subtraction]
            = [](const Stereo::Unit& a, const Stereo::Unit& b) { return a - b; };
        lengthType[sol::meta_function::equal_to] = [](const Stereo::Unit& a, const Stereo::Unit& b) { return a == b; };
        lengthType[sol::meta_function::multiplication] = [](const Stereo::Unit& a, float c) { return a * c; };
        lengthType[sol::meta_function::division] = [](const Stereo::Unit& a, float c) { return a / c; };
        lengthType["__tostring"] = [](Stereo::Unit a) -> std::string {
            std::stringstream ss;
            ss << "< Meters=" << a.asMeters() << ", MWUnits=" << a.asMWUnits() << " >";
            return ss.str();
        };

        api["PositionFromMeters"]
            = [](float x, float y, float z) -> Stereo::Position { return Stereo::Position::fromMeters(x, y, z); };
        api["PositionFromMWUnits"]
            = [](float x, float y, float z) -> Stereo::Position { return Stereo::Position::fromMWUnits(x, y, z); };

        sol::usertype<Stereo::Position> positionType = context.mLua->sol().new_usertype<Stereo::Position>("Position");
        positionType["meters"]
            = sol::readonly_property([](Stereo::Position position) -> osg::Vec3 { return position.asMeters(); });
        positionType["mwunits"]
            = sol::readonly_property([](Stereo::Position position) -> osg::Vec3 { return position.asMWUnits(); });
        positionType[sol::meta_function::unary_minus] = [](const Stereo::Position& a) { return -a; };
        positionType[sol::meta_function::addition]
            = [](const Stereo::Position& a, const Stereo::Position& b) { return a + b; };
        positionType[sol::meta_function::subtraction]
            = [](const Stereo::Position& a, const Stereo::Position& b) { return a - b; };
        positionType[sol::meta_function::equal_to]
            = [](const Stereo::Position& a, const Stereo::Position& b) { return a == b; };
        positionType[sol::meta_function::multiplication] = [](const Stereo::Position& a, float c) { return a * c; };
        positionType[sol::meta_function::division] = [](const Stereo::Position& a, float c) { return a / c; };
        positionType["x"] = sol::readonly_property([](const Stereo::Position& a) -> Stereo::Unit { return a.mX; });
        positionType["y"] = sol::readonly_property([](const Stereo::Position& a) -> Stereo::Unit { return a.mY; });
        positionType["z"] = sol::readonly_property([](const Stereo::Position& a) -> Stereo::Unit { return a.mZ; });
        positionType["__tostring"] = [](const Stereo::Position& a) -> std::string {
            std::stringstream ss;
            ss << "< Meters=" << a.asMeters() << ", MWUnits=" << a.asMWUnits() << " >";
            return ss.str();
        };


        sol::usertype<VR::TrackingPose> poseType = context.mLua->sol().new_usertype<VR::TrackingPose>("Pose");
        poseType["status"]
            = sol::readonly_property([](const VR::TrackingPose& pose) -> VR::TrackingStatus { return pose.status; });
        poseType["position"]
            = sol::readonly_property([](const VR::TrackingPose& pose) -> Stereo::Position { return pose.pose.position; });
        poseType["orientation"] = sol::readonly_property([](const VR::TrackingPose& pose) -> LuaUtil::TransformQ {
            return LuaUtil::asTransform(pose.pose.orientation);
        });
        poseType["__tostring"] = [](const VR::TrackingPose& pose) -> std::string {
            std::stringstream ss;
            ss << "< status=" << static_cast<int>(pose.status) << ", pose=" << pose.pose << " >";
            return ss.str();
        };

        api["isVr"] = []() -> bool { return VR::getVR(); };
        api["isLeftHandControllerEnabled"] = []() -> bool { return VR::getLeftControllerActive(); };
        api["isRightHandControllerEnabled"] = []() -> bool { return VR::getRightControllerActive(); };
        api["isKeyboardMouseMode"] = []() -> bool { return VR::getKBMouseModeActive(); };
        api["isSeatedPlay"] = []() -> bool { return VR::getSeatedPlay(); };
        api["isStandingPlay"] = []() -> bool { return VR::getStandingPlay(); };
        api["getPlayerHeight"] = []() -> Stereo::Unit { return VR::getPlayerHeight(); };
        api["recenter"] = []() -> void { return VR::recenter(); };
        api["resetEyeLevel"] = []() -> void { return VR::resetEyeLevel(); };
        api["stringToPath"] = [](std::string_view pathString) -> VR::VRPath { return VR::stringToVRPath(pathString); };
        api["pathToString"] = [](VR::VRPath path) -> std::string_view { return VR::VRPathToString(path); };
        api["getPose"] = [](VR::VRPath path) -> VR::TrackingPose { return VR::getTrackedPose(path); };
        api["fromMeters"] = [](float meters) -> Stereo::Unit { return Stereo::Unit::fromMeters(meters); };
        api["fromMWUnits"] = [](float mwUnits) -> Stereo::Unit { return Stereo::Unit::fromMWUnits(mwUnits); };

        // TODO: These are temporary interfaces until OpenMW-Lua exposes equivalents
        api["TEMPINTERFACE_playSwish"] = [](float strength, const Object& object) -> void {
            static ESM::RefId weaponSwish = ESM::RefId::stringRefId("Weapon Swish");
            const ESM::RefId* soundId = &weaponSwish;
            float volume = 0.98f + strength * 0.02f;
            float pitch = 0.75f + strength * 0.4f;
            auto ptr = object.ptr();

            const MWWorld::Class& cls = ptr.getClass();
            if (cls.isNpc() && cls.getNpcStats(ptr).isWerewolf())
            {
                MWBase::World* world = MWBase::Environment::get().getWorld();
                const MWWorld::ESMStore& store = world->getStore();
                const ESM::Sound* sound = store.get<ESM::Sound>().searchRandom("WolfSwing", world->getPrng());
                if (sound)
                    soundId = &sound->mId;
            }

            if (!soundId->empty())
                MWBase::Environment::get().getSoundManager()->playSound3D(ptr, *soundId, volume, pitch);
        };

        api["TEMPINTERFACE_evaluateHit"]
            = [](const Object& attacker, const osg::Vec3f& origin,
                  const LuaUtil::TransformQ& originOrientation) -> std::tuple<bool, sol::optional<Object>, osg::Vec3> {
            MWWorld::Ptr victim;
            osg::Vec3 hitPosition;
            auto ptr = attacker.ptr();
            bool success = ptr.getClass().evaluateHit(ptr, victim, hitPosition, origin, originOrientation.mQ);

            if (victim.isEmpty())
                return std::make_tuple(success, sol::nullopt, hitPosition);
            return std::make_tuple(success, Object(getId(victim)), hitPosition);
        };

        api["TEMPINTERFACE_hit"] = [](const Object& attacker, float strength, int attackType,
                                       sol::optional<Object> victim,
                                       osg::Vec3 hitPosition, bool success) {
            auto victimPtr = victim.has_value() ? victim->ptr() : MWWorld::Ptr();
            attacker.ptr().getClass().hit(attacker.ptr(), strength, attackType, victimPtr, hitPosition, success);
        };

        return LuaUtil::makeReadOnly(api);
    }

}
