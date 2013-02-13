//! aweTools.h -- common awesome toolkit
//! very common commands

#ifndef AWE_TOOLS_H
#define AWE_TOOLS_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "aweBuffer.h"

namespace awe {

    //! Resamples a given buffer.
    inline void resample (AfBuffer* buffer, double factor);

    /**
     * Generate MONO sine floating-point buffer
     * @param waveFreq frequency of the sine wave generated
     * @param buffFreq sampling frequency of the buffer
     * @param length   time length of the generated buffer in seconds
     * @param gain     peak-to-peak volume of the sine wave
     */
    AfBuffer* generateMonoSinefBuffer(const double &waveFreq, const unsigned &buffFreq, const double &length, const double &gain = 0.5f);

    /**
     * Generate MONO sine integral buffer
     * @param waveFreq frequency of the sine wave generated
     * @param buffFreq sampling frequency of the buffer
     * @param length   time length of the generated buffer in seconds
     * @param gain     peak-to-peak volume of the sine wave
     */
    AiBuffer* generateMonoSineiBuffer(const double &waveFreq, const unsigned &buffFreq, const double &length, const double &gain = 0.5f);

};
#endif
