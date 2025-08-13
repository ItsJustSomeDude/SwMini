
#ifndef SWMINI_HOOK_CORE_H
#define SWMINI_HOOK_CORE_H

//void* get_library_base_address(const char*);
void *hookSym(const char *symName, void *newAddr, void **origAddr);
void *hookAddr(void* func_addr, void *new_addr, void **orig_addr);
void *hookOffset(unsigned long offset, void *new_addr, void **orig_addr);
void *getSym(const char* symbol);
void *getOffset(unsigned int offset);

#endif //SWMINI_HOOK_CORE_H
