#ifndef AWE_H
#define AWE_H

#define AWE
#define AWE_VERSION 0.5.2
#define AWE_NAME   "0.5.2-CUSTOM"

/* aweCompileConfig */

/**
 * Fix underruns by sending an empty buffer to PortAudio.
 */
//  #define AWE_CATCH_OUTPUT_UNDERRUNS

/**
 * Fix floating point overclips on output
 */
//  #define AWE_CATCH_OUTPUT_OVERCLIPS


/** Use equal power panning (resultant power == input power)
 *  instead of linear panning (faster, but has weird loudness)
 */
#define AWE_EQUALPOWER_PAN


/** Prevent libawe from sleeping to free up some CPU.
 *  Enable this unless you're using libawe on it's own.
 */
#define AWE_INSOMNIA

/**
 * Turn off all assertions, including debugging ones.
 */
#define AWE_NOASSERT

/**
 * Turn on debugging.
 */
#define AWE_DEBUG

/**
 * Clear Abuffer memory when they are destroyed.
 */
#define AWE_NUKE_BUFFERS


/** Use C++11 <algorithm> library in modules */

// Causes popping sounds. Not recommended until GCC fixes it.
// #define AWE_ENGINE_USE_CXX11_ALGORITHM

#endif
