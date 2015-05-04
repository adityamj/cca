#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cca_worker.h>
#include <transition_helper.h>
#include <cca_ca.h>
#include <cca.h>
#include <cca_config.h>
#include <cca_groups.h>
#include <cca_rng.h>
#include <string.h>
/*	Age
	Activity group
	existing infection
 */

void sim_birth( struct ca_cell * cell){
	double p;
	p = birth_p();
	if( chooseit(p)){
		cell->activity_group = acg_oracle( MIN_AGE * 12);
		cell->occupied = 1;
		cell->age = MIN_AGE * 12;
		birth();
		p = cca_rng_get();
		if ( p <= conf.asir[ cell->age-MIN_AGE ]){
			cell->current_status = 1;	
			new_infected();
		}
	}
}

void infected( struct ca_cell * cell){
	double p = 0;
	double rand;
	rand = cca_rng_get();
	cell->age_of_infection++;
	if( cell->age_of_infection <= 1){
		if( chooseit( conf.i_to_h)){
			new_hsil();
			cell->current_status = 3;
		}	
	}
	else {	
		if( rand <= ( p = i_to_n_p( cell->age))){	
			cell->current_status = 0;
			cell->age_of_infection = 0;
			infection_recovered();
		}
		else if ( rand <= ( p+= i_to_l_p())){
			cell->current_status = 2;
			new_lsil();
		}
	}
}

void hsil( struct ca_cell * cell){
	double p;
	double rand;
	rand = cca_rng_get();
	p = 0;
	cell->age_of_infection++;
	if( rand <= ( p = h_to_l_p( cell->age_of_infection))){ // recovery to lsil
		//if( cell->age_of_infection <= conf.h_to_l_time) 
			hsil_to_lsil();
		new_lsil();
		cell->current_status = 2;
	}
	else if( rand <= ( p += h_to_c_p()) ){	// cancer
		cell->current_status = 4;
		new_cancer();
		cell->age_of_infection = 0;
	}
}

void lsil( struct ca_cell * cell){
	double p;
	double rand;
	rand = cca_rng_get();
	p = 0;
	cell->age_of_infection++;
	if( rand <= ( p = l_to_h_p( cell->age))){ // recovery to lsil
		new_hsil( p);
		cell->current_status = 3;
	}
	else if( rand <= ( p += l_to_i_p(cell->age, cell->age_of_infection)) ){
		cell->current_status = 0;
		if( cell->age_of_infection <= conf.l_to_n_time)	
			lsil_to_normal();
		cell->age_of_infection = 0;
	}
}

void recovered( struct ca_cell * cell){
	int choice;
	choice = chooseit( conf.r_to_n / conf.r_to_n_time);
	if(choice)
		cell->current_status = 0;
}

void cancer( struct ca_cell * cell){
	int choice;
	choice = chooseit( c_to_d_p());
	if( choice){
		death();
		memset( cell, 0, sizeof(struct ca_cell));
		cell->num_p = 0;
	}

}

void populate(struct ca_grid * t){
	int size = 0;
	double rand = 0;
	int age;
	int target_population, age_target_population, achieved;
	int index;
	size = t->size * t->size * t->size;
	struct ca_cell * curr_cell;
	target_population = size * conf.population_density;
	fprintf(stderr, "polulation target:%d\n",target_population);
	for( age = MIN_AGE; age <= MAX_AGE ; age++){
		age_target_population = (double)target_population * conf.age_ratio[ age - MIN_AGE] ;
		achieved = 0;
		for ( achieved = 0; achieved <= age_target_population ; achieved++){
			rand = cca_rng_get();
			index = rand * size;
			curr_cell = t->cells + index;
			if( curr_cell->occupied){
				achieved--;
				continue;
			}
			birth();
			curr_cell->occupied = 1;
			curr_cell->age =age * 12 + 11.9* cca_rng_get();
			curr_cell->activity_group = acg_oracle(curr_cell->age);
			rand = cca_rng_get();
			if( curr_cell->current_status ){
				//very unlikely
				fprintf(stderr,"non zero status in populate. status:%d\tactivityg:%d\n",curr_cell->current_status, acg_oracle(curr_cell->age));
			}
			if ( rand <= conf.asir[ age -MIN_AGE ]){
				curr_cell->current_status = 1;	
				new_infected();
			}
		}
	}
}

