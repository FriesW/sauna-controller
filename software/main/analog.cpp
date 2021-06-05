#include <Arduino.h>
#include "util.h"
#include "analog.h"
#include "ntc.h"
#include "io.h"

struct range_struct{
    float ratio;
    int states[3];
};
#define RANGE_CNT (6)
static const struct range_struct ranges[RANGE_CNT] = {
    {0.197567f, {-1, -1,  1} },
    {0.246211f, {-1,  0,  1} },
    {0.395135f, {-1,  1,  1} },
    {0.969696f, { 0,  0,  1} },
    {0.984615f, { 0,  1,  1} },
    {0.993826f, { 1,  1,  1} },
};
static const uint8_t aref_pins[3] = {AREF_330, AREF_1K0, AREF_1K1};
static void set_range(const range_struct * range);

static void read(float * v, float scale, uint8_t pin);
static void ntc_off();
static void ntc_on();

void anlg_init(){
    analogReference( EXTERNAL );
    ntc_off();
}

void anlg_update(){

    float pcb_r = -1.0f;
    float room_r = -1.0f;
    float pot_r = -1.0f;

    ntc_on();
    for(uint8_t i = 0; i < RANGE_CNT; i++){
        set_range( &ranges[i] );
        read( &pcb_r, ranges[i].ratio, PCB_TEMP );
        read( &room_r, ranges[i].ratio, ROOM_TEMP );
        read( &pot_r, ranges[i].ratio, POT );
    }
    ntc_off();

    Serial.print(pcb_r);
    Serial.print(" ");
    Serial.print(room_r);
    Serial.print(" ");
    Serial.println(pot_r);


}

static void read(float * v, float scale, uint8_t pin){
    if( *v > 0.0f ) return;
    uint16_t vt = analogRead(pin) + analogRead(pin) + analogRead(pin) + analogRead(pin);
    if( vt > (1020 * 4) ) return;
    *v = (float)vt / (1023.0f * 4.0f) / scale;
}

static void ntc_off(){
    pinMode(PCB_TAP, OUTPUT);
    digitalWrite(PCB_TAP, LOW);
    pinMode(ROOM_TAP, OUTPUT);
    digitalWrite(ROOM_TAP, LOW);
}

static void ntc_on(){
    pinMode(PCB_TAP, INPUT);
    pinMode(ROOM_TAP, INPUT);
}

/*
void anlg_read_avg(uint pin, float * val) {
    // 1k + 1k divider + internal 32k pulldown on AREF
    #define AREF_V (2.461538)
    float v = (float)analogRead(pin);
    v = v / ((float)(1<<10) - 1.0) * AREF_V;
    if( *val <= 0.0 || *val >= AREF_V )
        *val = v;
    else
        *val = v * 0.1 + (*val) * 0.9;
    #undef AREF_V
}

// Return temperature in deg C, readings saturate at ends of range
float volt_ntc_convert(float v) {
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
float volt_pot_convert(float v) {
    if (v <= 0.0) return 0.0;
    if (v >= 2.5) return 1.0;
    float p = ( 330.0 * v / (5.0 - v) ) / 300.0;
    if (p >= 1.0) return 1.0;
    if (p <= 0.0) return 0.0;
    return p;
}
*/

static void set_range(const range_struct * range){
    #define SET_PIN(p, s) { \
        pinMode( (p), (s) == 0 ? INPUT : OUTPUT); \
        if((s) != 0) digitalWrite((p), (s) == 1); \
    }
    #define SET_LUT(lut, id) SET_PIN( aref_pins[id], lut[id] )
    #define SET(lut) SET_LUT(lut, 0) SET_LUT(lut, 1) SET_LUT(lut, 2)

    SET(range->states)
    delay(6);
    (void)analogRead(PCB_TEMP);

    #undef SET_PIN
    #undef SET_LUT
    #undef SET
}

/*
static float range_set(uint range) {
    static const uint aref_pins[] = {AREF_330, AREF_1K0, AREF_1K1};
    static const int states[][] = {
        {-1, -1,  1},
        {-1,  0,  1},
        {-1,  1,  1},
        { 0,  0,  1},
        { 0,  1,  1},
        { 1,  1,  1},
    };
    static const float ratios[] = {
        0.197567f,
        0.246211f,
        0.395135f,
        0.969696f,
        0.984615f,
        0.993826f,
    };
    #define SET_PIN(p, s) { pinMode( (p), (s) == 0 ? INPUT : OUTPUT); if((s) != 0) digitalWrite((p), (s) == 1); }
    #define SET_LUT(lut, id) SET_PIN( aref_pins[id], lut[id] )
    #define SET(lut) SET(lut, 0) SET(lut, 1) SET(lut, 2)

    if( range > 5 ) return -1.0f;
    SET( states[range] );
    delay(6);
    return ratios[range];

    #undef SET_PIN
    #undef SET_LUT
    #undef SET
}
*/
