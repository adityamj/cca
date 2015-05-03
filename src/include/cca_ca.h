#ifndef __CCA_CA_H__
#define __CCA_CA_H__

#define translate(p,x,y,z,size) (p+ x*size*size + y*size + z)
#include<stdint.h>

struct ca_cell {
	int age;
	int occupied;
	int age_of_infection;
	int current_status; /* 0->S; 1->I ; 2->l ; 3->h ; 4: cancer; 5: natural immunity	*/
	int activity_group;
	int ever_infected;
	double num_p;
};

struct ca_grid {
	int size; //size of side. 100 = 100^3 cells
	struct ca_cell * cells;
};
#endif

