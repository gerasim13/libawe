//  Source.hpp :: Sound source base class
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_SOURCE_H
#define AWE_SOURCE_H

#include "Define.hpp"

namespace awe {

/*! Sound source interface.
 *
 *  This interface is used to provide sound source managers, such as
 *  \ref awe::Source::Track a way to communicate requests to the individual
 *  sound sources.
 */
class Asource
{
public:
    /*! Virtual destructor for the interface.
     *  \note Does not call \ref drop().
     */
    virtual ~Asource() {}

    /*! Make this thing into an active sound source.
     *  \param[in,out]  userData pointer to a user defined structure
     */
    virtual void make_active (void* userData = nullptr) = 0;

    /*! Queries the activity state of the sound source.
     *  \return true if this source source is up and running.
     */
    virtual bool is_active () const = 0;

    /*! Function to render data from the sound source into a buffer.
     *  \param[in,out]  targetBuffer target to render sound into
     *  \param[in]      targetConfig output configuration structure
     */
    virtual void render (AfBuffer &targetBuffer, const ArenderConfig &targetConfig) = 0;

    /*! This function should be called when the sound source pointer is
     *  being released by a manager.
     */
    virtual void drop () = 0;
};

}

#endif
