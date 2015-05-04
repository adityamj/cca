#ifndef __TRANSITION_HELPER_H__
#define __TRANSITION_HELPER_H__
#include <cca_ca.h>
#include <cca.h>

extern double birth_p();
extern double c_to_d_p();
extern void new_infected(); //func
extern void reset_yearly_counts();//func
extern void hsil_to_lsil();//func
extern void new_hsil();//func
extern void lsil_to_normal();//func
extern void infection_recovered();//func
extern void new_cancer();//func
extern int should_die(struct ca_cell * cell );	//func
extern void death();	//func
extern void birth();	//func
extern int get_population();	//func
extern double i_to_n_p( int age);	//func
extern double i_to_l_p();	//func
extern double l_to_i_p(int age, int age_of_infection);	//func
extern double l_to_h_p ( int age) ; //func
extern double h_to_l_p( int age_of_infection);	//func
extern double h_to_c_p(); //func
extern void new_lsil();
extern void new_month();
#endif
