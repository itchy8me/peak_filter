#include "../includes/notch.h"


void update_notch_coeff(notch *notchfilter, int f0){
	notchfilter->f0 = f0;
	notchfilter->num[0] = 1.0;
	notchfilter->num[1] = -2*(notchfilter->r)*cos((2*PI*(notchfilter->f0))/notchfilter->fs);
	notchfilter->num[2] = notchfilter->r*notchfilter->r;
	notchfilter->den[0] = 1.0;
	notchfilter->den[1] = -2*cos((2*PI*notchfilter->f0)/notchfilter->fs);
	notchfilter->den[2] = 1.0;
}


void init_notch(notch *notchfilter, int f0, int fs, float r){
	notchfilter->r = r;						// the radial length of the poles
	notchfilter->fs = fs;
	notchfilter->x[0] = 0;					// input delays
	notchfilter->x[1] = 0;					// input delays
	notchfilter->x[2] = 0;					// input delays
	notchfilter->y[0] = 0;					// input delays
	notchfilter->y[1] = 0;					// input delays
	notchfilter->y[2] = 0;					// input delays
	notchfilter->delayN = 2;							// number of input delays
	// filter coeffecients
	update_notch_coeff(notchfilter, f0);
}


int increment_notch(notch *notchfilter, short x){
	short y = 0;
	notchfilter->x[(notchfilter->delayN)-2] = notchfilter->x[(notchfilter->delayN)-1];								// delay
	notchfilter->x[(notchfilter->delayN)-1] = notchfilter->x[(notchfilter->delayN)];									// delay
	notchfilter->x[(notchfilter->delayN)] = x;						// new input
	notchfilter->y[(notchfilter->delayN)-2] = notchfilter->y[(notchfilter->delayN)-1];								// delay output delay
	notchfilter->y[(notchfilter->delayN)-1] = notchfilter->y[(notchfilter->delayN)];									// delay output
	//y[notchfilter->delayN] = codec_in.channel[0];
	notchfilter->y[(notchfilter->delayN)] = notchfilter->den[0]*notchfilter->x[(notchfilter->delayN)] + notchfilter->den[1]*notchfilter->x[(notchfilter->delayN)-1] + notchfilter->den[2]*notchfilter->x[(notchfilter->delayN)-2] - notchfilter->num[1]*notchfilter->y[(notchfilter->delayN)-1] - notchfilter->num[2]*notchfilter->y[(notchfilter->delayN)-2]; // calculate new signal value
	y = notchfilter->y[(notchfilter->delayN)];
	return(y);
}
