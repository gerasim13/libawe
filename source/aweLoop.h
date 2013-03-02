//  aweLoop.h :: Loopable object definition
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_LOOP_H
#define AWE_LOOP_H

#include <cstdint>

namespace awe {

    /**
     * Aloop namespace/object.
     */
    class Aloop
    {
        public:
            /** Loop mode enumerator and operators */
            enum class Mode : std::uint8_t {
				// First two bits is the loop method
				UNDEFINED   = 0x0,

				ONCE        = 0x1,
				REPEAT      = 0x2,
				ALTERNATING = 0x3,

				// Third bit is reserved
				// Forth bit is move direction
				FORWARD     = 0x0,
				REVERSE     = 0x8,

                // Default
                __DEFAULT   = FORWARD | ONCE,

				// Masks
				__LOOP_MODE = 0x3,
				__DIRECTION = 0x8,
			};

            Mode   mode;
            bool   paused;
            double begin, now, end;

            Aloop (const double &_begin, const double &_end, const Aloop::Mode &_mode, const bool startPaused = false);
            Aloop (const double &_begin, const double &_end, const double &_now, const Aloop::Mode &_mode, const bool startPaused = false);

            /* Move current position by b */
                   bool operator+= (const double &b);
            inline bool operator-= (const double &b) { return operator+=(-b); }

            /* cast to unsigned integer */
            inline unsigned long long  ubegin () const { return static_cast<unsigned long long>(begin); }
            inline unsigned long long  unow   () const { return static_cast<unsigned long long>(now); }
            inline unsigned long long  uend   () const { return static_cast<unsigned long long>(end); }

            /* cast to unsigned integer with loop boundary checking */
            inline unsigned long long sunow   () const { return unow() < uend() ? unow() : uend() - 1; }
            inline unsigned long long suend   () const { return uend() - 1; }
    };
   
    inline static std::uint8_t operator* (const Aloop::Mode&  mode) { return static_cast<std::uint8_t>(mode); }
    inline static std::uint8_t operator* (const Aloop::Mode&& mode) { return static_cast<std::uint8_t>(mode); }

	inline static bool        isReverse (const Aloop::Mode &mode) { return (*mode & *Aloop::Mode::__DIRECTION) == *Aloop::Mode::__DIRECTION; }
	inline static bool        isForward (const Aloop::Mode &mode) { return !isReverse(mode); }
	inline static Aloop::Mode getMethod (const Aloop::Mode &mode) { return static_cast<Aloop::Mode>(*mode & *Aloop::Mode::__LOOP_MODE); } /** b'00000111' */

    /* Aloop::Mode global operator overloading. */
    inline static Aloop::Mode operator& (const Aloop::Mode &a, const Aloop::Mode &b) { return static_cast<Aloop::Mode>(*a & *b); } /** Bit-wise AND operator */
    inline static Aloop::Mode operator| (const Aloop::Mode &a, const Aloop::Mode &b) { return static_cast<Aloop::Mode>(*a | *b); } /** Bit-wise  OR operator */
    inline static Aloop::Mode operator^ (const Aloop::Mode &a, const Aloop::Mode &b) { return static_cast<Aloop::Mode>(*a ^ *b); } /** Bit-wise XOR operator */
    inline static Aloop::Mode operator~ (const Aloop::Mode &mode) { return static_cast<Aloop::Mode>(*mode ^  *Aloop::Mode::__DIRECTION); } /** Inverts the loop direction */
    inline static Aloop::Mode operator+ (const Aloop::Mode &mode) { return static_cast<Aloop::Mode>(*mode & ~*Aloop::Mode::__DIRECTION); /* b'11110111' */ } /** Forces the direction to forward */
    inline static Aloop::Mode operator- (const Aloop::Mode &mode) { return static_cast<Aloop::Mode>(*mode |  *Aloop::Mode::__DIRECTION); /* b'00001000' */ } /** Forces the direction to reverse */

}
#endif