int should_infect( const struct ca_grid *t, int x, int y, int z, double p){
	int xx, yy, zz;
	int ineigh=0;
	int ret = 0;
	const struct ca_cell * curr;
	xx = x -1;
	yy = y -1;
	zz = z -1;
	if( xx < 0)
		xx = 0 ;

	for( ; (xx <= ( x+1)) && (xx < t->size); xx++){
		yy = y -1;
		if( yy < 0)
			yy = 0;
		for(; (yy <= (y+1)) && (yy < t->size) ; yy++){
			zz = z -1;	
			if ( zz < 0)
				zz = 0;
			for( ;( zz <= (z+1)) &&( zz < t->size); zz++){
				curr = translate( t->cells, xx,yy,zz, t->size);
				if( xx == x && yy == y && zz == z)
					continue;
				if( curr->occupied){
					if( chooseit( p))
						if( curr->current_status % 5 !=0)
							if( chooseit( conf.p_transmission)){
								ret = 1;
								return ret;
							}
				}
			}
		}
	}
	return ret;
}

int get_num_ineigh( const struct ca_grid *t, int x, int y, int z){
	int xx, yy, zz;
	int neigh=0;
	const struct ca_cell * curr;
	xx = x -1;
	yy = y -1;
	zz = z -1;
	if( xx < 0)
		xx = 0 ;

	if( yy < 0)
		yy = 0;

	if ( zz < 0)
		zz = 0;

	for( ; (xx <= ( x+1)) && (xx < t->size); xx++){
		yy = y -1;
		if( yy < 0)
			yy = 0;
		for(; (yy <= (y+1)) && (yy < t->size) ; yy++){
			zz = z -1;	
			if ( zz < 0)
				zz = 0;
			for( ;( zz <= (z+1)) &&( zz < t->size); zz++){
				curr = translate( t->cells, xx,yy,zz, t->size);
				if( xx == x && yy == y && zz == z)
					continue;
				if( curr->occupied)
					if( curr->current_status % 5 !=0)
						neigh++;
			}
		}
	}
	return neigh;
}
int get_num_neigh( const struct ca_grid *t, int x, int y, int z){
	int xx, yy, zz;
	int neigh=0;
	struct ca_cell * curr;
	xx = x -1;
	yy = y -1;
	zz = z -1;
	if( xx < 0)
		xx = 0 ;

	if( yy < 0)
		yy = 0;

	if ( zz < 0)
		zz = 0;

	for( ; (xx <= ( x+1)) && (xx < t->size); xx++){
		yy = y -1;
		if( yy < 0)
			yy = 0;
		for(; (yy <= (y+1)) && (yy < t->size) ; yy++){
			zz = z -1;	
			if ( zz < 0)
				zz = 0;
			for( ;( zz <= (z+1)) &&( zz < t->size); zz++){
				curr = translate( t->cells, xx,yy,zz, t->size);
				if( xx == x && yy == y && zz == z)
					continue;
				if( curr->occupied)
					neigh++;
			}
		}
	}
	return neigh;
}

