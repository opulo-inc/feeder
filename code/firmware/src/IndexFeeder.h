#ifndef _INDEX_FEEDER_H
#define _INDEX_FEEDER_H

#include "Feeder.h"
#include <functional>

#ifndef MOTOR_DEPS
    #define MOTOR_DEPS
    #include <RotaryEncoder.h>
    #include <FastPID.h>

#endif 



class IndexFeeder : public Feeder {

    public:
        IndexFeeder(uint8_t drive1_pin, uint8_t drive2_pin, uint8_t peel1_pin, uint8_t peel2_pin, RotaryEncoder* encoder);
        bool init() override;
        Feeder::FeedResult feedDistance(uint8_t tenths_mm, bool forward) override;
        bool moveInternal(uint32_t timeout, bool forward, uint8_t tenths_mm);
        
    private:
        uint8_t _drive1_pin;
        uint8_t _drive2_pin;

        uint8_t _peel1_pin;
        uint8_t _peel2_pin;  

        RotaryEncoder* _encoder;

        double* _Setpoint;
        double* _Input;
        double* _Output;

        bool moveForward(uint8_t tenths_mm);
        bool moveBackward(uint8_t tenths_mm);
        void stop();
        //bool moveInternal(uint32_t timeout, bool forward, uint8_t tenths_mm);
        bool tension(uint32_t timeout);
        bool loopback();

};

#endif //_INDEX_FEEDER_H