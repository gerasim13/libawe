//  awesndfile.hpp :: Audio file reader via libsndfile
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_SNDFILE_H
#define AWE_SNDFILE_H

#include <sndfile.h>

namespace awe
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//!@name Implementation code for sndfile virtual I/O methods
//!@{

struct awe_sf_vmio_data {
    sf_count_t  curr;                                       //!< current offset
    sf_count_t  size;                                       //!< file size
    char*       mptr;                                       //!< Pointer to beginning of data
};

sf_count_t awe_sf_vmio_get_filelen(void* user_data);
sf_count_t awe_sf_vmio_tell(void* user_data);
sf_count_t awe_sf_vmio_seek(sf_count_t offset, int whence, void* user_data);
sf_count_t awe_sf_vmio_read(void* ptr, sf_count_t count, void* user_data);
sf_count_t awe_sf_vmio_write(const void* ptr, sf_count_t count, void* user_data);

SF_VIRTUAL_IO awe_sf_vmio = {
    awe_sf_vmio_get_filelen,
    awe_sf_vmio_seek,
    awe_sf_vmio_read,
    awe_sf_vmio_write,
    awe_sf_vmio_tell
};

//!@}
#endif

class Asample;

/** Reads audio data from a `libsndfile` instance into a \ref awe::Asample.
 *  Upon exit, the `SNDFILE*` object is closed via `sf_close()` and memory
 *  occupied by the `SF_INFO` object is freed back to the system via `delete`.
 *
 *  \param sample   Pointer to \ref awe::Asample object to store data into.
 *  \param sndf     Pointer to `SNDFILE` object returned by `sf_open()` or
 *                  `sf_open_virtual()`.
 *  \param info     Pointer to `SF_INFO` object passed to `sf_open()` or
 *                  `sf_open_virtual()`.
 */
void read_sndfile(Asample* sample, SNDFILE* sndf, SF_INFO* info);

}

#endif
