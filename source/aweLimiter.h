#include "aweBuffer.h"

namespace awe {

    /**
     * A simple brickwall limiter. Nothing fancy.
     */
    struct aLimiterData {
        bool   active;
        Afloat gain;
        Afloat peak;
        aLimiterData () : active (true), gain (1.0), peak(1.0) {}
    };

    int aLimiter     (unsigned long currRenderPos, AfBuffer* thisBuffer, AfBuffer* nextBuffer, void* userData);
    int aLimiterFlip (AfBuffer* thisBuffer, void* userData);

};
