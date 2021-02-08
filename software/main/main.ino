#define P_RQST (4)
#define SR_RST (3)
#define WD_KICK (2)

#define BD_LED (13)

#define PCB_TEMP (16)
#define ROOM_TEMP (15)
#define POT (14)

typedef unsigned int uint;

#include "thermostat.h"
#include "ntc.h"


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

static void halt() {
    set(false);
    while(true);
}

static void set(bool target) {
    digitalWrite(BD_LED, target);
    digitalWrite(P_RQST, target);
}

static bool cycle(){
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


static void kick() {
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

static void anlg_read_avg(uint pin, float * val) {
    #define AREF_V (2.5)
    if( *val <= 0.0 || *val >= AREF_V )
        *val = (float)analogRead(pin);
    float v = (float)analogRead(pin);
    v = v / ((float)(1<<10) - 1.0) * AREF_V;
    *val = v * 0.1 + *val * 0.9;
    #undef AREF_V
}

// Return temperature in deg C, readings saturate at ends of range
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

// Return 0.0 - 1.0 for pot position
static float volt_pot_convert(float v) {
    if (v <= 0.0) return 0.0;
    if (v >= 2.5) return 1.0;
    float p = ( 330.0 * v / (5.0 - v) ) / 300.0;
    if (p >= 1.0) return 1.0;
    if (p <= 0.0) return 0.0;
    return p;
}

