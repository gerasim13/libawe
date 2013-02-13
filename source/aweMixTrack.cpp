#include "aweMixTrack.h"
#include "aweTools.h"
#include <stdexcept>

namespace awe {

    bool AMixTrack::attachSource (Asource* src)
    {
        if (sourceSet.count(src) > 0) {
            return false;
        } else {
            sourceSet.insert(src);
            return true;
        }
    }

    bool AMixTrack::detachSource (Asource* src)
    {
        return sourceSet.erase(src);
    }

    void AMixTrack::setAllSourcesTo (awe::AState state)
    {
        for (Asource* snd : sourceSet)
            snd->setState(state);
    }

    size_t AMixTrack::countPlaying () const
    {
        size_t ret = 0;

        for (Asource* snd : sourceSet)
            if (snd->getState() == RUNNING)
                ret++;

        return ret;
    }


    void AMixTrack::pull ()
    {
        for (Asource* snd : sourceSet)
            if (snd->getState() == RUNNING)
                snd->render(pullPos, &bufferPool, &renderConfig);

        pullPos += renderConfig.targetFrameCount;
    }

    void AMixTrack::skip ()
    {
        ARenderQ temp = renderConfig.renderQuality;
        renderConfig.renderQuality = MUTE;

        for (Asource* snd : sourceSet)
            if (snd->getState() == RUNNING)
                snd->render(pullPos, &bufferPool, &renderConfig);

        pullPos += renderConfig.targetFrameCount;
        renderConfig.renderQuality = temp;
    }


    void AMixTrack::flip ()
    {
        // Re-initialize output buffer
        bufferOutput.reset(true);

        // Swap contents.
        bufferOutput.swap(bufferPool);

        pullPos = 0;

        for (size_t renderPos=0; renderPos < bufferOutput.getFrameCount(); renderPos++)
        {
            // Send this buffer through an external processing function.
            if (sendThrough != nullptr)
                sendThrough(renderPos, &bufferOutput, userData);

            // Apply track gain and panning.
            Afloat2 frame (
                    bufferOutput.at(renderPos*2  ),
                    bufferOutput.at(renderPos*2+1)
                    );
            mixer(frame);
        }

        if (flipThrough != nullptr)
            flipThrough(&bufferOutput, userData);

    }

    AfBuffer* AMixTrack::dine ()
    {
        switch (trackState)
        {
            case INITIAL:
                flip();
                trackState = RUNNING;
                return &bufferOutput;
            case RUNNING:
                return nullptr;
            case PAUSED :
                trackState = RUNNING;
                return &bufferOutput;
            case STOPPED:
                trackState = INITIAL;
                return &bufferOutput;
            default:
                throw std::invalid_argument("libawe [exception @ AMixTrack::dine] invalid mixer track state");
                return nullptr;
        }
    }

    bool AMixTrack::work ()
    {
        switch (trackState)
        {
            case INITIAL:
                flip();
                trackState = PAUSED;
                return false;
            case RUNNING: 
                if (pullPos < bufferPool.getFrameCount()) { pull(); }
                else { trackState = INITIAL; }
                return true;
            case PAUSED :
                if (pullPos < bufferPool.getFrameCount()) { pull(); }
                else { trackState = STOPPED; }
                return true;
            case STOPPED:
                return false;
            default:
                throw std::invalid_argument("libawe [exception @ AMixTrack::work] invalid mixer track state");
                return false;
        }
    }

};
