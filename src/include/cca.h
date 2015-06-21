#ifndef __CCA_H__
#define __CCA_H__
#define MIN_AGE 15
#define MAX_AGE 65

struct cli_opts {
	int procs;
	char * data_dir;
	char * conf;
	char * prefix;
};

struct cca_conf{
	int size;
	double population_density;
	double age_ratio[70];
	double birth_rate;
	int iterations;
	int pass;
	double calib_factor;
	double death_rate;
	double activity[4][4]; /*[age_group][activity_group]	*/
	double age_dist[4][4]; /*[age_group][activity_group]	*/
	double p_transmission;
	double init_prevalence;
	double screening_rate;
	double v_eff;
	int min_age;
	int max_age;
	double asir[70];	
	double i_to_n[70]; 
	int i_to_n_time;	/* in months	*/
	double i_to_l;
	int   i_to_l_time;
	double i_to_h;
	double l_to_n[70];
	int   l_to_n_time;
	double l_to_n_frac;
	double l_to_h[70];
	int   l_to_h_time;
	double h_to_l;
	int h_to_l_time;
	double h_to_l_frac;
	double h_to_c;
	int h_to_c_time;
	double r_to_n;
	int r_to_n_time;
	double c_to_d;
	int c_to_d_time;
	int enable_vaccination;
	double vaccination_rate;
	double vaccine_efficacy;
};

extern struct cli_opts opts;
extern struct cca_conf conf;

#endif	
