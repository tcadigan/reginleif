#ifndef VN_H_
#define VN_H_

#ifdef USE_VN

void do_vn(shiptype *);
void planet_do_vn(shiptype *, planettype *);
void vn_brain(int, int);
void vn_mad(void);

#endif

#endif /* VN_H_ */
