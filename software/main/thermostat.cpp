#include "thermostat.h"

static bool set = false;
static float desired;
static float actual;

void update() {

    if(!set) {
        // set_relay(false);
        return;
    }

    // set_relay( actual < desired );

}

void target(float d, float a) {
    desired = d;
    actual = a;
    set = true;
}
