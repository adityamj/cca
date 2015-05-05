#include <cca.h>
#include <cca_config.h>
#include <cca_rng.h>
#include <cca_state.h>
#include <cca_ca.h>
#include <transition_helper.h>

static struct cca_state state = CCA_STATE_INIT;
double birth_p(){
	double p;
	p = ((conf.birth_rate) * state.population )	/ (12*( conf.size*conf.size*conf.size - state.population ));
	return p;
}
void new_infected(){ state.total_infected++; }

void new_lsil(){ state.lsil_total++; }

void reset_yearly_counts(){//func
	state.yearly_vaccinated = 0;
	state.yearly_screened = 0;
	state.death = 0;
	state.pop_yb = state.population;
	state.ytd = 1;
}
void hsil_to_lsil(){ state.hsil_to_lsil++;  new_lsil();}

void new_hsil(){ state.hsil_total++; }

void lsil_to_normal(){ state.lsil_to_normal++; }

void infection_recovered(){ state.infected_to_normal++; }

void new_cancer(){ state.total_cancer++; }

void new_month(){	state.ytd++;}

int should_die(struct ca_cell * cell ){	//func

	return chooseit( conf.death_rate / 12); 
}

void death(){	state.population--; state.death++; }

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
		ret = ( conf.l_to_n_frac + 
				( conf.l_to_n_frac - (double) state.lsil_to_normal/(double)state.lsil_total)
			  )	/	(double)( 120 - conf.l_to_n_time) ;
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
	
	if ( age_of_infection > conf.l_to_h_time){
		ret =  (conf.h_to_l_frac + ( conf.h_to_l_frac - (double)state.hsil_to_lsil/(double)state.hsil_total ))/( 120 - conf.h_to_l_time) ;
	}
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

void print_state_stats(FILE *fp){
	fprintf(fp, "population: %d\n\
Infected:\t %d\n\
HSIL_total:\t %d\n\
HSIL_to_LSIL ratio:\t %f\n\
LSIL_total:\t %d\n\
LSIL to normal ratio:\t %f \n\
dead:\t %d\n",state.population, 
			state.total_infected - state.infected_to_normal	, 
			state.hsil_total, 
			state.hsil_to_lsil/(double)state.hsil_total,
			state.lsil_total,state.lsil_total/(double)state.lsil_to_normal,
			state.death);
}

