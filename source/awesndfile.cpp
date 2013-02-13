#include <exception>
#include "awesndfile.h"

namespace awe {

    /* Read from memory -- libsndfile virtual IO functions */

    sf_count_t awe_sf_vmio_get_filelen (void *user_data)
    {
        awe_sf_vmio_data* io = (awe_sf_vmio_data*)user_data;
        return io->size;
    }

    sf_count_t awe_sf_vmio_tell (void *user_data)
    {
        awe_sf_vmio_data* io = (awe_sf_vmio_data*)user_data;
        return io->curr;
    }

    sf_count_t awe_sf_vmio_seek (sf_count_t offset, int whence, void *user_data)
    {
        awe_sf_vmio_data* io = (awe_sf_vmio_data*)user_data;

        switch (whence)
        {
            case SEEK_SET: io->curr =        0 + offset; break;
            case SEEK_CUR: io->curr = io->curr + offset; break;
            case SEEK_END: io->curr = io->size; break;
            default: throw std::exception(); break;
        }

        return io->curr;
    }

    sf_count_t awe_sf_vmio_read (void *ptr, sf_count_t count, void *user_data)
    {
        awe_sf_vmio_data* io = (awe_sf_vmio_data*)user_data;
        sf_count_t realcount = 0;
        char* sptr = (char*)ptr;

        for (sf_count_t i=0; i<count; i++) {
            if (io->curr < io->size) {
                sptr[i] = io->mptr[io->curr];
                io->curr++;
                realcount++;
            }
        }

        return realcount;
    }

    sf_count_t awe_sf_vmio_write (const void *ptr, sf_count_t count, void *user_data)
    {
        awe_sf_vmio_data* io = (awe_sf_vmio_data*)user_data;
        sf_count_t realcount = 0;
        char* sptr = (char*)ptr;

        for (sf_count_t i=0; i<count; i++) {
            if (io->curr < io->size) {
                io->mptr[io->curr] = sptr[i];
                io->curr++;
                realcount++;
            }
        }

        return realcount;
    }

};
