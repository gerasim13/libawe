#include <stdexcept>
#include <sstream>
#include <bitset>

#include "aweLoop.h"

namespace awe {
    Aloop::Aloop (const double &_begin, const double &_end, const Aloop::Mode &_mode, const bool startPaused) :
        mode    (_mode),
        paused  (startPaused),
        begin   (_begin),
        now     (isReverse(_mode) ? _end : _begin),
        end     (_end)
    {
    }

    Aloop::Aloop (const double &_begin, const double &_now, const double &_end, const Aloop::Mode &_mode, const bool startPaused) :
        mode    (_mode),
        paused  (startPaused),
        begin   (_begin),
        now     (_now),
        end     (_end)
    {
    }

    // Move current position by b
    bool Aloop::operator+= (const double &b)
    {
        if (paused) return true;

        switch (getMethod(mode))
        {
            case UNDEFINED:
                // Warn undefined loop mode
            case ONCE:
                if (isForward(mode)) {
                    now += b;
                    if (now > end) {
                        now = end;
                        paused = true;
                    }
                } else {
                    now -= b;
                    if (now < begin) {
                        now = begin;
                        paused = true;
                    }
                }

                break;

            case REPEAT:
                if (isForward(mode))
                    now += b;
                else
                    now -= b;

                if (now > end)
                    now = begin;
                else if (now < begin)
                    now = end;

                break;

            case ALTERNATING:
                if (isForward(mode))
                    now += b;
                else
                    now -= b;

                if (now < begin || now > end)
                    mode = ~mode;

                break;

            default:
                std::ostringstream error_string;
                error_string
                    << "Invalid loop mode {in binary " << std::bitset<8>(mode)
                    << "} encountered on loop object " << std::hex << this
                    << "." << std::endl;
                throw std::invalid_argument(error_string.str().c_str());

                return true;
        } // switch(getMethod(mode))

        return false;
    }

} // namespace awe
