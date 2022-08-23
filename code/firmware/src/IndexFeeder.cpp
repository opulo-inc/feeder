#include <Arduino.h>
#include "IndexFeeder.h"

#define TENTH_MM_PER_PIP 40
#define TIMEOUT_PER_PIP
#define TICKS_PER_TENTH_MM 11.3

#define TENSION_TIMEOUT         400

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255

// Unit Tests Fail Because This Isn't Defined In ArduinoFake for some reason
#ifndef INPUT_ANALOG
#define INPUT_ANALOG 0x04
#endif

IndexFeeder::IndexFeeder(uint8_t drive1_pin, uint8_t drive2_pin, uint8_t peel1_pin, uint8_t peel2_pin, RotaryEncoder* encoder) :
    _drive1_pin(drive1_pin),
    _drive2_pin(drive2_pin),
    _peel1_pin(peel1_pin),
    _peel2_pin(peel2_pin),
    _encoder(encoder) {
    init();
}

bool IndexFeeder::init() {

    pinMode(_drive1_pin, OUTPUT);
    pinMode(_drive2_pin, OUTPUT);
    pinMode(_peel1_pin, OUTPUT);
    pinMode(_peel2_pin, OUTPUT);

    return true;
}

Feeder::FeedResult IndexFeeder::feedDistance(uint8_t tenths_mm, bool forward) {

    int test = abs(tenths_mm) % TENTH_MM_PER_PIP;

    // if (abs(tenths_mm) % TENTH_MM_PER_PIP != 0) {
    //     // The Index Feeder has only been tested and calibrated for moves of 4mm (One Pip) so far.
    //     // If any other value is supplied, indicate it is invalid.
    //     return Feeder::FeedResult::INVALID_LENGTH;
    // }

    bool success = (forward) ? moveForward(tenths_mm) : moveBackward(tenths_mm);
    if (!success) {
        return FeedResult::MOTOR_FAULT;
    }
    

    return Feeder::FeedResult::SUCCESS;
}

//returns true if we reached position within timeout, false if we didn't
bool IndexFeeder::moveInternal(uint32_t timeout, bool forward, uint8_t tenths_mm) {
    signed long start_pos, goal_pos, ticks_to_move;

    //move based on encoder value
    //all this does is move the tape a number of ticks, and that's it!
    ticks_to_move = tenths_mm * TICKS_PER_TENTH_MM;
    start_pos = _encoder->getPosition();

    if (forward == true) {
        ticks_to_move = ticks_to_move * -1;
    }

    goal_pos = start_pos + ticks_to_move;

    unsigned long start_time = millis();

    bool ret = false;

    float Kp=200, Ki=5, Kd=0, Hz=60;
    int output_bits = 8;
    bool output_signed = true;
    FastPID pid(Kp, Ki, Kd, Hz, output_bits, output_signed);
    pid.setOutputRange(-255, 255);

    while(millis() < start_time + timeout){

        signed long current_pos = _encoder->getPosition();        

// PID implementation

        signed long output = pid.step(goal_pos, current_pos);

        // Stop early if we've hit steady state
        if(fabs(goal_pos - current_pos) < 3){ // if we're at setpoint, set return value to true and break
            ret = true;
            break;
        }
        else { //if not at setpoint yet
            if(output > 0){
                analogWrite(_drive1_pin, 0);
                analogWrite(_drive2_pin, output*2);
            }
            else {
                output = abs(output);
                analogWrite(_drive1_pin, output*2);
                analogWrite(_drive2_pin, 0);
            }
        }


    }

    // be sure to turn off motors
    analogWrite(_drive1_pin, 0);
    analogWrite(_drive2_pin, 0);

    return ret;
}

bool IndexFeeder::tension(uint32_t timeout) {
    unsigned long start_millis, current_millis;

    //tension film
    digitalWrite(PA8, LOW);
    analogWrite(_peel1_pin, 255);
    analogWrite(_peel2_pin, 0);
    delay(500);
    analogWrite(_peel1_pin, 0);
    analogWrite(_peel2_pin, 0);
    digitalWrite(PA8, HIGH);


    return true;
}

bool IndexFeeder::moveForward(uint8_t tenths_mm) {
    // First, ensure everything is stopped
    stop();

    //peel film

    tension(TENSION_TIMEOUT);
    

    analogWrite(_peel1_pin, 255);
    analogWrite(_peel2_pin, 0);
    
    //move tape
    moveInternal(5000, true, tenths_mm);

    analogWrite(_peel1_pin, 0);
    analogWrite(_peel2_pin, 0);

    // move film
    //return tension(TENSION_TIMEOUT);
    return true;
}

bool IndexFeeder::moveBackward(uint8_t tenths_mm) {
    // First, ensure everything is stopped
    stop();

    // Next, unspool some film to give the tape slack. imprecise amount because we retention later

    // move tape backward
    moveInternal(5000, false, tenths_mm);

    stop();

    //tension film again
    return tension(TENSION_TIMEOUT);
}

void IndexFeeder::stop() {
    // Stop Everything
    analogWrite(_drive1_pin, 0);
    analogWrite(_drive2_pin, 0);
    analogWrite(_peel1_pin, 0);
    analogWrite(_peel2_pin, 0);
}