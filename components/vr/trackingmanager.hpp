#ifndef VR_TRACKING_MANAGER_H
#define VR_TRACKING_MANAGER_H

#include <components/vr/trackingpath.hpp>

#include <list>
#include <map>
#include <set>
#include <vector>

namespace VR
{
    class TrackingListener;
    class TrackingSource;
    struct Frame;

    class TrackingManager
    {
    public:
        static TrackingManager& instance();

        TrackingManager();
        ~TrackingManager();

        //! Angles to be used for overriding movement direction
        // void movementAngles(float& yaw, float& pitch);

        void updateTracking(const VR::Frame& frame);

        //! Bind listener to source, listener will receive tracking updates from source until unbound.
        //! \note A single listener can only receive tracking updates from one source.
        void addListener(TrackingListener* listener);

        //! Unbind listener, listener will no longer receive tracking updates.
        void removeListener(TrackingListener* listener);

        //! Angles to be used for overriding movement direction
        void movementAngles(float& yaw, float& pitch);

        void processChangedSettings(const std::set<std::pair<std::string, std::string>>& changed);

        TrackingPose locate(VRPath path, DisplayTime predictedDisplayTime) const;

        TrackingSource* getTrackingSource(VRPath path) const;

        const std::set<VRPath>& availablePaths() const;

    private:
        friend class TrackingSource;
        void registerTrackingSource(TrackingSource* source);
        void unregisterTrackingSource(TrackingSource* source);
        void updateMovementAngles(DisplayTime predictedDisplayTime);
        void checkAvailablePathsChanged();
        void updateAvailablePaths();

        std::vector<TrackingSource*> mSources;
        std::map<VRPath, TrackingSource*> mPathToSourceMap;
        std::set<VRPath> mAvailablePaths;
        std::list<TrackingListener*> mListeners;

        bool mHandDirectedMovement = 0.f;
        VRPath mHeadPath = VRPath();
        VRPath mHandPath = VRPath();
        float mMovementYaw = 0.f;
        float mMovementPitch = 0.f;
    };
}

#endif
