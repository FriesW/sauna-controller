#include <Arduino.h>
#include "util.h"
#include "analog.h"
#include "ntc.h"

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

static float auto_range(uint pin) {
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
    static const int init_state[] = {-1, -1, -1};

    #define SET_PIN(p, s) { pinMode( (p), (s) == 0 ? INPUT : OUTPUT); if((s) != 0) digitalWrite((p), (s) == 1); }
    #define SET_LUT(lut, id) SET_PIN( aref_pins[id], lut[id] )
    #define SET(lut) SET(lut, 0) SET(lut, 1) SET(lut, 2)

    SET( init_state );
    delay(5);

}
