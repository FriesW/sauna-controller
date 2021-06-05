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

    //float v = 0.0;
    //while(true){
        //analogRead(PCB_TEMP);
    //}

    pinMode(AREF_1K0, OUTPUT);
    pinMode(AREF_1K1, OUTPUT);
    pinMode(AREF_330, OUTPUT);
    digitalWrite(AREF_1K0, HIGH);
    digitalWrite(AREF_1K1, HIGH);
    digitalWrite(AREF_330, LOW);
}

float a_ntc_room = 0.0;
float a_ntc_pcb = 0.0;
float a_pot = 0.0;

void loop() {
    //update();
    if( !cycle() ) return;
    kick();

    set_relay( ( millis() / 1000 ) % 2 == 0 );

    anlg_read_avg( PCB_TEMP, &a_ntc_pcb );
    //anlg_read_avg( ROOM_TEMP, &a_ntc_room );
    //anlg_read_avg( POT, &a_pot );
    //Serial.println( volt_ntc_convert(a_ntc_pcb) );
    Serial.println( analogRead(PCB_TEMP) );
    return;

    if( a_ntc_pcb > 45.0 ) {
        halt();
    }

    float t = a_pot * (90.0 - 65.0) + 65.0;

    target(t, a_ntc_room);
}
