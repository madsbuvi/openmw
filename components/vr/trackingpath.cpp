#include "trackingpath.hpp"
#include <components/debug/debuglog.hpp>

namespace VR
{
    std::map<std::string, VRPath> sPathIdentifiers;

    VRPath newPath(const std::string& path)
    {
        VRPath vrPath = sPathIdentifiers.size() + 1;
        auto res = sPathIdentifiers.emplace(path, vrPath);
        return res.first->second;
    }

    VRPath stringToVRPath(const std::string& path)
    {
        // Empty path is invalid
        if (path.empty())
        {
            Log(Debug::Error) << "VR::stringToVRPath: Empty path";
            return VRPath();
        }

        // Return path immediately if it already exists
        auto it = sPathIdentifiers.find(path);
        if (it != sPathIdentifiers.end())
            return it->second;

        // Add new path and return it
        return newPath(path);
    }

    std::string VRPathToString(VRPath path)
    {
        // Find the identifier in the map and return the corresponding string.
        for (auto& e : sPathIdentifiers)
            if (e.second == path)
                return e.first;

        // No path found, return empty string
        Log(Debug::Warning) << "VR::VRPathToString: No such path identifier (" << path << ")";
        return "";
    }
}
