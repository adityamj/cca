#include <cca.h>
#include <cca_groups.h>
#include <cca_rng.h>

int tellag( int age_inmonths){
	if( age_inmonths/12 >= 15 && age_inmonths/12 <= 19)
		return 0;
	else if ( age_inmonths/12 >= 20 && age_inmonths/12 <= 24)
		return 1;
	else if ( age_inmonths/12 >= 25 && age_inmonths/12 <= 34)
		return 2;
	else if ( age_inmonths/12 >= 35 && age_inmonths/12 <= 64)
		return 3;
	return -1;
}

double num_part( int age, int acg){
	return (conf.activity[tellag(age)][acg]);
}

int is_ag_transition( int age){
	int ret=0;
	if( age == 15*12 || age == 20*12 || age == 25*12 || age == 35*12)
		ret = 1;
	return ret;
}

int acg_oracle( int age_inmonths){

	int age_group;
	double rand;
	double p;
	age_group = tellag(age_inmonths);
	while(1){
	rand = cca_rng_get();
	if( rand <= ( p = conf.age_dist[age_group][0]) )
		return 0;
	else if ( rand <= ( p+= conf.age_dist[age_group][1]))
		return 1;
	else if ( rand <= ( p+= conf.age_dist[age_group][2]))
		return 2;
	else if ( rand <= ( p+= conf.age_dist[age_group][3]))
		return 3;
	}
} 
