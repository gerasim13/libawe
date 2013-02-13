#include <vector>
#include "aweBuffer.h"

namespace awe {

    // Buffer class is a template, see header for declaration.
    // Here will be all the small buffer related utility functions.

    size_t mixBuffer_itof (const AiBuffer* src, AfBuffer* dst, size_t size, size_t src_offset, size_t dst_offset)
    {
        if ((src_offset > src->getSampleCount())
                || (dst_offset > dst->getSampleCount())
           ) { return 0; }

        if (dst_offset + size > src->getSampleCount())
            size = dst->getSampleCount() - dst_offset;
        if (src_offset + size > src->getSampleCount())
            size = src->getSampleCount() - src_offset;

        auto dst_data = dst->getVector();

        for (size_t i=0; i<size; i++)
            dst_data->at(dst_offset+i) += to_Afloat(src->getSample(src_offset+i));

        return size;
    }

    size_t mixBuffer_ftoi (const AfBuffer* src, AiBuffer* dst, size_t size, size_t src_offset, size_t dst_offset)
    {
        if ((src_offset > src->getSampleCount())
                || (dst_offset > dst->getSampleCount())
           ) { return 0; }

        if (dst_offset + size > src->getSampleCount())
            size = dst->getSampleCount() - dst_offset;
        if (src_offset + size > src->getSampleCount())
            size = src->getSampleCount() - src_offset;

        auto dst_data = dst->getVector();

        for (size_t i=0; i<size; i++)
            dst_data->at(dst_offset+i) += to_Aint(src->getSample(src_offset+i));

        return size;
    }


};
