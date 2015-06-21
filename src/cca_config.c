#include <stdio.h>
#include <cca.h>
#include <cca_config.h>
#include <libconfig.h>
#define ERR(x) fprintf(stderr,"%s\n",x)
int parse_conf( const struct cli_opts * opts, struct cca_conf * conf){
	config_t s_cfg, *cr;
	const config_setting_t * cst; 
	int counter;
	int ret=0;
	int err=0;
	cr = &s_cfg;
	config_init(cr);
	if( config_read_file( cr, opts->conf) == CONFIG_FALSE ) {
		fprintf( stderr, "%s:%d - %s\n",
			config_error_file(cr),
			config_error_line(cr),
			config_error_text(cr));
		config_destroy(cr);
		return 1;
	}
	if( !config_lookup_int( cr, "size", &conf->size)){
		ERR("size is compulsory"); err++;	}
	if( !config_lookup_float( cr, "population_density", &conf->population_density)){
		ERR("population_density is required"); err++; }
	if( !config_lookup_float(cr,"birth_rate", &conf->birth_rate)){
		ERR("birth_rate required"); err++; }
	if( !config_lookup_int( cr, "iterations", &conf->iterations)){
		ERR("iterations required"); err++;	}
	if( !config_lookup_float( cr, "l_to_n_frac", &conf->l_to_n_frac)){
		ERR("l_to_n_frac required"); err++;	}
	if( !config_lookup_float( cr, "death_rate", &conf->death_rate)){
		ERR("death_rate required"); err++;	}
	if( !config_lookup_int(cr,"pass", &conf->pass)){
		ERR("pass required"); err++;	}
	if( !config_lookup_float(cr,"p_transmission",&conf->p_transmission)){
		ERR("p_transmission required"); err++;	}
	if( !config_lookup_int( cr, "i_to_n_time", &conf->i_to_n_time)){
		ERR("i_to_n_time required");	err++;	}
	if( !config_lookup_float(cr, "i_to_l", &conf->i_to_l)){
		ERR("i_to_l required");	err++;	}
	if( !config_lookup_int(cr, "i_to_l_time", &conf->i_to_l_time)){
		ERR("i_to_l_time required"); err++;	}
	if( ! config_lookup_float(cr, "i_to_h" , &conf->i_to_h)){
		ERR("i_to_h required"); err++;	}
	if( !config_lookup_int( cr, "l_to_n_time", &conf->l_to_n_time)){
		ERR("l_to_n_time required"); 	err++;	}
	if( !config_lookup_int(cr, "l_to_h_time", &conf->l_to_h_time)){
		ERR("l_to_h_time required");	err++;	}
	if( !config_lookup_float( cr, "h_to_l", &conf->h_to_l)){
		ERR("h_to_l require");	err++;	}
	if( !config_lookup_int(cr, "h_to_l_time",&conf->h_to_l_time)){
		ERR("h_to_l_time required");	err++;	}
	if( !config_lookup_float(cr,"h_to_l_frac", &conf->h_to_l_frac)){
		ERR("h_to_l_frac required");	err++;	}
	if( !config_lookup_float(cr,"h_to_c", &conf->h_to_c)){
		ERR("h_to_c required");	err++;	}
	if( !config_lookup_int( cr, "h_to_c_time", &conf->h_to_c_time)){
		ERR("h_to_c_time required");	err++;	}
	if( !config_lookup_int( cr, "enable_vaccination", &conf->enable_vaccination)){
		ERR("vaccination_enable required"); err++;	
	}
	if( &conf->enable_vaccination){
	if( !config_lookup_float( cr, "vaccination_rate", &conf->vaccination_rate)){
		ERR("vaccination rate required if vaccination enabled");	err++;	}
		
	if( !config_lookup_float( cr, "vaccine_efficacy", &conf->vaccine_efficacy)){
		ERR("vaccination efficacy required if vaccination enabled");	err++;	}
		
	}
	if( !config_lookup_float(cr, "r_to_n", &conf->r_to_n )){
		ERR("r_to_n required");	err++;	}
	if( !config_lookup_int(cr, "r_to_n_time", &conf->r_to_n_time)){
		ERR("r_to_n_time required");	err++;	}
	if( !config_lookup_float(cr , "c_to_d", &conf->c_to_d)){
		ERR("c_to_d required");	err++;	}
	if( !config_lookup_int( cr, "c_to_d_time", &conf->c_to_d_time)){
		ERR("c_to_d_time required");	err++;	}
	if( !config_lookup_float( cr, "calib_factor", &conf->calib_factor)){
		ERR("calib_factor required");	err++;	}
	if( (cst = config_lookup(cr, "age_ratio")) == NULL){
		ERR("age_ratio required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 51){
			ERR("age_ratio must be provided for 15-65"); err++;}
		else{
			for(counter = 0; counter < 51; counter++)
			conf->age_ratio[counter] = config_setting_get_float_elem(cst, counter);
		}
	}

	if( (cst = config_lookup(cr, "activity")) == NULL){
		ERR("activity required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 16){
			ERR("16 different values must be provided for activity"); err++;}
		else{
			for(counter = 0; counter < 16; counter++)
			conf->activity[counter/4][counter%4] = config_setting_get_float_elem(cst, counter)/conf->calib_factor;
		}
	}


	if( (cst = config_lookup(cr, "age_dist")) == NULL){
		ERR("age_dist required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 16){
			ERR("16 different values must be provided for age_dist"); err++;}
		else{
			for(counter = 0; counter < 16; counter++)
			conf->age_dist[counter/4][counter%4] = config_setting_get_float_elem(cst, counter);
		}
	}

	if( (cst = config_lookup(cr, "asir")) == NULL){
		ERR("asir required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 51){
			ERR("asir must be provided for 15-65"); err++;}
		else{
			for(counter = 0; counter < 51; counter++)
			conf->asir[counter] = config_setting_get_float_elem(cst, counter);
		}
	}


	if( (cst = config_lookup(cr, "i_to_n")) == NULL){
		ERR("i_to_n required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 51){
			ERR("i_to_n must be provided for 15-65"); err++;}
		else{
			for(counter = 0; counter < 51; counter++)
			conf->i_to_n[counter] = config_setting_get_float_elem(cst, counter);
		}
	}

	if( (cst = config_lookup(cr, "l_to_n")) == NULL){
		ERR("l_to_n required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 51){
			ERR("l_to_n must be provided for 15-65"); err++;}
		else{
			for(counter = 0; counter < 51; counter++)
			conf->l_to_n[counter] = config_setting_get_float_elem(cst, counter);
		}
	}

	if( (cst = config_lookup(cr, "l_to_h")) == NULL){
		ERR("l_to_h required");	err++;
	}
	else{
		if( (counter = config_setting_length(cst)) < 51){
			ERR("l_to_h must be provided for 15-65"); err++;}
		else{
			for(counter = 0; counter < 51; counter++)
			conf->l_to_h[counter] = config_setting_get_float_elem(cst, counter);
		}
	}
	config_write(cr, stderr);
	return ret;
}


