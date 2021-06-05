// Deg C
static const uint ntc_start_temp = 20;
static const uint ntc_temp_step = 5;

#define L(v) ( (float)(v) * 3000.0f ),

// Each position is the resistance at ntc_start_temp + ntc_temp_step * index deg C
// NTC, so ntc_lut[0] > ntc_lut[1]
static const float ntc_lut[] = {
L(1.25)
L(1.0)
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

static const uint ntc_lut_size = sizeof(ntc_lut) / sizeof(float);
