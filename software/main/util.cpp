#include <Arduino.h>
#include "util.h"
#include "io.h"

void halt() {
    set_relay(false);
    bool s = false;
    while(true){
        delay(100);
        digitalWrite(BD_LED, s);
        s = !s;
    }
}

void set_relay(bool target) {
    digitalWrite(BD_LED, target);
    digitalWrite(P_RQST, target);
}

bool cycle(){
    static ulong last = 0;
    if( last == 0 ) {
        last = millis();
        return true;
    }
    if( millis() - last < 100UL ) {
        return false;
    }
    last += 100UL;
    return true;
}


void kick() {
    static ulong last = 0;
    ulong t = millis();
    // max system runtime
    if( t > ( 1000UL * 60UL * 100UL ) )
        halt();
    // min WD timeout 1 sec
    if( (t - last) < 750UL )
        return;
    last = t;

    digitalWrite( WD_KICK, !digitalRead(WD_KICK) );
}
