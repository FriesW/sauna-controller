#ifndef _UTIL_H
#define _UTIL_H

typedef unsigned int uint;
typedef unsigned long ulong;

void halt();
void set_relay(bool target);
bool cycle();
void kick();

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#endif
