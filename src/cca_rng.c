#include <gsl/gsl_rng.h>
#include <stdlib.h>
#include <cca_rng.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static gsl_rng * r;

int cca_rng_init(){
	int fd;
	unsigned long int seed;
	int rv;
	const gsl_rng_type * T;
	fd = open("/dev/urandom", O_RDONLY);
	if( fd < 0){
		fprintf(stderr,"Error opening /dev/urandom. exiting\n");
		return (1);
	}
	while( (rv =	read( fd, &seed, sizeof(seed))) == 0 )	;	
	close(fd);
	setenv( "GSL_RNG_TYPE",DEFAULT_RNG_TYPE, 0);
	fprintf(stderr, "SEED: %lu\n",seed);
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc (T);
	gsl_rng_set(r, seed);
	return 0;
}

double cca_rng_get(){
	double rand;
	rand = gsl_rng_uniform(r);
	return rand;
}

int chooseit (double p){
	int ret = 0 ;
	double rand;
	rand = cca_rng_get();
	if ( rand <= p)
		ret = 1;

	return ret;
}
