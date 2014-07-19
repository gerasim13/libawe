//  Frame.hpp :: Audio frame template
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FRAME_H
#define AWE_FRAME_H

#include "Define.hpp"

#include <algorithm>
#include <array>

namespace awe
{

/*! Preprocessor definition to disable some const operators.
 *  C++11 STL array does not support
 */
#define NO_CONST_FRAME_OPERATORS

/*! Structure holding a single audio frame.
 *
 *  A frame is a snapshot of all sound samples on all channels at a specific
 *  time point. This structure is designed as an expansion of the STL array to
 *  support array/frame-wide algorithms and arithmetic.
 *
 *  \tparam T type of data that this structure will hold.
 *  \tparam Channels number of channels of data within this single frame.
 */
template< typename T, Achan Channels >
struct Aframe {

    using  container_type = std::array< T, Channels >;
    using      value_type = typename container_type::value_type;
    using       reference = typename container_type::      reference;
    using const_reference = typename container_type::const_reference;

    container_type data;

    //! Empty constructor
    Aframe() : data() { }

    //! Pointer-to-data copy constructor
    Aframe(const T* _data, Achan _size = Channels) : data() {
        std::copy(_data, _data + std::min(_size, Channels), data.begin());
    }

    //! Container copy constructor
    Aframe(const container_type&  _data) : data(_data) {}

    //! Container move constructor
    Aframe(const container_type&& _data) : data(_data) {}

    //! Frame sample access operator
          reference operator[](const Achan& pos)       { return data[pos]; }

    //! Frame sample access operator
    const_reference operator[](const Achan& pos) const { return data[pos]; }


    /*! Copies a frame of audio from a buffer.
     *  \param buffer Buffer to copy from.
     *  \param frame  Frame index to copy from.
     */
    static Aframe from_buffer(Abuffer<T> const& buffer, size_t frame) {
        assert(buffer.size() / Channels > frame);
        return Aframe(buffer.data() + (frame * Channels), Channels);
    }

    /*! Overwrites a frame in a buffer with this frame.
     *  \param buffer Buffer to write into.
     *  \param frame  Frame index to write into.
     */
    void to_buffer(Abuffer<T>& buffer, size_t frame) const {
        assert(buffer.size() / Channels > frame);
        T* buff = buffer.data() + (frame * Channels);
        for(Achan c = 0; c < Channels; c += 1) {
            buff[c] = data[c];
        }
    }

//! \name Arithmetic operator overloads
//! \{

    // T as argument
    void operator+= (const T& v) { for(T& u : data) u += v; }
    void operator-= (const T& v) { for(T& u : data) u -= v; }
    void operator*= (const T& v) { for(T& u : data) u *= v; }
    void operator/= (const T& v) { for(T& u : data) u /= v; }

    Aframe operator+ (const T& v) const { Aframe f(data); f += v; return f; }
    Aframe operator- (const T& v) const { Aframe f(data); f -= v; return f; }
    Aframe operator* (const T& v) const { Aframe f(data); f *= v; return f; }
    Aframe operator/ (const T& v) const { Aframe f(data); f /= v; return f; }


    // awe::Aframe<T> as argument
    template< Achan channels > void operator+= (const Aframe<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] += v[c];
    }
    template< Achan channels > void operator-= (const Aframe<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] -= v[c];
    }
    template< Achan channels > void operator*= (const Aframe<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] *= v[c];
    }
    template< Achan channels > void operator/= (const Aframe<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] /= v[c];
    }

    template< Achan channels > Aframe operator+ (const Aframe<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] + v[c];
        return f;
    }
    template< Achan channels > Aframe operator- (const Aframe<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] - v[c];
        return f;
    }
    template< Achan channels > Aframe operator* (const Aframe<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] * v[c];
        return f;
    }
    template< Achan channels > Aframe operator/ (const Aframe<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] / v[c];
        return f;
    }

    // std::array<T> as argument
    template< Achan channels > void operator+= (const std::array<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] += v[c];
    }

    template< Achan channels > void operator-= (const std::array<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] -= v[c];
    }

    template< Achan channels > void operator*= (const std::array<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] *= v[c];
    }

    template< Achan channels > void operator/= (const std::array<T, channels>& v) {
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) data[c] /= v[c];
    }

    template< Achan channels > Aframe operator+ (const std::array<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] + v[c];
        return f;
    }
    template< Achan channels > Aframe operator- (const std::array<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] - v[c];
        return f;
    }
    template< Achan channels > Aframe operator* (const std::array<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] * v[c];
        return f;
    }
    template< Achan channels > Aframe operator/ (const std::array<T, channels>& v) const {
        Aframe f;
        for(Achan c = 0; c < std::min<>(Channels, channels); ++c) f.data[c] = data[c] / v[c];
        return f;
    }
//! \}

//! \name Algorithms

    //! Passes samples in this frame through a function.
    void operator()(std::function<void(T&) F> function) {
        for(T& u : data) function(u);
    }

    //! Passes samples in this frame through a function.
    void operator()(std::function<void(const T&) F> function) const {
        for(T& u : data) function(u);
    }

    //! Converts all sample values in the frame into absolute values.
    void abs() {
        for(T& u : data) u = std::abs(u);
    }

#ifdef MSC_VER
    //! \return Maximum sample value in frame.
    const T& max() const {
        return std::max<>(data.cbegin(), data.cend());
    }

    //! \return Minimum sample value in frame.
    const T& min() const {
        return std::min<>(data.cbegin(), data.cend());
    }
#endif

    //! \return Maximum sample magnitude in frame.
    T absmax() const {
        auto v = std::minmax_element(data.cbegin(), data.cend());
        return std::max<>(std::fabs(*v.first), std::fabs(*v.second));
    }

    //! \return Minimum sample magnitude in frame.
    T absmin() const {
        auto v = std::minmax_element(data.cbegin(), data.cend());
        return std::min<>(std::fabs(*v.first), std::fabs(*v.second));
    }
//! \}
};

//! \name Standard libawe frame types
//! \{
typedef Aframe<Aint  , 2> Asintf;                           //!< a stereo Aint frame
typedef Aframe<Afloat, 2> Asfloatf;                         //!< a stereo Afloat frame
//! \}

//! \name Standard data type converters
//! \{
template< Achan Channels >
Aframe< Afloat, Channels > to_Afloatf(Aframe< Aint, Channels > i)
{
    Aframe< Afloat, Channels > f;
    for(Achan c = 0; c < Channels; c += 1) {
        f[c] = to_Afloat(i[c]);
    }
    return i;
}

template< Achan Channels >
Aframe< Aint, Channels > to_Aintf(Aframe< Afloat, Channels > f)
{
    Aframe< Aint, Channels > i;
    for(Achan c = 0; c < Channels; c += 1) {
        i[c] = to_Aint(f[c]);
    }
    return i;
}

template< Achan SChannels, Achan DChannels >
Aframe< Afloat, DChannels > to_Afloatf(Aframe< Aint, SChannels > i)
{
    Aframe< Afloat, DChannels > f;
    Achan C = std::min<>(SChannels, DChannels);
    for(Achan c = 0; c < C; c += 1) {
        f[c] = to_Afloat(i[c]);
    }
    return i;
}

template< Achan SChannels, Achan DChannels >
Aframe< Aint, DChannels > to_Aintf(Aframe< Afloat, SChannels > f)
{
    Aframe< Aint, DChannels > i;
    Achan C = std::min<>(SChannels, DChannels);
    for(Achan c = 0; c < C; c += 1) {
        i[c] = to_Aint(f[c]);
    }
    return i;
}
//! \}
//! \}

}

#endif
