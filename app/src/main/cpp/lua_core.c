#include <dlfcn.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "log.h"
#include <android/log.h>

#define LOG_TAG "MiniLuaCore"

/**
 * This file defines the core fetching portion of SwMini Lua.
 * All the required functions are fetched from the compiled library with dlsym.
 * This is needed due to dynamic loading and C++ name mangling.
 */

lua_State *(*lua_newstate) (lua_Alloc f, void *ud);
void       (*lua_close) (lua_State *L);
lua_State *(*lua_newthread) (lua_State *L);
lua_CFunction (*lua_atpanic) (lua_State *L, lua_CFunction panicf);
int   (*lua_gettop) (lua_State *L);
void  (*lua_settop) (lua_State *L, int idx);
void  (*lua_pushvalue) (lua_State *L, int idx);
void  (*lua_remove) (lua_State *L, int idx);
void  (*lua_insert) (lua_State *L, int idx);
void  (*lua_replace) (lua_State *L, int idx);
int   (*lua_checkstack) (lua_State *L, int sz);
void  (*lua_xmove) (lua_State *from, lua_State *to, int n);
int             (*lua_isnumber) (lua_State *L, int idx);
int             (*lua_isstring) (lua_State *L, int idx);
int             (*lua_iscfunction) (lua_State *L, int idx);
int             (*lua_isuserdata) (lua_State *L, int idx);
int             (*lua_type) (lua_State *L, int idx);
const char     *(*lua_typename) (lua_State *L, int tp);
int            (*lua_equal) (lua_State *L, int idx1, int idx2);
int            (*lua_rawequal) (lua_State *L, int idx1, int idx2);
int            (*lua_lessthan) (lua_State *L, int idx1, int idx2);
lua_Number      (*lua_tonumber) (lua_State *L, int idx);
lua_Integer     (*lua_tointeger) (lua_State *L, int idx);
int             (*lua_toboolean) (lua_State *L, int idx);
const char     *(*lua_tolstring) (lua_State *L, int idx, size_t *len);
size_t          (*lua_objlen) (lua_State *L, int idx);
lua_CFunction   (*lua_tocfunction) (lua_State *L, int idx);
void	       *(*lua_touserdata) (lua_State *L, int idx);
lua_State      *(*lua_tothread) (lua_State *L, int idx);
const void     *(*lua_topointer) (lua_State *L, int idx);
void  (*lua_pushnil) (lua_State *L);
void  (*lua_pushnumber) (lua_State *L, lua_Number n);
void  (*lua_pushinteger) (lua_State *L, lua_Integer n);
void  (*lua_pushlstring) (lua_State *L, const char *s, size_t l);
void  (*lua_pushstring) (lua_State *L, const char *s);
const char *(*lua_pushvfstring) (lua_State *L, const char *fmt,
                                 va_list argp);
