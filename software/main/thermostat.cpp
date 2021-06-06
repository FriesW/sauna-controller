#include <Arduino.h>
#include "thermostat.h"
#include "util.h"

static ulong last_transition = 0;
static bool current_state = false;

void therm_update(float actual, float desired) {

    set_relay( current_state );

    // Wait 10 seconds before turning on
    if( millis() < 1000UL * 10UL ) return;

    // Calculate histeresis
    desired += desired + ( current_state ? (1.5) : (-1.5) );
    // Determine new state and if new state is different
    bool new_state = actual < desired;
    if( new_state == current_state ) return;
    // If turning on, then we need to make sure we aren't short cycling
    ulong t = millis();
    if( new_state && t - last_transition < 1000UL * 45UL ) return;

    last_transition = t;
    current_state = new_state;
    set_relay( current_state );

}
