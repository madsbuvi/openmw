#ifndef VR_TRACKING_LISTENER_H
#define VR_TRACKING_LISTENER_H

#include <components/vr/trackingpath.hpp>

#include <set>

namespace VR
{
    class TrackingManager;

    class TrackingListener
    {
    public:
        TrackingListener();
        virtual ~TrackingListener();

        //! Notify that available tracking poses have changed.
        virtual void onAvailablePathsChanged(const std::set<VRPath>& paths){};

        //! Called every frame, after tracking poses have been updated
        virtual void onTrackingUpdated(TrackingManager& manager, DisplayTime predictedDisplayTime) = 0;

    private:
    };
}

#endif
