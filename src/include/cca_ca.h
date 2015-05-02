#ifndef __CCA_CA_H__
#define __CCA_CA_H__

#define translate(p,x,y,z,size) (p+ x*size*size + y*size + z)

struct ca_cell {
	int age;
	int occupied;
	int age_of_infection;
	int current_status; /* 0->S; 1->I ; 2->l ; 3->h ; 4: cancer; 5: natural immunity	*/
	int n_immunity_age;
	int age_sil;
	int sil_stage;   /* 1: lsil 2: hsil	*/
	int activity_group;
	int immunity;
	int ever_infected;
	double num_p;

};

struct ca_grid_cubic {
	int size; //size of side. 100 = 100^3 cells
	struct ca_cell * cells;
}
#endif

