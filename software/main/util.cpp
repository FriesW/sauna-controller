#include <Arduino.h>
#include "util.h"
#include "io.h"

void halt() {
    set_relay(false);
    while(true);
}

void set_relay(bool target) {
    digitalWrite(BD_LED, target);
    digitalWrite(P_RQST, target);
}

bool cycle(){
    static unsigned long last = 0;
    if( last = 0 ) {
        last = millis();
        return true;
    }
    if( millis() - last < 10UL ) {
        return false;
    }
    last += 10UL;
    return true;
}


void kick() {
    static unsigned long last = 0;
    unsigned long t = millis();
    // max system runtime
    if( t > ( 1000UL * 60UL * 100UL ) )
        halt();
    // min WD timeout 1 sec
    if( (t - last) < 800UL )
        return;
    last = t;

    digitalWrite( WD_KICK, !digitalRead(WD_KICK) );
}

