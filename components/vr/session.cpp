#include "session.hpp"

#include <components/debug/debuglog.hpp>
#include <components/misc/constants.hpp>
#include <components/sdlutil/sdlgraphicswindow.hpp>
#include <components/vr/trackingpath.hpp>
#include <components/vr/trackingsource.hpp>
#include <components/vr/vr.hpp>

#include <osg/Camera>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <time.h>
#include <vector>

namespace VR
{
    Session* sSession = nullptr;

    Session& Session::instance()
    {
        assert(sSession);
        return *sSession;
    }

    Session::Session()
    {
        if (!sSession)
            sSession = this;
        else
            throw std::logic_error("Duplicated VR::Session singleton");

        readSettings();

        auto stageUserHeadPath = VR::stringToVRPath("/stage/user/head/input/pose");
        auto worldUserPath = VR::stringToVRPath("/world/user");
        auto worldUserHeadPath = VR::stringToVRPath("/world/user/head/input/pose");
        mTrackerToWorldBinding = std::make_unique<VR::StageToWorldBinding>(worldUserPath, stageUserHeadPath);
        mTrackerToWorldBinding->bindPaths(worldUserHeadPath, stageUserHeadPath);
    }

    Session::~Session() {}

    void Session::processChangedSettings(const std::set<std::pair<std::string, std::string>>& changed)
    {
        readSettings();
    }

    VR::Frame Session::newFrame()
    {
        static uint64_t frameNo = 0;
        VR::Frame frame;
        frame.frameNumber = frameNo++;
        newFrame(frame.frameNumber, frame.shouldSyncFrameLoop, frame.shouldSyncInput);
        return frame;
    }

    void Session::frameBeginUpdate(VR::Frame& frame)
    {
        if (frame.shouldSyncFrameLoop)
            syncFrameUpdate(
                frame.frameNumber, frame.shouldRender, frame.predictedDisplayTime, frame.predictedDisplayPeriod);
    }

    void Session::frameBeginRender(VR::Frame& frame)
    {
        if (frame.shouldSyncFrameLoop)
            syncFrameRender(frame);
    }

    void Session::frameEnd(osg::GraphicsContext* gc, VR::Frame& frame)
    {
        gc->swapBuffersImplementation();
        if (frame.shouldSyncFrameLoop)
            syncFrameEnd(frame);
    }

    void Session::readSettings()
    {
        auto seatedPlay = Settings::Manager::getBool("seated play", "VR");
        if (VR::getSeatedPlay() != seatedPlay)
        {
            VR::setSeatedPlay(seatedPlay);
            requestRecenter(true);
        }

        mHandDirectedMovement = Settings::Manager::getBool("hand directed movement", "VR");

        mHandsOffset.x() = (Settings::Manager::getFloat("hands offset x", "VR") - 0.5) * Constants::UnitsPerMeter;
        mHandsOffset.y() = (Settings::Manager::getFloat("hands offset y", "VR") - 0.5) * Constants::UnitsPerMeter;
        mHandsOffset.z() = (Settings::Manager::getFloat("hands offset z", "VR") - 0.5) * Constants::UnitsPerMeter;
    }

    void Session::computePlayerScale()
    {
        mPlayerHeight = Stereo::Unit::fromMeters(Settings::Manager::getFloat("player height", "VR"));
        Log(Debug::Verbose) << "Read player height: " << mPlayerHeight.asMeters();
        mPlayerScale = mCharHeight / mPlayerHeight;
        Log(Debug::Verbose) << "Calculated player scale: " << mPlayerScale;
        requestRecenter(true);
    }

    void Session::setCharHeight(Stereo::Unit height)
    {
        Log(Debug::Verbose) << "Set char height: " << height.asMeters();
        mCharHeight = height;
        computePlayerScale();
    }

    void Session::requestRecenter(bool recenterZ)
    {
        if (mTrackerToWorldBinding)
        {
            Log(Debug::Verbose) << "Recentering (recenterZ=" << recenterZ << ")";
            mTrackerToWorldBinding->setEyeLevel(charHeight());
            mTrackerToWorldBinding->recenter(recenterZ);
        }
        else
            Log(Debug::Warning) << "Recenter was requested, but session was not ready";
    }

    void Session::instantTransition()
    {
        if (mTrackerToWorldBinding)
            mTrackerToWorldBinding->instantTransition();
        else
            Log(Debug::Warning) << "Instant transition was requested, but session was not ready";
    }

    VR::StageToWorldBinding& Session::stageToWorldBinding()
    {
        return *mTrackerToWorldBinding;
    }

    void Session::setInteractionProfileActive(VRPath topLevelPath, bool active)
    {
        if (topLevelPath == stringToVRPath("/user/hand/left"))
            setLeftControllerActive(active);
        if (topLevelPath == stringToVRPath("/user/hand/right"))
            setRightControllerActive(active);

        if (active)
            mActiveInteractionProfiles.insert(topLevelPath);
        else
            mActiveInteractionProfiles.erase(topLevelPath);
    }

    bool Session::getInteractionProfileActive(VRPath topLevelPath) const
    {
        return !!mActiveInteractionProfiles.count(topLevelPath);
    }

    void Session::setSneak(bool sneak)
    {
        if (sneak)
            mSneakOffset = Stereo::Unit::fromMWUnits(-20.f);
        else
            mSneakOffset = {};
    }
}
