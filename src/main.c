#include "dsk6713.h"					// for the DSK6713_waitusec() functi
#include "dsk6713_aic23.h"				// support file for codec, DSK
#include "dsk6713_dip.h"				// for the DSK6713_DIP_get() function
#include "dsk6713_led.h"				// for the DSK6713_LED_xx() functions
//#include "c67fastMath.h"
//#include "math.h"
#include "../includes/defines.h"
#include "../includes/notch.h"

//sampling
Uint32 fs = DSK6713_AIC23_FREQ_48KHZ;	// set sampling rate, this is required by C6713dskinit
int fss = 48e3;

//notch filter
int notch0f0 = 50;						// notch 0 initianlization frequency
float r0 = 0.9971;						// notch 0 radial length of pole.
static notch *notchfilter;
int f0 = 50;							// start notch frequency
int f0last = 50;

//int samples = 2048;					// set number of samples
typedef union {
	Uint32 combo;						// buffer / data for reading / writing to audio channels
	short channel[2];
} codec_data;
codec_data  codec_out;					// audio data out
codec_data  codec_in;					// audio data in
short temp;

short iTwid[N/2];						// twiddle factor array W
short iData[N];							// indexing for bit reversing
float Xmag[N];							// X magnitude data
typedef struct complex_tag {float re, im;} complex;	// for complex numbers
complex xfft[N];							// The fft data
complex W[N/RADIX];						// twiddle constants
#pragma DATA_ALIGN(W, sizeof(complex))
#pragma DATA_ALIGN(xfft, sizeof(complex))
int short fft_trig = 0;


// buffers
complex IOBUF1[(N)];
complex IOBUF2[(N)];
complex *FFTBUF = IOBUF1;
complex *INPUTBUF = IOBUF2;
complex *INPUTBUF0 = IOBUF1;						// start adress of IOBUF
complex *FFTBUF0 = IOBUF1;;							// start adress of IOBUF
//short int BUFFFULL = 0;

int n = 0;	//256;

// prototyping {i hate warnings}
Uint32 input_sample();
void output_sample(Uint32);
void comm_intr();
void cfftr2_dit(complex *,complex *t,int);
void digitrev_index(short *, int, int);
float sin(float);
void bitrev(complex *, short *, int);
float sqrt(float);
int detect_peak(float *, int);


int  i,k,m,n,p,q,u;						// for iterations


int detect_peak(float *Xmag0, int N0){
	float peak0 = 0.0;							//
	for (i = 0 ; i < N ; i++){
		if(Xmag0[i] > peak0){

		}
	}
	return(peak0);
}


interrupt void c_int11()
{

	codec_in.combo = input_sample();				// sample the left and right audio channels
	codec_out.channel[1] = codec_in.channel[0];		// for standard ouput (original signal)

	if((n == N)){
		FFTBUF = INPUTBUF0;
		INPUTBUF = FFTBUF0;
		FFTBUF0 = FFTBUF;
		INPUTBUF0 = INPUTBUF;
		fft_trig = 1;
		n = 0;
	}else{
		INPUTBUF->re = codec_in.channel[0];
		INPUTBUF->im = 0.0;
		INPUTBUF++;
		n++;
	}
	//
	codec_out.channel[0] = increment_notch(notchfilter, codec_in.channel[0]);				// output left and right channels
	output_sample(codec_out.combo);					//output sample
	return;
}


void main()
{

	INPUTBUF0 = INPUTBUF;						// start adress of IOBUF
	FFTBUF0 = FFTBUF;							// start adress of IOBUF

	// FFTr2.c TI lib
	// spawn twiddle factors
	for ( i = 0 ; i < N/RADIX ; i++)
	{
		W[i].re = cos(DELTA*i);
		W[i].im = sin(DELTA*i);
	}

	digitrev_index(iTwid,(N/RADIX), RADIX);			// bitreversed twiddle factor index
	bitrev(W, iTwid, (N/RADIX));					//	" for twiddle data

	// init the output mag (set all 0, nothing undefines)
	for(i = 0; i < N; i++){
			Xmag[i] = 0;
	}

	init_notch(notchfilter,notch0f0, fss, r0);		// initialize filter with notch @ 50HZ,
	comm_intr(); //init DSK, codec, McBSP
	while(1){
		if(fft_trig == 1){

			cfftr2_dit(FFTBUF, W, N);
			digitrev_index(iData, N, RADIX);
			bitrev(FFTBUF,iData, N);

			for (i = 0; i < N; i++){
				Xmag[i] = sqrt(xfft[i].re*xfft[i].re + xfft[i].im*xfft[i].im)/32;	// Magnitude
			}

			f0 = 50; //detect_peak(Xmag, N);
			if(f0 != f0last){
				update_notch_coeff(notchfilter, 50);
			}
			f0last = f0;

			fft_trig = 0;
		}
	}
}