void simulate ( struct ca_grid * t, struct ca_grid * t1){
	int x,y,z;
	x=y=z=0;
	struct ca_cell * t_cell;
	struct ca_cell * t1_cell;
	int num_p;
	int num_neigh;
	int ret;
	double probability;
	for( x = 0; x < t->size; x++){
		for( y = 0; y < t->size; y++){
			for ( z = 0; z< t->size ; z++){
				t_cell = translate (t->cells , x,y,z, t->size);
				t1_cell = translate ( t1->cells, x,y,z, t->size);
				memcpy ( t1_cell, t_cell, sizeof(struct ca_cell));	// Initialize our new cell.
				if( t1_cell->occupied ){
					t1_cell->age++;
					if ( t1_cell->age > MAX_AGE *12){
						death();
						memset( t1_cell, 0, sizeof(struct ca_cell));
						continue;
					}
					if(should_die( t1_cell)){
						memset( t1_cell, 0, sizeof(struct ca_cell));
						t1_cell->num_p = 0;
						death();	
						continue;
					}
					if ( is_ag_transition( t1_cell->age)){
						t1_cell->activity_group = acg_oracle( t1_cell->age);
					}
					switch (t1_cell->current_status){
						case 0:{
							// S
							int g;
							g = tellag( t1_cell->age);
							if( g < 0)
								exit(507);	
							t1_cell->num_p += (double) num_part(t_cell->age, t_cell->activity_group)/(double) 12 ;
							num_p = t1_cell->num_p;
							t1_cell->num_p -= num_p;
							if ( ! num_p)
								continue;
							num_neigh = get_num_neigh( t, x,y,z);
							ret = should_infect( t,x,y,z, (double) num_p/ num_neigh);
/*							ret = chooseit( get_num_ineigh( t,x,y,z)/(double)get_num_neigh(t,x,y,z) *
									conf.activity[g][t1_cell->activity_group] / 12 * conf.p_transmission) ;				*/
							if(ret){
								// Infected
								t1_cell->ever_infected = 1;
								new_infected();
								t1_cell->age_of_infection = 0;
							}
							break;
						}
						case 1:{ // Infected
							infected( t1_cell);	
							break;
						}
						case 2:{ // lsil
							lsil( t1_cell);
							break;
						}
						case 3:{ //hsil
							hsil( t1_cell);
							break;
						}
						case 4:{ //cancer; nothing to do. wait. Accounting is already done by hsil
							cancer( t1_cell);
							break;
						}
						case 5:{
							// immune
							recovered( t1_cell);
							break;
						}
					}
				}
				else{
					// add a new individual to the simulation ?birth()
					sim_birth( t1_cell);
				}
			}	
		}
	}

}
int do_reporting( struct ca_grid * t, int count, int pid){
	static int init;
	static FILE * fp;
	int hsil[4] = {0,0,0,0}, lsil[4] = {0,0,0,0}, inf[4] = {0,0,0,0}, cancer[4] = {0,0,0,0};
	int c, limit;
	int ag;
	int sizeoffn;
	char * fname;
	struct ca_cell * curr;
	limit = conf.size*conf.size*conf.size;
	if( !init){
		sizeoffn = strlen( opts.data_dir) + strlen(opts.prefix) + 30;
		fname = (char*) calloc(sizeof(char), sizeoffn);
		if(fname == NULL)
			return 2;
		snprintf( fname, sizeoffn, "%s/%s_%d.out",opts.data_dir, opts.prefix, pid);
		fprintf(stderr, "datafile name: %s\n", fname);
		if( (fp = fopen( fname, "w")) == NULL)
			return 1;
		fprintf(fp,"time, age_group, status, number\n");
		init = 1;
	}
	for( c = 0; c < limit; c++){
		curr = t->cells + c;
		ag = tellag( curr->age);	
		switch ( curr->current_status){
			case 1:
				inf[ag]++;
				break;
			case 2:
				lsil[ag]++;
				break;
			case 3:
				hsil[ag]++;
				break;
			case 4:
				cancer[ag]++;
				break;
		}
	}
	for( c=0; c< 4; c++){
		fprintf(fp, "%d,%d,infected,%d\n",count, c, inf[c]);
		fprintf(fp, "%d,%d,lsil,%d\n",count, c, lsil[c]);
		fprintf(fp, "%d,%d,hsil,%d\n",count, c, hsil[c]);
		fprintf(fp, "%d,%d,cancer,%d\n",count, c, cancer[c]);
	}
	fflush ( fp);
	return 0;
}

int worker_fork(int pindex){

	int count;
	int ret;
	struct ca_grid  t;
	struct ca_grid  t1;
	struct ca_cell  *swap;
	int memsize;
	memsize = conf.size*conf.size*conf.size;
	t.size = conf.size;
	t1.size = conf.size;

	if((ret = cca_rng_init()))
		return (ret+30);	

	if ( ( t.cells = (struct ca_cell *) calloc( memsize, sizeof(struct ca_cell))) == NULL){
		fprintf(stderr,"Unable to allocate memory. exiting. size: %d\nstruct size: %lu\ntotal mem req: %lu",
				memsize, sizeof(struct ca_cell), memsize*sizeof(struct ca_cell));

		return (8);
	}

	if ( ( t1.cells = (struct ca_cell *) calloc( memsize, sizeof(struct ca_cell))) == NULL){
		fprintf(stderr,"Unable to allocate memory. exiting.\n");
		return (8);
	}
	populate(&t);
	print_state_stats(stderr);
	swap = t.cells;
	for( count = 0 ; count < memsize ; count ++)
		(swap + count)->num_p = 0;
	/* Now populated, rand init complete. start simulattion	*/
	for( count = 0; count <= conf.pass; count++){
		fprintf(stderr, "fork:%d\tSIM_round:%d\n",pindex,count);
		new_month();
		if( count%12 == 0)
			reset_yearly_counts(); // vaccination & screening
		simulate( &t, &t1);
		swap = t.cells;
		t.cells = t1.cells;
		t1.cells = swap;
		if( count%60 == 0){
			if( (ret = do_reporting( &t, count, pindex)) ){
				free(t.cells);
				free(t1.cells);
				return ( ret + 20);
			}
		}
		print_state_stats(stderr);
	}
	print_state_stats(stderr);
	free(t.cells);
	free(t1.cells);
	fprintf(stderr, "simualtion completed\ncount:%d\n",count);
	if( (ret = do_reporting( &t, count, pindex)) ){
		free(t.cells);
		free(t1.cells);
		return ( ret + 20);
	}
	return 0;
}
