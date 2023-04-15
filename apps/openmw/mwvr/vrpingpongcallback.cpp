#include "vrpointer.hpp"
#include "vrutil.hpp"

#include <osg/BlendFunc>
#include <osg/Drawable>
#include <osg/Fog>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/MatrixTransform>

#include <components/resource/resourcesystem.hpp>
#include <components/resource/scenemanager.hpp>

#include <components/sceneutil/shadow.hpp>

#include <components/vr/viewer.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "../mwrender/renderingmanager.hpp"
#include "../mwrender/vismask.hpp"
#include "vrpingpongcallback.hpp"

namespace MWVR
{

    void PingPongCallback::pingPongBegin(size_t frameId, osg::State& state, const MWRender::PingPongCanvas& canvas)
    {
        /*
         * Pseudocode:
         * determine view
         * vrviewer -> get target framebuffer for view
         * canvas -> set destination framebuffer
         */

        // if(frameId == mLastFrameId)
        //     view = left

        Stereo::Eye eye = Stereo::Eye::Left;
        if (Stereo::getMultiview())
            eye = Stereo::Eye::Center;
        else if (frameId == mLastFrameId)
            eye = Stereo::Eye::Right;

        auto fbo = VR::Viewer::instance().getFboForView(eye);

        fbo->apply(state, osg::FrameBufferObject::DRAW_FRAMEBUFFER);
        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_DEPTH_BUFFER_BIT);

        canvas.setDestinationFbo(frameId, fbo);

        auto& sm = Stereo::Manager::instance();
        auto resolution = sm.eyeResolution();
        auto& destinationViewport = mDestinationViewport[frameId];
        if (!destinationViewport)
            destinationViewport = new osg::Viewport;
        destinationViewport->setViewport(0, 0, resolution.x(), resolution.y());

        canvas.setDestinationViewport(frameId, destinationViewport);
    }

    void PingPongCallback::pingPongEnd(size_t frameId, osg::State& state, const MWRender::PingPongCanvas& canvas)
    {
        /*
         * Pseudocode:
         * determine view
         * vrviewer -> get target framebuffer for view
         *   [[-> acquire swapchain and use swapchain directly?]]
         * canvas -> get depth texture
         * blit depth texture if app should share depth info
         *
         */

        Stereo::Eye eye = Stereo::Eye::Left;
        if (Stereo::getMultiview())
            eye = Stereo::Eye::Center;
        else if (frameId == mLastFrameId)
            eye = Stereo::Eye::Right;
        mLastFrameId = frameId;

        osg::ref_ptr<osg::FrameBufferObject> fbo = nullptr;
        if (mParent->depthPassEnabled())
            fbo = mParent->getFbo(MWRender::PostProcessor::FBO_OpaqueDepth, frameId);
        else
            fbo = mParent->getFbo(MWRender::PostProcessor::FBO_Primary, frameId);

        VR::Viewer::instance().submitDepthForView(state, fbo, eye);
    }

}
