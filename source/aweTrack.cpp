#include <cassert>
#include "aweTrack.h"

namespace awe {

    AfBuffer* ATrack::flipBuffer () {
        // Re-initialize output buffer
        bufferOutput.reset(true);

        // Swap contents.
        bufferOutput.swap(bufferPool);

        // Return pointer to output buffer
        return &bufferOutput;
    }

    void ATrack::drop () {
        bufferOutput.reset(false);
        bufferPool.reset(false);
    }

};