const char *(*lua_pushfstring) (lua_State *L, const char *fmt, ...);
void  (*lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
void  (*lua_pushboolean) (lua_State *L, int b);
void  (*lua_pushlightuserdata) (lua_State *L, void *p);
int   (*lua_pushthread) (lua_State *L);
void  (*lua_gettable) (lua_State *L, int idx);
void  (*lua_getfield) (lua_State *L, int idx, const char *k);
void  (*lua_rawget) (lua_State *L, int idx);
void  (*lua_rawgeti) (lua_State *L, int idx, int n);
void  (*lua_createtable) (lua_State *L, int narr, int nrec);
void *(*lua_newuserdata) (lua_State *L, size_t sz);
int   (*lua_getmetatable) (lua_State *L, int objindex);
void  (*lua_getfenv) (lua_State *L, int idx);
void  (*lua_settable) (lua_State *L, int idx);
void  (*lua_setfield) (lua_State *L, int idx, const char *k);
void  (*lua_rawset) (lua_State *L, int idx);
void  (*lua_rawseti) (lua_State *L, int idx, int n);
int   (*lua_setmetatable) (lua_State *L, int objindex);
int   (*lua_setfenv) (lua_State *L, int idx);
void  (*lua_call) (lua_State *L, int nargs, int nresults);
int   (*lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
int   (*lua_cpcall) (lua_State *L, lua_CFunction func, void *ud);
int   (*lua_load) (lua_State *L, lua_Reader reader, void *dt,
                   const char *chunkname);
int (*lua_dump) (lua_State *L, lua_Writer writer, void *data);
int  (*lua_yield) (lua_State *L, int nresults);
int  (*lua_resume) (lua_State *L, int narg);
int  (*lua_status) (lua_State *L);
int (*lua_gc) (lua_State *L, int what, int data);
int   (*lua_error) (lua_State *L);
int   (*lua_next) (lua_State *L, int idx);
void  (*lua_concat) (lua_State *L, int n);
lua_Alloc (*lua_getallocf) (lua_State *L, void **ud);
void (*lua_setallocf) (lua_State *L, lua_Alloc f, void *ud);
void (*lua_setlevel)	(lua_State *from, lua_State *to);
int (*lua_getstack) (lua_State *L, int level, lua_Debug *ar);
int (*lua_getinfo) (lua_State *L, const char *what, lua_Debug *ar);
const char *(*lua_getlocal) (lua_State *L, const lua_Debug *ar, int n);
const char *(*lua_setlocal) (lua_State *L, const lua_Debug *ar, int n);
const char *(*lua_getupvalue) (lua_State *L, int funcindex, int n);
const char *(*lua_setupvalue) (lua_State *L, int funcindex, int n);
int (*lua_sethook) (lua_State *L, lua_Hook func, int mask, int count);
lua_Hook (*lua_gethook) (lua_State *L);
int (*lua_gethookmask) (lua_State *L);
int (*lua_gethookcount) (lua_State *L);


int (*luaL_getn) (lua_State *L, int t);
void (*luaL_setn) (lua_State *L, int t, int n);
void (*luaI_openlib) (lua_State *L, const char *libname,
                      const luaL_Reg *l, int nup);
void (*luaL_register) (lua_State *L, const char *libname,
                       const luaL_Reg *l);
int (*luaL_getmetafield) (lua_State *L, int obj, const char *e);
int (*luaL_callmeta) (lua_State *L, int obj, const char *e);
int (*luaL_typerror) (lua_State *L, int narg, const char *tname);
int (*luaL_argerror) (lua_State *L, int numarg, const char *extramsg);
const char *(*luaL_checklstring) (lua_State *L, int numArg,
                                  size_t *l);
const char *(*luaL_optlstring) (lua_State *L, int numArg,
                                const char *def, size_t *l);
lua_Number (*luaL_checknumber) (lua_State *L, int numArg);
lua_Number (*luaL_optnumber) (lua_State *L, int nArg, lua_Number def);
lua_Integer (*luaL_checkinteger) (lua_State *L, int numArg);
lua_Integer (*luaL_optinteger) (lua_State *L, int nArg,
                                lua_Integer def);
void (*luaL_checkstack) (lua_State *L, int sz, const char *msg);
void (*luaL_checktype) (lua_State *L, int narg, int t);
void (*luaL_checkany) (lua_State *L, int narg);
int   (*luaL_newmetatable) (lua_State *L, const char *tname);
void *(*luaL_checkudata) (lua_State *L, int ud, const char *tname);
void (*luaL_where) (lua_State *L, int lvl);
int (*luaL_error) (lua_State *L, const char *fmt, ...);
int (*luaL_checkoption) (lua_State *L, int narg, const char *def,
                         const char *const lst[]);
int (*luaL_ref) (lua_State *L, int t);
void (*luaL_unref) (lua_State *L, int t, int ref);
int (*luaL_loadfile) (lua_State *L, const char *filename);
int (*luaL_loadbuffer) (lua_State *L, const char *buff, size_t sz,
                        const char *name);
int (*luaL_loadstring) (lua_State *L, const char *s);
lua_State *(*luaL_newstate) (void);
const char *(*luaL_gsub) (lua_State *L, const char *s, const char *p,
                          const char *r);
const char *(*luaL_findtable) (lua_State *L, int idx,
                               const char *fname, int szhint);
void (*luaL_buffinit) (lua_State *L, luaL_Buffer *B);
char *(*luaL_prepbuffer) (luaL_Buffer *B);
void (*luaL_addlstring) (luaL_Buffer *B, const char *s, size_t l);
void (*luaL_addstring) (luaL_Buffer *B, const char *s);
void (*luaL_addvalue) (luaL_Buffer *B);
void (*luaL_pushresult) (luaL_Buffer *B);


// Special - These are the bundled libs:
int (*luaopen_base) (lua_State *L);
int (*luaopen_string) (lua_State *L);


int setup_lua_core() {
    void* libsw_handle = dlopen("libswordigo.so", RTLD_NOLOAD);

#define load(name, mangled)                                   \
    name = dlsym(libsw_handle, mangled);                      \
    if(name == NULL) {                                        \
        LOGD("Failed to find symbol in lib: %s", mangled);    \
        /*return -1;*/                                        \
    }

    // Bash command to generate this load blob:
    // nm -gD libswordigo.so | grep -P '_Z..?lua.+' | while read; do sym="$(awk '{ print $3 }' <<<"$REPLY")"; var="$(c++filt -pi <<<"$sym")"; varFull="$(c++filt <<<"$sym")"; echo "load($var, \"$sym\"); // $varFull"; done
    // It's different between 32 and 64, so make sure to update both.

#if defined(__aarch64__)
    load(lua_concat, "_Z10lua_concatP9lua_Statei"); // lua_concat(lua_State*, int)
    load(lua_cpcall, "_Z10lua_cpcallP9lua_StatePFiS0_EPv"); // lua_cpcall(lua_State*, int (*)(lua_State*), void*)
    load(lua_gettop, "_Z10lua_gettopP9lua_State"); // lua_gettop(lua_State*)
    load(lua_insert, "_Z10lua_insertP9lua_Statei"); // lua_insert(lua_State*, int)
    load(luaL_error, "_Z10luaL_errorP9lua_StatePKcz"); // luaL_error(lua_State*, char const*, ...)
    load(luaL_unref, "_Z10luaL_unrefP9lua_Stateii"); // luaL_unref(lua_State*, int, int)
    load(luaL_where, "_Z10luaL_whereP9lua_Statei"); // luaL_where(lua_State*, int)
    load(lua_objlen, "_Z10lua_objlenP9lua_Statei"); // lua_objlen(lua_State*, int)
    load(lua_rawget, "_Z10lua_rawgetP9lua_Statei"); // lua_rawget(lua_State*, int)
    load(lua_rawset, "_Z10lua_rawsetP9lua_Statei"); // lua_rawset(lua_State*, int)
    load(lua_remove, "_Z10lua_removeP9lua_Statei"); // lua_remove(lua_State*, int)
    load(lua_resume, "_Z10lua_resumeP9lua_Statei"); // lua_resume(lua_State*, int)
    load(lua_settop, "_Z10lua_settopP9lua_Statei"); // lua_settop(lua_State*, int)
    load(lua_status, "_Z10lua_statusP9lua_State"); // lua_status(lua_State*)
    load(lua_atpanic, "_Z11lua_atpanicP9lua_StatePFiS0_E"); // lua_atpanic(lua_State*, int (*)(lua_State*))
    load(lua_getfenv, "_Z11lua_getfenvP9lua_Statei"); // lua_getfenv(lua_State*, int)
    load(lua_gethook, "_Z11lua_gethookP9lua_State"); // lua_gethook(lua_State*)
    load(lua_getinfo, "_Z11lua_getinfoP9lua_StatePKcP9lua_Debug"); // lua_getinfo(lua_State*, char const*, lua_Debug*)
    load(lua_pushnil, "_Z11lua_pushnilP9lua_State"); // lua_pushnil(lua_State*)
    load(lua_rawgeti, "_Z11lua_rawgetiP9lua_Stateii"); // lua_rawgeti(lua_State*, int, int)
    load(lua_rawseti, "_Z11lua_rawsetiP9lua_Stateii"); // lua_rawseti(lua_State*, int, int)
    load(lua_replace, "_Z11lua_replaceP9lua_Statei"); // lua_replace(lua_State*, int)
    load(lua_setfenv, "_Z11lua_setfenvP9lua_Statei"); // lua_setfenv(lua_State*, int)
    load(lua_sethook, "_Z11lua_sethookP9lua_StatePFvS0_P9lua_DebugEii"); // lua_sethook(lua_State*, void (*)(lua_State*, lua_Debug*), int, int)
    load(lua_getfield, "_Z12lua_getfieldP9lua_StateiPKc"); // lua_getfield(lua_State*, int, char const*)
    load(lua_getlocal, "_Z12lua_getlocalP9lua_StatePK9lua_Debugi"); // lua_getlocal(lua_State*, lua_Debug const*, int)
    load(lua_getstack, "_Z12lua_getstackP9lua_StateiP9lua_Debug"); // lua_getstack(lua_State*, int, lua_Debug*)
    load(lua_gettable, "_Z12lua_gettableP9lua_Statei"); // lua_gettable(lua_State*, int)
    load(lua_isnumber, "_Z12lua_isnumberP9lua_Statei"); // lua_isnumber(lua_State*, int)
    load(lua_isstring, "_Z12lua_isstringP9lua_Statei"); // lua_isstring(lua_State*, int)
    load(lua_lessthan, "_Z12lua_lessthanP9lua_Stateii"); // lua_lessthan(lua_State*, int, int)
    load(luaL_openlib, "_Z12luaL_openlibP9lua_StatePKcPK8luaL_Regi"); // luaL_openlib(lua_State*, char const*, luaL_Reg const*, int)
    load(lua_newstate, "_Z12lua_newstatePFPvS_S_mmES_"); // lua_newstate(void* (*)(void*, void*, unsigned long, unsigned long), void*)
    load(luaopen_base, "_Z12luaopen_baseP9lua_State"); // luaopen_base(lua_State*)
    load(lua_rawequal, "_Z12lua_rawequalP9lua_Stateii"); // lua_rawequal(lua_State*, int, int)
    load(lua_setfield, "_Z12lua_setfieldP9lua_StateiPKc"); // lua_setfield(lua_State*, int, char const*)
    load(lua_setlevel, "_Z12lua_setlevelP9lua_StateS0_"); // lua_setlevel(lua_State*, lua_State*)
    load(lua_setlocal, "_Z12lua_setlocalP9lua_StatePK9lua_Debugi"); // lua_setlocal(lua_State*, lua_Debug const*, int)
    load(lua_settable, "_Z12lua_settableP9lua_Statei"); // lua_settable(lua_State*, int)
    load(lua_tonumber, "_Z12lua_tonumberP9lua_Statei"); // lua_tonumber(lua_State*, int)
    load(lua_tothread, "_Z12lua_tothreadP9lua_Statei"); // lua_tothread(lua_State*, int)
    load(lua_typename, "_Z12lua_typenameP9lua_Statei"); // lua_typename(lua_State*, int)
    load(lua_getallocf, "_Z13lua_getallocfP9lua_StatePPv"); // lua_getallocf(lua_State*, void**)
    load(luaL_addvalue, "_Z13luaL_addvalueP11luaL_Buffer"); // luaL_addvalue(luaL_Buffer*)
    load(luaL_argerror, "_Z13luaL_argerrorP9lua_StateiPKc"); // luaL_argerror(lua_State*, int, char const*)
    load(luaL_buffinit, "_Z13luaL_buffinitP9lua_StateP11luaL_Buffer"); // luaL_buffinit(lua_State*, luaL_Buffer*)
    load(luaL_callmeta, "_Z13luaL_callmetaP9lua_StateiPKc"); // luaL_callmeta(lua_State*, int, char const*)
    load(luaL_checkany, "_Z13luaL_checkanyP9lua_Statei"); // luaL_checkany(lua_State*, int)
    load(luaL_loadfile, "_Z13luaL_loadfileP9lua_StatePKc"); // luaL_loadfile(lua_State*, char const*)
    load(luaL_newstate, "_Z13luaL_newstatev"); // luaL_newstate()
    load(luaL_register, "_Z13luaL_registerP9lua_StatePKcPK8luaL_Reg"); // luaL_register(lua_State*, char const*, luaL_Reg const*)
    load(luaL_typerror, "_Z13luaL_typerrorP9lua_StateiPKc"); // luaL_typerror(lua_State*, int, char const*)
    load(lua_newthread, "_Z13lua_newthreadP9lua_State"); // lua_newthread(lua_State*)
    load(lua_pushvalue, "_Z13lua_pushvalueP9lua_Statei"); // lua_pushvalue(lua_State*, int)
    load(lua_setallocf, "_Z13lua_setallocfP9lua_StatePFPvS1_S1_mmES1_"); // lua_setallocf(lua_State*, void* (*)(void*, void*, unsigned long, unsigned long), void*)
    load(lua_toboolean, "_Z13lua_tobooleanP9lua_Statei"); // lua_toboolean(lua_State*, int)
    load(lua_tointeger, "_Z13lua_tointegerP9lua_Statei"); // lua_tointeger(lua_State*, int)
    load(lua_tolstring, "_Z13lua_tolstringP9lua_StateiPm"); // lua_tolstring(lua_State*, int, unsigned long*)
    load(lua_topointer, "_Z13lua_topointerP9lua_Statei"); // lua_topointer(lua_State*, int)
    load(lua_checkstack, "_Z14lua_checkstackP9lua_Statei"); // lua_checkstack(lua_State*, int)
    load(lua_getupvalue, "_Z14lua_getupvalueP9lua_Stateii"); // lua_getupvalue(lua_State*, int, int)
    load(lua_isuserdata, "_Z14lua_isuserdataP9lua_Statei"); // lua_isuserdata(lua_State*, int)
    load(luaL_addstring, "_Z14luaL_addstringP11luaL_BufferPKc"); // luaL_addstring(luaL_Buffer*, char const*)
    load(luaL_checktype, "_Z14luaL_checktypeP9lua_Stateii"); // luaL_checktype(lua_State*, int, int)
    load(luaL_findtable, "_Z14luaL_findtableP9lua_StateiPKci"); // luaL_findtable(lua_State*, int, char const*, int)
    load(luaL_optnumber, "_Z14luaL_optnumberP9lua_Stateid"); // luaL_optnumber(lua_State*, int, double)
    load(luaopen_string, "_Z14luaopen_stringP9lua_State"); // luaopen_string(lua_State*)
    load(lua_pushnumber, "_Z14lua_pushnumberP9lua_Stated"); // lua_pushnumber(lua_State*, double)
    load(lua_pushstring, "_Z14lua_pushstringP9lua_StatePKc"); // lua_pushstring(lua_State*, char const*)
    load(lua_pushthread, "_Z14lua_pushthreadP9lua_State"); // lua_pushthread(lua_State*)
    load(lua_setupvalue, "_Z14lua_setupvalueP9lua_Stateii"); // lua_setupvalue(lua_State*, int, int)
    load(lua_touserdata, "_Z14lua_touserdataP9lua_Statei"); // lua_touserdata(lua_State*, int)
    load(lua_createtable, "_Z15lua_createtableP9lua_Stateii"); // lua_createtable(lua_State*, int, int)
    load(lua_gethookmask, "_Z15lua_gethookmaskP9lua_State"); // lua_gethookmask(lua_State*)
    load(lua_iscfunction, "_Z15lua_iscfunctionP9lua_Statei"); // lua_iscfunction(lua_State*, int)
    load(luaL_addlstring, "_Z15luaL_addlstringP11luaL_BufferPKcm"); // luaL_addlstring(luaL_Buffer*, char const*, unsigned long)
    load(luaL_checkstack, "_Z15luaL_checkstackP9lua_StateiPKc"); // luaL_checkstack(lua_State*, int, char const*)
    load(luaL_checkudata, "_Z15luaL_checkudataP9lua_StateiPKc"); // luaL_checkudata(lua_State*, int, char const*)
    load(luaL_loadbuffer, "_Z15luaL_loadbufferP9lua_StatePKcmS2_"); // luaL_loadbuffer(lua_State*, char const*, unsigned long, char const*)
    load(luaL_loadstring, "_Z15luaL_loadstringP9lua_StatePKc"); // luaL_loadstring(lua_State*, char const*)
    load(luaL_optinteger, "_Z15luaL_optintegerP9lua_Stateil"); // luaL_optinteger(lua_State*, int, long)
    load(luaL_optlstring, "_Z15luaL_optlstringP9lua_StateiPKcPm"); // luaL_optlstring(lua_State*, int, char const*, unsigned long*)
    load(luaL_prepbuffer, "_Z15luaL_prepbufferP11luaL_Buffer"); // luaL_prepbuffer(luaL_Buffer*)
    load(luaL_pushresult, "_Z15luaL_pushresultP11luaL_Buffer"); // luaL_pushresult(luaL_Buffer*)
    load(lua_newuserdata, "_Z15lua_newuserdataP9lua_Statem"); // lua_newuserdata(lua_State*, unsigned long)
    load(lua_pushboolean, "_Z15lua_pushbooleanP9lua_Statei"); // lua_pushboolean(lua_State*, int)
    load(lua_pushfstring, "_Z15lua_pushfstringP9lua_StatePKcz"); // lua_pushfstring(lua_State*, char const*, ...)
    load(lua_pushinteger, "_Z15lua_pushintegerP9lua_Statel"); // lua_pushinteger(lua_State*, long)
    load(lua_pushlstring, "_Z15lua_pushlstringP9lua_StatePKcm"); // lua_pushlstring(lua_State*, char const*, unsigned long)
    load(lua_tocfunction, "_Z15lua_tocfunctionP9lua_Statei"); // lua_tocfunction(lua_State*, int)
    load(lua_gethookcount, "_Z16lua_gethookcountP9lua_State"); // lua_gethookcount(lua_State*)
    load(lua_getmetatable, "_Z16lua_getmetatableP9lua_Statei"); // lua_getmetatable(lua_State*, int)
    load(luaL_checknumber, "_Z16luaL_checknumberP9lua_Statei"); // luaL_checknumber(lua_State*, int)
    load(luaL_checkoption, "_Z16luaL_checkoptionP9lua_StateiPKcPKS2_"); // luaL_checkoption(lua_State*, int, char const*, char const* const*)
    load(lua_pushcclosure, "_Z16lua_pushcclosureP9lua_StatePFiS0_Ei"); // lua_pushcclosure(lua_State*, int (*)(lua_State*), int)
    load(lua_pushvfstring, "_Z16lua_pushvfstringP9lua_StatePKcSt9__va_list"); // lua_pushvfstring(lua_State*, char const*, std::__va_list)
    load(lua_setmetatable, "_Z16lua_setmetatableP9lua_Statei"); // lua_setmetatable(lua_State*, int)
    load(luaL_checkinteger, "_Z17luaL_checkintegerP9lua_Statei"); // luaL_checkinteger(lua_State*, int)
    load(luaL_checklstring, "_Z17luaL_checklstringP9lua_StateiPm"); // luaL_checklstring(lua_State*, int, unsigned long*)
    load(luaL_getmetafield, "_Z17luaL_getmetafieldP9lua_StateiPKc"); // luaL_getmetafield(lua_State*, int, char const*)
    load(luaL_newmetatable, "_Z17luaL_newmetatableP9lua_StatePKc"); // luaL_newmetatable(lua_State*, char const*)
    load(lua_pushlightuserdata, "_Z21lua_pushlightuserdataP9lua_StatePv"); // lua_pushlightuserdata(lua_State*, void*)
    load(lua_gc, "_Z6lua_gcP9lua_Stateii"); // lua_gc(lua_State*, int, int)
    load(lua_call, "_Z8lua_callP9lua_Stateii"); // lua_call(lua_State*, int, int)
    load(lua_dump, "_Z8lua_dumpP9lua_StatePFiS0_PKvmPvES3_"); // lua_dump(lua_State*, int (*)(lua_State*, void const*, unsigned long, void*), void*)
    load(lua_load, "_Z8lua_loadP9lua_StatePFPKcS0_PvPmES3_S2_"); // lua_load(lua_State*, char const* (*)(lua_State*, void*, unsigned long*), void*, char const*)
    load(luaL_ref, "_Z8luaL_refP9lua_Statei"); // luaL_ref(lua_State*, int)
    load(lua_next, "_Z8lua_nextP9lua_Statei"); // lua_next(lua_State*, int)
    load(lua_type, "_Z8lua_typeP9lua_Statei"); // lua_type(lua_State*, int)
    load(lua_close, "_Z9lua_closeP9lua_State"); // lua_close(lua_State*)
    load(lua_equal, "_Z9lua_equalP9lua_Stateii"); // lua_equal(lua_State*, int, int)
    load(lua_error, "_Z9lua_errorP9lua_State"); // lua_error(lua_State*)
    load(luaL_gsub, "_Z9luaL_gsubP9lua_StatePKcS2_S2_"); // luaL_gsub(lua_State*, char const*, char const*, char const*)
    load(lua_pcall, "_Z9lua_pcallP9lua_Stateiii"); // lua_pcall(lua_State*, int, int, int)
    load(lua_xmove, "_Z9lua_xmoveP9lua_StateS0_i"); // lua_xmove(lua_State*, lua_State*, int)
    load(lua_yield, "_Z9lua_yieldP9lua_Statei"); // lua_yield(lua_State*, int)
#elif defined(__arm__)
    load(lua_concat, "_Z10lua_concatP9lua_Statei"); // lua_concat(lua_State*, int)
    load(lua_cpcall, "_Z10lua_cpcallP9lua_StatePFiS0_EPv"); // lua_cpcall(lua_State*, int (*)(lua_State*), void*)
    load(lua_gettop, "_Z10lua_gettopP9lua_State"); // lua_gettop(lua_State*)
    load(lua_insert, "_Z10lua_insertP9lua_Statei"); // lua_insert(lua_State*, int)
    load(luaL_error, "_Z10luaL_errorP9lua_StatePKcz"); // luaL_error(lua_State*, char const*, ...)
    load(luaL_unref, "_Z10luaL_unrefP9lua_Stateii"); // luaL_unref(lua_State*, int, int)
    load(luaL_where, "_Z10luaL_whereP9lua_Statei"); // luaL_where(lua_State*, int)
    load(lua_objlen, "_Z10lua_objlenP9lua_Statei"); // lua_objlen(lua_State*, int)
    load(lua_rawget, "_Z10lua_rawgetP9lua_Statei"); // lua_rawget(lua_State*, int)
    load(lua_rawset, "_Z10lua_rawsetP9lua_Statei"); // lua_rawset(lua_State*, int)
    load(lua_remove, "_Z10lua_removeP9lua_Statei"); // lua_remove(lua_State*, int)
    load(lua_resume, "_Z10lua_resumeP9lua_Statei"); // lua_resume(lua_State*, int)
    load(lua_settop, "_Z10lua_settopP9lua_Statei"); // lua_settop(lua_State*, int)
    load(lua_status, "_Z10lua_statusP9lua_State"); // lua_status(lua_State*)
    load(lua_atpanic, "_Z11lua_atpanicP9lua_StatePFiS0_E"); // lua_atpanic(lua_State*, int (*)(lua_State*))
    load(lua_getfenv, "_Z11lua_getfenvP9lua_Statei"); // lua_getfenv(lua_State*, int)
    load(lua_gethook, "_Z11lua_gethookP9lua_State"); // lua_gethook(lua_State*)
    load(lua_getinfo, "_Z11lua_getinfoP9lua_StatePKcP9lua_Debug"); // lua_getinfo(lua_State*, char const*, lua_Debug*)
    load(lua_pushnil, "_Z11lua_pushnilP9lua_State"); // lua_pushnil(lua_State*)
    load(lua_rawgeti, "_Z11lua_rawgetiP9lua_Stateii"); // lua_rawgeti(lua_State*, int, int)
    load(lua_rawseti, "_Z11lua_rawsetiP9lua_Stateii"); // lua_rawseti(lua_State*, int, int)
    load(lua_replace, "_Z11lua_replaceP9lua_Statei"); // lua_replace(lua_State*, int)
    load(lua_setfenv, "_Z11lua_setfenvP9lua_Statei"); // lua_setfenv(lua_State*, int)
    load(lua_sethook, "_Z11lua_sethookP9lua_StatePFvS0_P9lua_DebugEii"); // lua_sethook(lua_State*, void (*)(lua_State*, lua_Debug*), int, int)
    load(lua_getfield, "_Z12lua_getfieldP9lua_StateiPKc"); // lua_getfield(lua_State*, int, char const*)
    load(lua_getlocal, "_Z12lua_getlocalP9lua_StatePK9lua_Debugi"); // lua_getlocal(lua_State*, lua_Debug const*, int)
    load(lua_getstack, "_Z12lua_getstackP9lua_StateiP9lua_Debug"); // lua_getstack(lua_State*, int, lua_Debug*)
    load(lua_gettable, "_Z12lua_gettableP9lua_Statei"); // lua_gettable(lua_State*, int)
    load(lua_isnumber, "_Z12lua_isnumberP9lua_Statei"); // lua_isnumber(lua_State*, int)
    load(lua_isstring, "_Z12lua_isstringP9lua_Statei"); // lua_isstring(lua_State*, int)
    load(lua_lessthan, "_Z12lua_lessthanP9lua_Stateii"); // lua_lessthan(lua_State*, int, int)
    load(luaL_openlib, "_Z12luaL_openlibP9lua_StatePKcPK8luaL_Regi"); // luaL_openlib(lua_State*, char const*, luaL_Reg const*, int)
    load(lua_newstate, "_Z12lua_newstatePFPvS_S_jjES_"); // lua_newstate(void* (*)(void*, void*, unsigned int, unsigned int), void*)
    load(luaopen_base, "_Z12luaopen_baseP9lua_State"); // luaopen_base(lua_State*)
    load(lua_rawequal, "_Z12lua_rawequalP9lua_Stateii"); // lua_rawequal(lua_State*, int, int)
    load(lua_setfield, "_Z12lua_setfieldP9lua_StateiPKc"); // lua_setfield(lua_State*, int, char const*)
    load(lua_setlevel, "_Z12lua_setlevelP9lua_StateS0_"); // lua_setlevel(lua_State*, lua_State*)
    load(lua_setlocal, "_Z12lua_setlocalP9lua_StatePK9lua_Debugi"); // lua_setlocal(lua_State*, lua_Debug const*, int)
    load(lua_settable, "_Z12lua_settableP9lua_Statei"); // lua_settable(lua_State*, int)
    load(lua_tonumber, "_Z12lua_tonumberP9lua_Statei"); // lua_tonumber(lua_State*, int)
    load(lua_tothread, "_Z12lua_tothreadP9lua_Statei"); // lua_tothread(lua_State*, int)
    load(lua_typename, "_Z12lua_typenameP9lua_Statei"); // lua_typename(lua_State*, int)
    load(lua_getallocf, "_Z13lua_getallocfP9lua_StatePPv"); // lua_getallocf(lua_State*, void**)
    load(luaL_addvalue, "_Z13luaL_addvalueP11luaL_Buffer"); // luaL_addvalue(luaL_Buffer*)
    load(luaL_argerror, "_Z13luaL_argerrorP9lua_StateiPKc"); // luaL_argerror(lua_State*, int, char const*)
    load(luaL_buffinit, "_Z13luaL_buffinitP9lua_StateP11luaL_Buffer"); // luaL_buffinit(lua_State*, luaL_Buffer*)
    load(luaL_callmeta, "_Z13luaL_callmetaP9lua_StateiPKc"); // luaL_callmeta(lua_State*, int, char const*)
    load(luaL_checkany, "_Z13luaL_checkanyP9lua_Statei"); // luaL_checkany(lua_State*, int)
    load(luaL_loadfile, "_Z13luaL_loadfileP9lua_StatePKc"); // luaL_loadfile(lua_State*, char const*)
    load(luaL_newstate, "_Z13luaL_newstatev"); // luaL_newstate()
    load(luaL_register, "_Z13luaL_registerP9lua_StatePKcPK8luaL_Reg"); // luaL_register(lua_State*, char const*, luaL_Reg const*)
    load(luaL_typerror, "_Z13luaL_typerrorP9lua_StateiPKc"); // luaL_typerror(lua_State*, int, char const*)
    load(lua_newthread, "_Z13lua_newthreadP9lua_State"); // lua_newthread(lua_State*)
    load(lua_pushvalue, "_Z13lua_pushvalueP9lua_Statei"); // lua_pushvalue(lua_State*, int)
    load(lua_setallocf, "_Z13lua_setallocfP9lua_StatePFPvS1_S1_jjES1_"); // lua_setallocf(lua_State*, void* (*)(void*, void*, unsigned int, unsigned int), void*)
    load(lua_toboolean, "_Z13lua_tobooleanP9lua_Statei"); // lua_toboolean(lua_State*, int)
    load(lua_tointeger, "_Z13lua_tointegerP9lua_Statei"); // lua_tointeger(lua_State*, int)
    load(lua_tolstring, "_Z13lua_tolstringP9lua_StateiPj"); // lua_tolstring(lua_State*, int, unsigned int*)
    load(lua_topointer, "_Z13lua_topointerP9lua_Statei"); // lua_topointer(lua_State*, int)
    load(lua_checkstack, "_Z14lua_checkstackP9lua_Statei"); // lua_checkstack(lua_State*, int)
    load(lua_getupvalue, "_Z14lua_getupvalueP9lua_Stateii"); // lua_getupvalue(lua_State*, int, int)
    load(lua_isuserdata, "_Z14lua_isuserdataP9lua_Statei"); // lua_isuserdata(lua_State*, int)
    load(luaL_addstring, "_Z14luaL_addstringP11luaL_BufferPKc"); // luaL_addstring(luaL_Buffer*, char const*)
    load(luaL_checktype, "_Z14luaL_checktypeP9lua_Stateii"); // luaL_checktype(lua_State*, int, int)
    load(luaL_findtable, "_Z14luaL_findtableP9lua_StateiPKci"); // luaL_findtable(lua_State*, int, char const*, int)
    load(luaL_optnumber, "_Z14luaL_optnumberP9lua_Stateid"); // luaL_optnumber(lua_State*, int, double)
    load(luaopen_string, "_Z14luaopen_stringP9lua_State"); // luaopen_string(lua_State*)
    load(lua_pushnumber, "_Z14lua_pushnumberP9lua_Stated"); // lua_pushnumber(lua_State*, double)
    load(lua_pushstring, "_Z14lua_pushstringP9lua_StatePKc"); // lua_pushstring(lua_State*, char const*)
    load(lua_pushthread, "_Z14lua_pushthreadP9lua_State"); // lua_pushthread(lua_State*)
    load(lua_setupvalue, "_Z14lua_setupvalueP9lua_Stateii"); // lua_setupvalue(lua_State*, int, int)
    load(lua_touserdata, "_Z14lua_touserdataP9lua_Statei"); // lua_touserdata(lua_State*, int)
    load(lua_createtable, "_Z15lua_createtableP9lua_Stateii"); // lua_createtable(lua_State*, int, int)
    load(lua_gethookmask, "_Z15lua_gethookmaskP9lua_State"); // lua_gethookmask(lua_State*)
    load(lua_iscfunction, "_Z15lua_iscfunctionP9lua_Statei"); // lua_iscfunction(lua_State*, int)
    load(luaL_addlstring, "_Z15luaL_addlstringP11luaL_BufferPKcj"); // luaL_addlstring(luaL_Buffer*, char const*, unsigned int)
    load(luaL_checkstack, "_Z15luaL_checkstackP9lua_StateiPKc"); // luaL_checkstack(lua_State*, int, char const*)
    load(luaL_checkudata, "_Z15luaL_checkudataP9lua_StateiPKc"); // luaL_checkudata(lua_State*, int, char const*)
    load(luaL_loadbuffer, "_Z15luaL_loadbufferP9lua_StatePKcjS2_"); // luaL_loadbuffer(lua_State*, char const*, unsigned int, char const*)
    load(luaL_loadstring, "_Z15luaL_loadstringP9lua_StatePKc"); // luaL_loadstring(lua_State*, char const*)
    load(luaL_optinteger, "_Z15luaL_optintegerP9lua_Stateii"); // luaL_optinteger(lua_State*, int, int)
    load(luaL_optlstring, "_Z15luaL_optlstringP9lua_StateiPKcPj"); // luaL_optlstring(lua_State*, int, char const*, unsigned int*)
    load(luaL_prepbuffer, "_Z15luaL_prepbufferP11luaL_Buffer"); // luaL_prepbuffer(luaL_Buffer*)
    load(luaL_pushresult, "_Z15luaL_pushresultP11luaL_Buffer"); // luaL_pushresult(luaL_Buffer*)
    load(lua_newuserdata, "_Z15lua_newuserdataP9lua_Statej"); // lua_newuserdata(lua_State*, unsigned int)
    load(lua_pushboolean, "_Z15lua_pushbooleanP9lua_Statei"); // lua_pushboolean(lua_State*, int)
    load(lua_pushfstring, "_Z15lua_pushfstringP9lua_StatePKcz"); // lua_pushfstring(lua_State*, char const*, ...)
    load(lua_pushinteger, "_Z15lua_pushintegerP9lua_Statei"); // lua_pushinteger(lua_State*, int)
    load(lua_pushlstring, "_Z15lua_pushlstringP9lua_StatePKcj"); // lua_pushlstring(lua_State*, char const*, unsigned int)
    load(lua_tocfunction, "_Z15lua_tocfunctionP9lua_Statei"); // lua_tocfunction(lua_State*, int)
    load(lua_gethookcount, "_Z16lua_gethookcountP9lua_State"); // lua_gethookcount(lua_State*)
    load(lua_getmetatable, "_Z16lua_getmetatableP9lua_Statei"); // lua_getmetatable(lua_State*, int)
    load(luaL_checknumber, "_Z16luaL_checknumberP9lua_Statei"); // luaL_checknumber(lua_State*, int)
    load(luaL_checkoption, "_Z16luaL_checkoptionP9lua_StateiPKcPKS2_"); // luaL_checkoption(lua_State*, int, char const*, char const* const*)
    load(lua_pushcclosure, "_Z16lua_pushcclosureP9lua_StatePFiS0_Ei"); // lua_pushcclosure(lua_State*, int (*)(lua_State*), int)
    load(lua_pushvfstring, "_Z16lua_pushvfstringP9lua_StatePKcSt9__va_list"); // lua_pushvfstring(lua_State*, char const*, std::__va_list)
    load(lua_setmetatable, "_Z16lua_setmetatableP9lua_Statei"); // lua_setmetatable(lua_State*, int)
    load(luaL_checkinteger, "_Z17luaL_checkintegerP9lua_Statei"); // luaL_checkinteger(lua_State*, int)
    load(luaL_checklstring, "_Z17luaL_checklstringP9lua_StateiPj"); // luaL_checklstring(lua_State*, int, unsigned int*)
    load(luaL_getmetafield, "_Z17luaL_getmetafieldP9lua_StateiPKc"); // luaL_getmetafield(lua_State*, int, char const*)
    load(luaL_newmetatable, "_Z17luaL_newmetatableP9lua_StatePKc"); // luaL_newmetatable(lua_State*, char const*)
    load(lua_pushlightuserdata, "_Z21lua_pushlightuserdataP9lua_StatePv"); // lua_pushlightuserdata(lua_State*, void*)
    load(lua_gc, "_Z6lua_gcP9lua_Stateii"); // lua_gc(lua_State*, int, int)
    load(lua_call, "_Z8lua_callP9lua_Stateii"); // lua_call(lua_State*, int, int)
    load(lua_dump, "_Z8lua_dumpP9lua_StatePFiS0_PKvjPvES3_"); // lua_dump(lua_State*, int (*)(lua_State*, void const*, unsigned int, void*), void*)
    load(lua_load, "_Z8lua_loadP9lua_StatePFPKcS0_PvPjES3_S2_"); // lua_load(lua_State*, char const* (*)(lua_State*, void*, unsigned int*), void*, char const*)
    load(luaL_ref, "_Z8luaL_refP9lua_Statei"); // luaL_ref(lua_State*, int)
    load(lua_next, "_Z8lua_nextP9lua_Statei"); // lua_next(lua_State*, int)
    load(lua_type, "_Z8lua_typeP9lua_Statei"); // lua_type(lua_State*, int)
    load(lua_close, "_Z9lua_closeP9lua_State"); // lua_close(lua_State*)
    load(lua_equal, "_Z9lua_equalP9lua_Stateii"); // lua_equal(lua_State*, int, int)
    load(lua_error, "_Z9lua_errorP9lua_State"); // lua_error(lua_State*)
    load(luaL_gsub, "_Z9luaL_gsubP9lua_StatePKcS2_S2_"); // luaL_gsub(lua_State*, char const*, char const*, char const*)
    load(lua_pcall, "_Z9lua_pcallP9lua_Stateiii"); // lua_pcall(lua_State*, int, int, int)
    load(lua_xmove, "_Z9lua_xmoveP9lua_StateS0_i"); // lua_xmove(lua_State*, lua_State*, int)
    load(lua_yield, "_Z9lua_yieldP9lua_Statei"); // lua_yield(lua_State*, int)
#endif

    return 0;
}