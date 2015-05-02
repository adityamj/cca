#ifndef __TRANSITION_HELPER_H__
#define __TRANSITION_HELPER_H__
#include <cca_ca.h>
#include <cca.h>

extern int should_die(struct ca_cell * cell );	//func
extern void death();	//func
extern void birth();	//func
extern int get_population();	//func
extern double i_to_n_p( int age);	//func
extern double i_to_l_p();	//func
extern double l_to_i_p(int age, int age_of_infection);	//func
extern double h_to_l_p( int age_of_infection);	//func
extern double h_to_c_p(); //func
extern void reset_yearly_counts();

#endif
