//  aweFilter.h :: Filter class
//  Copyright 2013 Keigen Shu

#ifndef AWE_FILTER_H
#define AWE_FILTER_H

#include "aweDefine.h"

namespace awe {

// Namespace for filters
namespace Filter {}

// Stereo channel filter base class
class AscFilter
{
public:
    virtual ~AscFilter() {}

    virtual void doM(Afloat &m) = 0;
    virtual void doL(Afloat &l) = 0;
    virtual void doR(Afloat &r) = 0;

    virtual void doM(Aint   &m) = 0;
    virtual void doL(Aint   &l) = 0;
    virtual void doR(Aint   &r) = 0;

    virtual void doF(Asfloatf &f) = 0;
    virtual void doF(Asintf   &f) = 0;

    virtual Afloat   ffdoM(Afloat const &m) = 0;
    virtual Afloat   ffdoL(Afloat const &l) = 0;
    virtual Afloat   ffdoR(Afloat const &r) = 0;

    virtual Afloat   ifdoM(Aint   const &m) = 0;
    virtual Afloat   ifdoL(Aint   const &l) = 0;
    virtual Afloat   ifdoR(Aint   const &r) = 0;

    virtual Aint     iidoM(Aint   const &m) = 0;
    virtual Aint     iidoL(Aint   const &l) = 0;
    virtual Aint     iidoR(Aint   const &r) = 0;

    virtual Aint     fidoM(Afloat const &m) = 0;
    virtual Aint     fidoL(Afloat const &l) = 0;
    virtual Aint     fidoR(Afloat const &r) = 0;

    virtual Asfloatf ffdoF(Asfloatf const &f) = 0;
    virtual Asintf   iidoF(Asintf   const &f) = 0;
};
}
#endif
