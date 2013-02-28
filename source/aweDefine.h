//  aweDefine.h :: Essential definitions
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_DEFINE
#define AWE_DEFINE

#include <initializer_list>
#include <algorithm>
#include <array>
#include <queue>
#include <limits>
#include <stdint.h>

namespace awe
{

    typedef int16_t Aint;
    typedef float   Afloat;

    /* FIFO buffer */
    typedef std::queue<Afloat> AfFIFOBuffer;
    typedef std::queue<Aint  > AiFIFOBuffer;
    
#define IO_BUFFER_SIZE  16384   /** File IO buffer size */
#define FI_FACTOR       32768.0 /** Afloat to Aint convertion factor */ 

    /** @return Afloat equivalent of Aint */
    inline Afloat to_Afloat (const Aint &v)
    {
        return static_cast<Afloat>(v) / FI_FACTOR;
    }

    /** @return Aint equivalent of Afloat */
    inline Aint to_Aint (const Afloat &v)
    {
        if (v != v) // Abnormal floating point
            return 0;
        else if (v >  1.0)
            return std::numeric_limits<Aint>::max();
        else if (v < -1.0)
            return std::numeric_limits<Aint>::min();
        else
            return static_cast<Aint>(v * FI_FACTOR);
    }


    /**
     * A frame is a snapshot of all sound samples to and/or from all channels at
     * a specific time point. This structure is an expansion of the STL array to
     * support array/frame-wide algorithms and arithmetic.
     */
    template <typename T, size_t Channels>
        struct Aframe
        {
            typedef typename std::array<T, Channels> data_type;

            data_type data;

            Aframe () : data() {} 
            Aframe (const T*         _data) : data() { std::copy(_data, _data + Channels, data.begin()); }
            Aframe (const data_type& _data) : data(_data) {}

                  T& operator[](size_t pos)       { return data[pos]; }
            const T& operator[](size_t pos) const { return data[pos]; }

            /* ARITHMETIC */
            void operator+= (const T &v) { for (T& u : data) u += v; }
            void operator-= (const T &v) { for (T& u : data) u -= v; }
            void operator*= (const T &v) { for (T& u : data) u *= v; }
            void operator/= (const T &v) { for (T& u : data) u /= v; }

            template <size_t channels>
                void operator+= (const Aframe<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] += v[c];
                }

            template <size_t channels>
                void operator-= (const Aframe<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] -= v[c];
                }

            template <size_t channels>
                void operator*= (const Aframe<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] *= v[c];
                }

            template <size_t channels>
                void operator/= (const Aframe<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] /= v[c];
                }


            template <size_t channels>
                void operator+= (const std::array<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] += v[c];
                }

            template <size_t channels>
                void operator-= (const std::array<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] -= v[c];
                }

            template <size_t channels>
                void operator*= (const std::array<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] *= v[c];
                }

            template <size_t channels>
                void operator/= (const std::array<T, channels> &v) {
                    for (size_t c = 0; c < std::min(Channels,channels); ++c)
                        data[c] /= v[c];
                }

            /* ALGORITHMS */;;
            void abs () {
               for (T& u : data) { u = std::abs(u); }
            }

            const T& max () const {
                return *std::max_element(data.cbegin(), data.cend());
            }

            const T& min () const {
                return *std::min_element(data.cbegin(), data.cend());
            }

            const T& absmax () const {
                auto v = std::minmax_element(data.cbegin(), data.cend());
                return std::max(std::fabs(*v.first), std::fabs(*v.second));
            }

            const T& absmin () const {
                auto v = std::minmax_element(data.cbegin(), data.cend());
                return std::min(std::fabs(*v.first), std::fabs(*v.second));
            }
        };

    // Standard libawe frame types
    typedef Aframe<Aint  , 2> Asintf;       /** a stereo Aint frame   */
    typedef Aframe<Afloat, 2> Asfloatf;     /** a stereo Afloat frame */

    struct ArenderConfig
    {
        // unsigned long systemTime;
        unsigned long targetSampleRate; /** Sound sytem sampling rate */
        unsigned long targetFrameCount; /** Number of frames to render onto targetBuffer. */
        unsigned long targetFrameOffset; /** Index on the targetBuffer to start writing from. */
        enum class renderQuality /** 3-bit quality options */
        {
            DEFAULT  = 0x0,

            REALTIME = 0x1, /** Real-time */
            VERYFAST = 0x2,
            FAST     = 0x3,
            MEDIUM   = 0x4,
            SLOW     = 0x5,
            VERYSLOW = 0x6,
            PLACEBO  = 0x7, /** Let the renderer take as long as it wants to */

            MUTE     = 0xE,
            SKIP     = 0xF
        } quality;
    };

} // namespace awe

#endif // AWE_DEFINE
