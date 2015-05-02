#ifndef __CCA_RNG_H__
#define __CCA_RNG_H__

#define DEFAULT_RNG_TYPE "ranlxd2"

extern int cca_rng_init();
extern double cca_rng_get();
extern int chooseit( double p);

#endif
