#include <gsl/gsl_rng.h>
#include <stdlib.h>

static gsl_rng * r;

int cca_rng_init(){
	FILE * fp;
	unsigned long int seed;
	const gsl_rng_type * T;
	fp = fopen("/dev/urandom","r");
	if( fp == NULL){
		fprintf(stderr,"Error opening /dev/urandom. exiting\n");
		return (1);
	}
	fread(&seed, sizeof(seed), 1, fp);	
	setenv( "GSL_RNG_TYPE",DEFAULT_RNG_TYPE, 0);
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc (T);
	gsl_rng_set(r, seed);
	return 0;
}

int chooseit (double p){
	int ret 0 ;
	double rand;
	rand = cca_rng_get();
	if ( rand <= p)
		ret = 1;
}
double cca_rng_get(){
	return gsl_rng_uniform(r);
}
