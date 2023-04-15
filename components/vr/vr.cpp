#include "vr.hpp"

namespace VR
{

    namespace
    {
        bool sVRMode = false;
        bool sSteamVR = false;
        bool sLeftControllerActive = false;
        bool sRightControllerActive = false;
        bool sSeatedPlay = false;
    }

    bool getVR()
    {
        return sVRMode;
    }

    bool getKBMouseModeActive()
    {
        return !(sLeftControllerActive || sRightControllerActive);
    }

    bool getLeftControllerActive()
    {
        return sLeftControllerActive;
    }

    bool getRightControllerActive()
    {
        return sRightControllerActive;
    }

    bool getSteamVR()
    {
        return sSteamVR;
    }

    bool getSeatedPlay()
    {
        return sSeatedPlay;
    }

    bool getStandingPlay()
    {
        return !sSeatedPlay;
    }

    void setVR(bool VR)
    {
        sVRMode = VR;
    }

    void setLeftControllerActive(bool active)
    {
        sLeftControllerActive = active;
    }

    void setRightControllerActive(bool active)
    {
        sRightControllerActive = active;
    }

    void setSteamVR(bool steamVR)
    {
        sSteamVR = steamVR;
    }

    void setSeatedPlay(bool seated)
    {
        sSeatedPlay = seated;
    }

}
