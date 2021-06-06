#include "io.h"
#include "util.h"

#include "thermostat.h"
#include "analog.h"

#define TEMP_MIN (60.0f)
#define TEMP_MAX (87.5f)

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

    if( room_temp < 0.0f || pcb_temp < 0.0f || pot_pos < 0.0f ||
        room_temp > 95.0f || pcb_temp > 85.0f )
        halt();

    float desired = TEMP_MIN + pot_pos * (TEMP_MAX - TEMP_MIN);
    Serial.print("  Target deg C: ");
    Serial.print(desired);

    therm_update(room_temp, desired);

    Serial.println();
}
