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

static void read(float * v, float scale, uint8_t pin, uint16_t upper_r);
static void ntc_off();
static void ntc_on();
static float resist_ntc_convert(float v);

float pcb_temp;
float room_temp;
float pot_pos;

void anlg_init(){
    analogReference( EXTERNAL );
    ntc_off();
    pcb_temp = -1.0f;
    room_temp = -1.0f;
    pot_pos = 0.0f;
}

void anlg_update(){

    float pcb_r = -1.0f;
    float room_r = -1.0f;
    float pot_r = -1.0f;

    ntc_on();
    for(uint8_t i = 0; i < RANGE_CNT; i++){
        set_range( &ranges[i] );
        read( &pcb_r, ranges[i].ratio, PCB_TEMP, 2000 );
        read( &room_r, ranges[i].ratio, ROOM_TEMP, 2000 );
        read( &pot_r, ranges[i].ratio, POT, 330 );
    }
    ntc_off();

    if( pcb_r > 0.0f )
        pcb_temp = resist_ntc_convert(pcb_r);
    else
        pcb_temp = -1.0f;
    if( room_r > 0.0f )
        room_temp = resist_ntc_convert(room_r);
    else
        room_temp = -1.0f;
    pot_r = pot_r / 300.0f;
    pot_pos = 1.0f - MIN(1.0f, MAX(pot_r, 0.0f) );

    Serial.print("PCB deg C: ");
    Serial.print(pcb_temp);
    Serial.print("  Room deg C: ");
    Serial.print(room_temp);
    Serial.print("  Pot pos 0-1: ");
    Serial.print(pot_pos);
}

static void read(float * v, float scale, uint8_t pin, uint16_t upper_r){
    // Always read, deterministic timing
    uint16_t vt = analogRead(pin) + analogRead(pin) + analogRead(pin) + analogRead(pin);
    // Do nothing if already set or reading out of range
    if( *v > 0.0f ) return;
    if( vt > (1020 * 4) ) return;
    // Convert to ratio 0-1
    *v = (float)vt / (1023.0f * 4.0f) / scale;
    // Convert to actual resistance
    *v = *v * (float)upper_r / ( 1.0f - *v );
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

// Return temperature in deg C, readings saturate at ends of range
static float resist_ntc_convert(float r) {
    if( r > ntc_lut[0] )
        return ntc_start_temp;
    if( r < ntc_lut[ntc_lut_size-1] )
        return ntc_start_temp + (ntc_lut_size-1) * ntc_temp_step;
    for(uint i = 0; i < ntc_lut_size - 1; i++) {
        if( ntc_lut[i] >= r && r >= ntc_lut[i+1] ) {
            float g_delta = ntc_lut[i] - ntc_lut[i+1];
            float m_delta = ntc_lut[i] - r;
            float ratio = m_delta / g_delta;
            return ntc_start_temp + ((float)i + ratio) * ntc_temp_step;
        }
    }

    // Should not get here
    return 0.0;
}

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
