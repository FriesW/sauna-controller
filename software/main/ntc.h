// Deg C
static uint ntc_start_temp = 30;
static uint ntc_temp_step = 5;

#define L(v) (5.0 * ( (float)(v) * 3000.0 ) / ( ( (float)(v) * 3000.0 ) + 2000.0 ) ),

// Each position is the voltage at ntc_start_temp + ntc_temp_step * index deg C
// NTC, so ntc_lut[0] > ntc_lut[1]
static float ntc_lut = {
L(0.8054)
L(0.6528)
L(0.5324)
L(0.4368)
L(0.3603)
L(0.2989)
L(0.2492)
L(0.2088)
L(0.1758)
L(0.1487)
L(0.1263)
L(0.1078)
L(0.09233)
L(0.07941)
L(0.06857)
};

static uint ntc_lut_size = sizeof(ntc_lut) / sizeof(float);

