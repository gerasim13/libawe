#include <cmath>
#include "aweLimiter.h"

namespace awe {

    int aLimiter (unsigned long currRenderPos, AfBuffer* thisBuffer, AfBuffer* nextBuffer, void* userData)
    {
        aLimiterData* data = (aLimiterData*)userData;

        if (data->active == false) return 0;

        /* get higher out of the stereo frame */
        Afloat2 frame (fabs(thisBuffer->at(currRenderPos*2)), fabs(thisBuffer->at(currRenderPos*2+1)));
        Afloat  frmax = frame.max();

        if (frmax > data->gain)
            data->gain += (frmax - data->gain) / 2.0;

        // if (!std::isfinite(data->gain))
        // 	 data->gain = 1.0;

        thisBuffer->at(currRenderPos*2 )    /= Afloat(data->gain);
        thisBuffer->at(currRenderPos*2 + 1) /= Afloat(data->gain);

        if (data->gain > 1.0)
            data->gain -= (data->gain - 1.0) / 2.0; /* exponential fade. */

        if (data->gain > data->peak)
            data->peak = data->gain;

        return 0; /* success! */
    }

    /** @TODO Implement a limiter that works on the whole buffer.
     * Maybe add apply some look-ahead compression on it.
     */
    int aLimiterFlip (AfBuffer* thisBuffer, void* userData)
    {
        aLimiterData* data = (aLimiterData*)userData;

        if (data->active == false) return 0;

        return 0;
    }

};
