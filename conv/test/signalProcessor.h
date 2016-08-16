#ifndef _hel_signalProcessor_h_
#define _hel_signalProcessor_h_

#include "types.h"
#include "complex.h"
#include "responseModel.h"
#include "convolCore_meandr.h"
#include "convolCore_trig.h"
#include "convolCore_trig2.h"
#include "convolCore_triangle.h"

namespace hel
{

    //////////////////////////////////////////////////////////////////////////
    class OnePeriodProcessor
    {
        real _period;
        real _winLen;
        real _step0;

        typedef ConvolCore_trig2 Part;

        ConvolCore_trig        _part_m1;
        ConvolCore_trig        _part_p1;

        //Complex _value;

    public:
        OnePeriodProcessor();
        void setup(real period, real periodsOnWin);
        Complex eval(const TDTimedValue &signal);

    private:



    };

    //////////////////////////////////////////////////////////////////////////
    class SignalProcessor
    {
        size_t _periodsOnWin;
        TVReal _periodLogE;
        TVComplex _valueE;

        typedef std::vector<OnePeriodProcessor> TVProcessors;
        TVProcessors _processors;
    public:
        SignalProcessor();
        void setup(
            real minPeriodE, 
            real maxPeriodE, 
            size_t stepsPeriodE, 
            size_t periodsOnWin);
        void update(const TDTimedValue &signal);

        const TVReal &getPeriodLogE() const;
        const TVComplex &getValueE() const;
    private:

    };
}
#endif
