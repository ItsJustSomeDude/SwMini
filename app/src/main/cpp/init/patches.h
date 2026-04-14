#ifndef NEEDLE_INIT_PATCHES_H
#define NEEDLE_INIT_PATCHES_H

void initP_lua_loadfile(void);
void initP_program_state(void);
void initP_lua_panic(void);

void initP_external_assets(void);

#ifdef __arm__
void initP_program_32bit(void);
#endif /* __arm__ */

/* Network patch not included right now due to crashes on physical devices. */
/* void initP_network(void); */

#endif /* NEEDLE_INIT_PATCHES_H */
