#define P_RQST (4)
#define SR_RST (3)
#define WD_KICK (2)

#define BD_LED (13)

#define INT_TEMP (16)
#define EXT_TEMP (15)
#define POT (14)

typedef unsigned int uint;

#include "ntc.h"

void setup() {
    analogReference( EXTERNAL );
    pinMode(P_RQST, OUTPUT);
    pinMode(SR_RST, OUTPUT);
    pinMode(WD_KICK, OUTPUT);
    pinMode(INT_TEMP, INPUT);
    pinMode(EXT_TEMP, INPUT);
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

void loop() {
    kick();

    bool st = (millis() / 200 ) % 2;
    digitalWrite(BD_LED, st);
    digitalWrite(P_RQST, st);
}


static void kick() {
    static unsigned long last = 0;
    unsigned long t = millis();
    // max system runtime
    //if( t > ( 1000UL * 60UL * 1UL ) )
    //    while(true);
    // min WD timeout 1 sec
    if( (t - last) < 800UL )
        return;
    last = t;

    digitalWrite( WD_KICK, !digitalRead(WD_KICK) );
}

static void anlg_read_avg(uint pin, float * val) {
    #define AREF_V (2.5)
    if( *val <= 0.0 || *val >= AREF_V )
        *val = 0.0;
    float v = (float)analogRead(pin);
    v = v / ((float)(1<<10) - 1.0) * AREF_V;
    *val = v * 0.1 + *val * 0.9;
    #undef AREF_V
}

static float volt_ntc_convert(float v) {
    if( v > ntc_lut[0] )
        return ntc_start_temp;
    if( v < ntc_lut[ntc_lut_size-1] )
        return ntc_start_temp + (ntc_lut_size-1) * ntc_temp_step;
    for(uint i = 0; i < ntc_lut_size - 1; i++) {
        if( ntc_lut[i] >= v && v >= ntc_lut[i+1] ) {
            float g_delta = ntc_lut[i] - ntc_lut[i+1];
            float m_delta = ntc_lut[i] - v;
            float ratio = m_delta / g_delta;
            return ntc_start_temp + ((float)i + ratio) * ntc_temp_step;
        }
    }
    
    // Should not get here
    return 0.0;
}

//static float volt_pot_convert(float v)
