#ifndef AWE_DEFINE
#define AWE_DEFINE

#include <algorithm>
#include <stddef.h>
#include <stdint.h>
#include <limits>
#include <cstdio>

#include "aweCompileConfig.h"

namespace awe {

    typedef int16_t Aint;
    typedef float 	Afloat;

#define AWE_FILE_READ_SIZE  16384   /** File IO buffer size */
#define AWE_FTOI_FACTOR     32768.0 /** Floating point to integer convertion factor */

    /** @return Afloat equivalent of Aint data */
    inline Afloat to_Afloat (const Aint &r)
    {
        return static_cast<Afloat>(r) / AWE_FTOI_FACTOR;
    }

    /** @return Aint equivalent of Afloat data */
    inline Aint to_Aint (const Afloat &r)
    {
        if (r != r) // Abnormal float
            return 0;
        else if (r > 1.0) 
            return std::numeric_limits<Aint>::max();
        else if (r < -1.0)
            return std::numeric_limits<Aint>::min();
        else
            return static_cast<Aint>(r * AWE_FTOI_FACTOR);
    }

    /** Number pair structure used to store stereo buffers. */
    template <typename T>
        struct A2 {
            T l; /** Left sample */
            T r; /** Right sample */

            A2 (const T &L, const T &R) : l(L), r(R) {}

            void operator*= (const T &v) { l *= v; r *= v; }
            void operator/= (const T &v) { l /= v; r /= v; }
            void operator+= (const T &v) { l += v; r += v; }
            void operator-= (const T &v) { l -= v; r -= v; }

            void operator*= (const A2<T> &v) { l *= v.l; r *= v.r; }
            void operator/= (const A2<T> &v) { l /= v.l; r /= v.r; }
            void operator+= (const A2<T> &v) { l += v.l; r += v.r; }
            void operator-= (const A2<T> &v) { l -= v.l; r -= v.r; }

            /**
             * @return the higher magnitude of the two values.
             */
            const T& max () const { return std::max(std::fabs(l), std::fabs(r)); }
            /**
             * @return the lower magnitude of the two values.
             */
            const T& min () const { return std::min(std::fabs(l), std::fabs(r)); }
        };

    typedef A2<Afloat> Afloat2;
    typedef A2<Aint>   Aint2;

    /**
     * Specifies the looping method applied.
     * 
     * @TODO Redefine this to differentiate between direction(forwards or backwards)
     *       and looping sequence type (one-shot, repeat, alternating/ping-pong).
     */
    enum ALoopType
    {
        ONESHOT     = 0x21,
        FORWARD     = 0x22,
        BACKWARD    = 0x23,
        PINGPONG    = 0x24, /** alternating loop, but currently moving forwards   */
        PONGPING	= 0x25  /** alternating loop, but currently moving backwards  */
    };

    /**
     * Specifies the state of an object.
     * Applications may vary depending on object.
     */
    enum AState
    {
        CRASHED     = 0x4F, /** for broken stuff */
        INITIAL     = 0x41,
        RUNNING     = 0x42,
        PAUSED      = 0x43,
        STOPPED     = 0x44,
        ALLOKAY     = 0x46, /** only for stuff that don't loop */
        DROPPED     = 0x48  /** object marked for dropping     */
    };

    /**
     * Render quality settings.
     */
    enum ARenderQ
    {
        DEFAULT 	= 0x00, /** Default settings based on what the renderer is designed for.*/

        RT_DRAFT    = 0x01, /** Default settings for fastest real-time render.        */
        RT_NORMAL   = 0x02, /** Default settings for most efficient real-time render. */
        RT_FINAL    = 0x03, /** Defualt settings for best real-time render.           */

        RENDER_DRAFT    = 0x08, /** Default settings for most efficient output render.*/
        RENDER_NORMAL   = 0x09, /** Default settings for nice output render.          */
        RENDER_FINAL    = 0x0A, /** Default settings for perfect output render.       */

        MUTE = 0x0E, /** Do not render. Commonly used during underruns to keep sound in sync.    */
        SKIP = 0x0F  /** Do not render or modify anything that affects the result in the future. */
    };

    /** Render property structure. */
    typedef struct ArenderConfig
    {
        unsigned long systemTime;       /** Current system time. */
        unsigned long targetSampleRate; /** Render output sample rate. */
        unsigned long targetFrameCount; /** Number of frames to render. */
        ARenderQ renderQuality;         /** Quality of render requested by renderer. */

        void* userData;
        void* rendererCallback(void*);  /** This was added for something but I forgot what it was already... */

        ArenderConfig (unsigned long sample_rate, unsigned long frame_count, ARenderQ q = DEFAULT)
            : targetSampleRate(sample_rate),
            targetFrameCount(frame_count),
            renderQuality(q)
        {}

    } ArenderConfig;

};
#endif
