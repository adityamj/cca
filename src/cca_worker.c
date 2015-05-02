#include <cca_worker.h>
#include <transition_helper.h>
#include <cca_ca.h>
#include <cca.h>
#include <cca_groups.h>

/*	Age
	Activity group
	existing infection
*/

void infected( struct ca_cell * cell){
	cell->age_of_infection++;
	if( cell->age_of_infection == 1){


	}


}

void populate(struct ca_grid * t){
	int size;
	double rand;
	int age;
	int target_population, age_target_population, achieved;
	int index;
	size = t->size * t->size * t->size;
	struct ca_cell * curr_cell;
	target_population = size * conf.population_density;
	for( age = MIN_AGE; age <= MAX_AGE ; age++){
		age_target_poulation = target_population * conf.age_ratio[ age - MIN_AGE] ;
		achieved = 0;
		for ( achieved = 0; achieved <= age_target_population ; achieved++){
			rand = cca_get_rand();
			index = rand * size;
			curr_cell = t->cells + index;
			if( curr_cell->occupied){
				achieved--;
				continue;
			}
			curr_cell->occupied = 1;
			curr_cell->age =age * 12;
			curr_cell->activity_group = acg_oracle(curr_cell->age);
			rand = cca_get_rand();
			if ( rand <= conf.asir[ age-MIN_AGE ]){
				curr_cell->current_status = 1;	
			}
		}
	}
}

int should_infect( struct ca_grid *t, int x, int y, int z, double p){
	int xx, yy, zz;
	int ineigh=0;
	struct ca_cell * curr;
	if( x == 0)
		xx = 0 ;
	else 
		xx = x - 1;

	if( y == 0)
		yy = 0;
	else 
		yy = y -1;

	if ( z == 0)
		zz = 0;
	else
		zz = z -1;

	for( ; xx <= ( x+1) && xx < t->size; xx++)
		for(; yy <= (y+1) && yy < t->size ; yy++)
			for( ; zz <= (z+1) && zz < t->size; zz++){
				curr = translate( t->cells, xx,yy,zz, t->size);
				if( xx == x && yy == y && zz == z)
					continue;
				if( curr->occupied){
					if( chooseit( p))
						if( curr->current_status % 5 !=0)
							if( chooseit( 0.5)){
								ret = 1;
								return ret;
							}
				}
			}
	return ret;
}
int num_neigh( struct ca_grid *t, int x, int y, int z){
	int xx, yy, zz;
	int neigh=0;
	struct ca_cell * curr;
	if( x == 0)
		xx = 0 ;
	else 
		xx = x - 1;

	if( y == 0)
		yy = 0;
	else 
		yy = y -1;

	if ( z == 0)
		zz = 0;
	else
		zz = z -1;

	for( ; xx <= ( x+1) && xx < t->size; xx++)
		for(; yy <= (y+1) && yy < t->size ; yy++)
			for( ; zz <= (z+1) && zz < t->size; zz++){
				curr = translate( t->cells, xx,yy,zz, t->size);
				if( xx == x && yy == y && zz == z)
					continue;
				if( curr->occupied)
					neigh++;
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
	for( x = 0; x < t.size; x++)
		for( y = 0; y < t.size; y++)
			for ( z = 0; z< t.size ; z++){
				t_cell = translate (t->cells , x,y,z, t->size);
				t1_cell = translate ( t1->cells, x,y,z, t->size);
				memcpy ( t1_cell, t_cell, sizeof(struct ca_cell));	// Initialize our new cell.
				if( t1_cell->occupied ){
					if(should_die( t1_cell)){
						memset( t1_cell, 0, sizeof(struct ca_cell));
						death();	
						continue;
					}
					t1_cell->age++;
					if ( t1_cell->age > MAX_AGE *12){
						memset( t1_cell, 0, sizeof(struct ca_cell));
						continue;
					}
					if ( is_ag_transition( t1_cell->age)){
						t1_cell->activity_group = acg_oracle( t1_cell->age);
					}
					switch (t1_cell->current_status){
						case 0:{
							// S
							t1_cell->num_p += (double) num_part(t_cell->age, t_cell->activity_group)/(double) 12 ;
							num_p = t1_cell->num_p;
							t1_cell->num_p -= num_p;
							if ( ! num_p)
								continue;
							num_neigh = get_num_neigh( t, x,y,z);
							ret = should_infect( t,x,y,z, (double) num_p/ num_neigh);
							if(ret){
								// Infected
								t1_cell->ever_infected = 1;
								new_infected();
								t1_cell->age_of_infection = 0;
							}
							break;
						}
						case 1:{
							// Infected
							void infected( t1_cell);	
							break;
						}
						case 2:{
							// lsil
							void lsil( t1_cell);
							break;
						}
						case 3:{
							//hsil
							void hsil( t1_cell);
							break;
						}
						case 4:{
							//cancer
							break;
						}
						case 5:{
							// immune
							break;
						}
					}
					
				}
				else{
				// add a new individual to the simulation ?birth()
				}
			}	

}

int worker_fork(int pindex){

	int count;
	int ret;
	struct ca_grid  t;
	struct ca_grid  t1;
	struct ca_grid  swap;
	t.size = conf.size;
	t1.size = conf.size;
        if((ret = cca_rng_init())){
		return (ret+30);	
	
	if ( ( t.cells = (struct ca_cell *) calloc( conf.size*conf.size*conf.size, sizeof(struct ca_cell))) == NULL){
		fprintf(stderr,"Unable to allocate memory. exiting.\n");
		return (8);
	}

	if ( ( t1.cells = (struct ca_cell *) calloc( conf.size*conf.size*conf.size, sizeof(struct ca_cell))) == NULL){
		fprintf(stderr,"Unable to allocate memory. exiting.\n");
		return (8);
	}

	populate(&t);

/* Now populated, rand init complete. start simulattion	*/
	for( count = 0; count <= conf.pass; count++){
		if( count%12 == 0)
			reset_yearly_counts(); // vaccination & screening
		simulate( &t, &t1);
		swap.cells = t.cells;
		t.cells = t1.cells;
		t1.cells = swap.cells;
		if( count%60 == 0){
			    if( (ret = do_reporting( t, count, pindex)) ){
				free(t.cells);
				free(t1.cells);
				return ( ret + 20);
			    }
		}
	}
}
