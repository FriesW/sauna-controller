#include "io.h"
#include "util.h"

#include "thermostat.h"
#include "analog.h"

void setup() {
    pinMode(P_RQST, OUTPUT);
    pinMode(SR_RST, OUTPUT);
    pinMode(WD_KICK, OUTPUT);
    pinMode(PCB_TEMP, INPUT);
    pinMode(ROOM_TEMP, INPUT);
    pinMode(POT, INPUT);
    pinMode(BD_LED, OUTPUT);

    Serial.begin(9600);

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


    anlg_init();
}


void loop() {
    if( !cycle() ) return;
    kick();
    anlg_update();

    if( room_temp > 80.0f || pcb_temp > 80.0f )
        halt();

    float desired = 40.0f + pot_pos * (72.5f - 40.0f);
    Serial.print("  Target deg C: ");
    Serial.print(desired);

    therm_update(room_temp, desired);

    Serial.println();
}
