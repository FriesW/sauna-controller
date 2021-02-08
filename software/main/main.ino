#include "io.h"
#include "util.h"

#include "thermostat.h"
#include "analog.h"


void setup() {
    analogReference( EXTERNAL );
    pinMode(P_RQST, OUTPUT);
    pinMode(SR_RST, OUTPUT);
    pinMode(WD_KICK, OUTPUT);
    pinMode(PCB_TEMP, INPUT);
    pinMode(ROOM_TEMP, INPUT);
    pinMode(POT, INPUT);
    pinMode(BD_LED, OUTPUT);

    // Startup sequence
    // Set outputs
    digitalWrite(P_RQST, LOW);
    digitalWrite(SR_RST, LOW);
    digitalWrite(WD_KICK, LOW);
    // System settle
    delay(100);
    // Kick
    digitalWrite(WD_KICK, HIGH);
    delay(500);
    digitalWrite(WD_KICK, LOW);
    delay(100);
    // Reset SR
    digitalWrite(SR_RST, HIGH);
    delay(500);
    digitalWrite(SR_RST, LOW);

}

float a_ntc_room = 0.0;
float a_ntc_pcb = 0.0;
float a_pot = 0.0;

void loop() {
    update();
    if( !cycle() ) return;
    kick();

    anlg_read_avg( PCB_TEMP, &a_ntc_pcb );
    anlg_read_avg( ROOM_TEMP, &a_ntc_room );
    anlg_read_avg( POT, &a_pot );

    if( a_ntc_pcb > 45.0 ) {
        halt();
    }

    float t = a_pot * (90.0 - 65.0) + 65.0;

    target(t, a_ntc_room);
}

