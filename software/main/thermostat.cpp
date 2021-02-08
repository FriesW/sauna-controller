#include <Arduino.h>
#include "thermostat.h"
#include "util.h"

static bool set = false;
static float desired;
static float actual;

static ulong last_transition = 0;
static bool current_state = false;

void update() {

    if(!set) {
        set_relay(false);
        return;
    }
    
    set_relay( current_state );
    
    bool new_state = actual < ( desired + (current_state ? (1.5) : (-1.5)) );
    if( new_state == current_state ) return;
    ulong t = millis();
    if( t - last_transition < 1000UL * 45UL ) return;

    last_transition = t;
    current_state = new_state;
    set_relay( current_state );

}

void target(float d, float a) {
    desired = d;
    actual = a;
    set = true;
}

