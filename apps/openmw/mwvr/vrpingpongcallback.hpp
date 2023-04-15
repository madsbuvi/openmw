#ifndef MWVR_PINGPONGCALLBACK_H
#define MWVR_PINGPONGCALLBACK_H

#include "../mwrender/npcanimation.hpp"
#include "../mwrender/pingpongcanvas.hpp"
#include "../mwrender/postprocessor.hpp"
#include "../mwrender/renderingmanager.hpp"

#include <components/vr/trackinglistener.hpp>
#include <osg/ref_ptr>

namespace osg
{
    class Viewport;
}

namespace MWVR
{
    //! PingPongCallback that takes care of connecting the post processor output to VR
    class PingPongCallback : public MWRender::PingPongCanvas::PingPongCallback
    {
    public:
        PingPongCallback(MWRender::PostProcessor* parent)
            : mParent(parent){};
        ~PingPongCallback(){};

        void pingPongBegin(size_t frameId, osg::State& state, const MWRender::PingPongCanvas& canvas) override;
        void pingPongEnd(size_t frameId, osg::State& state, const MWRender::PingPongCanvas& canvas) override;

        size_t mLastFrameId = 0xffffffff;
        MWRender::PostProcessor* mParent;
        std::array<osg::ref_ptr<osg::Viewport>, 2> mDestinationViewport;
    };
}

#endif
