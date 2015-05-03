#include <cca.h>
#include <cca_config.h>
#include <cca_rng.h>
#include <cca_state.h>
#include <cca_ca.h>
#include <transition_helper.h>

static struct cca_state state = CCA_STATE_INIT;
double birth_p(){
	double p;
	p = ((conf.birth_rate * state.population) /  ( conf.size*conf.size*conf.size - state.population))/12;
	return p;
}
void new_infected(){ state.total_infected++; }

void new_lsil(){ state.lsil_total++; }

void reset_yearly_counts(){//func
	state.yearly_vaccinated = 0;
	state.yearly_screened = 0;
}
void hsil_to_lsil(){ state.hsil_to_lsil++; }

void new_hsil(){ state.hsil_total++; }

void lsil_to_normal(){ state.lsil_to_normal++; }

void infection_recovered(){ state.infected_to_normal++; }

void new_cancer(){ state.total_cancer++; }

int should_die(struct ca_cell * cell ){	//func
	static double m_death_rate;
	double rand;
	static int initialized;
	int ret = 0;
	if(!initialized){
		initialized = 1;
		m_death_rate = conf.death_rate / 12 ;
	}
	if( (rand = cca_rng_get()) <= m_death_rate)
		ret = 1;

	return ret;
}

void death(){	state.population--; }

void birth(){	state.population++; }

int get_population(){	//func
	return state.population;
}

double i_to_n_p( int age){	//func
	double ret;
 	ret = ( conf.i_to_n[ age/12 - MIN_AGE ]) ;
	return ret;
}

double i_to_l_p(){	//func
	double ret;
	ret = (conf.i_to_l/conf.i_to_l_time);
#ifdef DEBUG
	ret = 0.35 / 72;
#endif
	return ret;
} 

double l_to_i_p(int age, int age_of_infection){	//func
	double ret;
	if( age_of_infection > conf.l_to_n_time )
		ret = ( conf.l_to_n_frac - (double) state.lsil_to_normal/(double)state.lsil_total)/(double)( 120 - conf.l_to_n_time) ;
	else
		ret = conf.l_to_n[ age/12 -MIN_AGE] ;
#ifdef DEBUG
	ret = 0.65 / 120;
#endif

	return ret;
}

double l_to_h_p ( int age) { //func
	double ret;
	ret = conf.l_to_h[ age/12 - MIN_AGE] / conf.l_to_h_time	;
#ifdef DEBUG
	ret = 0.3/72;
#endif
	return ret;		
}

double h_to_l_p( int age_of_infection){	//func
	double ret;
	
	if ( age_of_infection > conf.l_to_h_time)
		ret = ( conf.h_to_l_frac - (double)state.hsil_to_lsil/(double)state.hsil_total )/( 120 - conf.h_to_l_time) ;
	else
		ret = conf.h_to_l / conf.h_to_l_time;
	
#ifdef DEBUG
	ret = 0.4/120;
#endif
	return ret;
}

double h_to_c_p(){ //func
	double ret;
	ret =  conf.h_to_c / conf.h_to_c_time ;
	return ret;
}

double c_to_d_p(){
	double ret;
	ret = conf.c_to_d / conf.c_to_d_time;
	return ret;
}

