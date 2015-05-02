#ifndef __CCA_STATE_H__
#define __CCA_STATE_H__
struct cca_state{
	int population;	/* Current Population	*/
	int hsil_total;	/* Total HSIL seen so far */
	int hsil_to_lsil;/* HSIL reverting back to normal. Ideally, hsil_to_lsil / hsil_total should be 0.5 */
	int lsil_to_normal;	/* */
	int lsil_total;	/* lsil_to_normal/lsil_total ~ 0.9 */
	int total_cancer ;	/* Total cancers seen	*/
	int total_infected;
	int infected_to_normal;
	int yearly_vaccinated;
	int yearly_screened;
};

#define CCA_STATE_INIT  { 0,0,0,0,0,0,0,0,0,0}

#endif
