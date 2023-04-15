#include "trackingmanager.hpp"
#include "frame.hpp"
#include "trackinglistener.hpp"
#include "trackingsource.hpp"

#include <components/debug/debuglog.hpp>
#include <components/settings/settings.hpp>

#include <cassert>

namespace VR
{
    TrackingManager* sManager = nullptr;

    TrackingManager& TrackingManager::instance()
    {
        assert(sManager);
        return *sManager;
    }

    TrackingManager::TrackingManager()
    {
        if (!sManager)
            sManager = this;
        else
            throw std::logic_error("Duplicated VR::TrackingManager singleton");

        mHandDirectedMovement = Settings::Manager::getBool("hand directed movement", "VR");
        mHeadPath = stringToVRPath("/stage/user/head/input/pose");
        mHandPath = stringToVRPath("/stage/user/hand/left/input/aim/pose");
    }

    TrackingManager::~TrackingManager()
    {
        sManager = nullptr;
    }

    void TrackingManager::addListener(TrackingListener* listener)
    {
        mListeners.push_back(listener);
    }

    void TrackingManager::removeListener(TrackingListener* listener)
    {
        for (auto it = mListeners.begin(); it != mListeners.end();)
            if (*it == listener)
                it = mListeners.erase(it);
            else
                it++;
    }

    void TrackingManager::movementAngles(float& yaw, float& pitch)
    {
        yaw = mMovementYaw;
        pitch = mMovementPitch;
    }

    void TrackingManager::processChangedSettings(const std::set<std::pair<std::string, std::string>>& changed)
    {
        mHandDirectedMovement = Settings::Manager::getBool("hand directed movement", "VR");
    }

    TrackingPose TrackingManager::locate(VRPath path, DisplayTime predictedDisplayTime) const
    {
        auto it = mPathToSourceMap.find(path);
        if (it != mPathToSourceMap.end())
            return it->second->locate(path, predictedDisplayTime);

        TrackingPose pose = TrackingPose();
        pose.status = TrackingStatus::NotTracked;
        return pose;
    }

    TrackingSource* TrackingManager::getTrackingSource(VRPath path) const
    {
        for (auto* source : mSources)
            if (source->path() == path)
                return source;
        return nullptr;
    }

    const std::set<VRPath>& TrackingManager::availablePaths() const
    {
        return mAvailablePaths;
    }

    void TrackingManager::registerTrackingSource(TrackingSource* source)
    {
        mSources.emplace_back(source);
    }

    void TrackingManager::unregisterTrackingSource(TrackingSource* source)
    {
        for (auto it = mSources.begin(); it != mSources.end(); it++)
        {
            if (*it == source)
            {
                it = mSources.erase(it);
            }

            if (it == mSources.end())
                break;
        }
    }

    void TrackingManager::updateTracking(const VR::Frame& frame)
    {
        if (frame.predictedDisplayTime == 0)
            return;

        checkAvailablePathsChanged();
        updateMovementAngles(frame.predictedDisplayTime);

        for (auto* source : mSources)
            source->updateTracking(frame.predictedDisplayTime);

        for (auto* listener : mListeners)
            listener->onTrackingUpdated(*this, frame.predictedDisplayTime);
    }

    void TrackingManager::updateMovementAngles(DisplayTime predictedDisplayTime)
    {
        if (mHandDirectedMovement)
        {
            auto tpHead = locate(mHeadPath, predictedDisplayTime);
            auto tpHand = locate(mHandPath, predictedDisplayTime);

            if (!!tpHead.status && !!tpHand.status)
            {
                float headYaw = 0.f;
                float headPitch = 0.f;
                float headsWillRoll = 0.f;

                float handYaw = 0.f;
                float handPitch = 0.f;
                float handRoll = 0.f;
                Stereo::getEulerAngles(tpHead.pose.orientation, headYaw, headPitch, headsWillRoll);
                Stereo::getEulerAngles(tpHand.pose.orientation, handYaw, handPitch, handRoll);

                mMovementYaw = handYaw - headYaw;
                mMovementPitch = handPitch - headPitch;
            }
        }
        else
        {
            mMovementYaw = 0;
            mMovementPitch = 0;
        }
    }

    void TrackingManager::checkAvailablePathsChanged()
    {
        bool availablePosesChanged = false;

        for (auto source : mSources)
            availablePosesChanged |= source->availablePosesChanged();

        if (availablePosesChanged)
            updateAvailablePaths();

        for (auto source : mSources)
            source->clearAvailablePosesChanged();
    }

    void TrackingManager::updateAvailablePaths()
    {
        mAvailablePaths.clear();
        mPathToSourceMap.clear();
        for (auto source : mSources)
        {
            auto paths = source->listSupportedPaths();
            mAvailablePaths.insert(paths.begin(), paths.end());
            for (auto& path : paths)
            {
                mPathToSourceMap.emplace(path, source);
            }
        }
        for (auto* listener : mListeners)
            listener->onAvailablePathsChanged(mAvailablePaths);
    }

}
