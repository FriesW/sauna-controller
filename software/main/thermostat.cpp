#include <Arduino.h>
#include "thermostat.h"
#include "util.h"

#define FIRST_ON_DELAY (1000UL * 10UL)

static ulong last_transition = 0;
static bool current_state = false;
static bool turned_on_once = false;

void therm_update(float actual, float desired) {
    ulong t = millis();

    set_relay( current_state );

    // Wait a fixed period before first turned on
    if( t < FIRST_ON_DELAY ) return;

    // Calculate histeresis
    desired += ( current_state ? (1.5f) : (-1.5f) );
    // Determine new state and if new state is different
    bool new_state = actual < desired;
    if( new_state == current_state ) return;
    // If turning on, then we need to make sure we aren't short cycling
    // Skip short cycling check if this is the first turn-on
    if( turned_on_once && new_state && t - last_transition < 1000UL * 45UL ) return;

    last_transition = t;
    current_state = new_state;
    if( new_state )
        turned_on_once = true;
    set_relay( current_state );

}
