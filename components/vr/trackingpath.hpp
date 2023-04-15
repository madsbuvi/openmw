#ifndef VR_TRACKINGPATH_H
#define VR_TRACKINGPATH_H

#include <array>
#include <map>
#include <memory>
#include <queue>
#include <string>

#include <components/misc/constants.hpp>
#include <components/stereo/types.hpp>
#include <components/vr/constants.hpp>

namespace osg
{
    class Transform;
}

namespace Stereo
{
    struct View;
}

namespace VR
{
    //! @brief An identifier representing an path, with 0 representing no path.
    //! A VRPath can be optained from the string representation of a path using VRTrackingManager::stringToVRPath()
    //! \note Support is determined by each VRTrackingSource. ALL strings are convertible to VRPaths but won't be useful
    //! unless they match a supported string. \sa VRTrackingManager::getTrackingPath()
    using VRPath = uint32_t;

    //! Display time as a 64bit integer. Units and reference are undefined, value is as provided by the VR runtime.
    using DisplayTime = int64_t;

    //! A single tracked pose
    struct TrackingPose
    {
        TrackingStatus status = TrackingStatus::Unknown; //!< State of the prediction.
        Stereo::Pose pose = {}; //!< The predicted pose.
        DisplayTime time = 0; //!< The time for which the pose was predicted.
    };

    //! Converts a string representation of a path to a VRTrackerPath identifier
    VRPath stringToVRPath(const std::string& path);

    //! Converts a path identifier back to string. Returns an empty string if no such identifier exists.
    std::string VRPathToString(VRPath path);
}

#endif
