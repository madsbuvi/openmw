#ifndef VR_H
#define VR_H

namespace VR
{
    bool getVR();
    bool getKBMouseModeActive();
    bool getLeftControllerActive();
    bool getRightControllerActive();
    bool getSteamVR();
    bool getSeatedPlay();
    bool getStandingPlay();

    void setVR(bool VR);
    void setLeftControllerActive(bool active);
    void setRightControllerActive(bool active);
    void setSteamVR(bool steamVR);
    void setSeatedPlay(bool seated);
}

#endif
