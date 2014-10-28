#ifndef _H_NOTCH
#define _H_NOTCH
#include "../includes/defines.h"
//The notch filter
//The notch filter
typedef struct notch_tag{
	float r;						// the radial length of the poles
	int fs;							// sampling frequency of ADC
	int f0;							// set cutoff frequency
	short x[3];						// input delays
	short y[3];						// output delays
	int delayN;						// number of input delays
	// filter coeffecients
	float num[3];					// numerator coefficients
	float den[3];					// denominator coeffecients
}notch;
//notch *notchfilter;


float cos(float);

void init_notch(notch *, int f0, int fs, float r);
void update_notch_coeff(notch *, int);
int increment_notch(notch *, short);
#endif	//_H_NOTCH
