

Find/Replace to convert Vanilla Lua's .h files into Pointers for use with hooked SwLua:
Find:
(LUA(?:LIB)?_API[^(]+?)\(?(lua(?:open|L|I)?_[a-z]+)\)?
Replace:
$1(*$2)

Needed on:
lua.h
lauxlib.h
lualib.h - ONLY on the libs bundled in libswordigo.


Find to select all the pointer definitions from header files. Use ... -> Select All Occurrences,
Then copy/paste to lua_core.c
Find:
(?<=LUA(?:LIB)?_API )[\s\S]+?;

Needed on the same header files as the pointer addition F/R.






Command to dump all lua symbols/mangled names from libswordigo:

nm -gD libswordigo.so
    | grep -P '_Z..?lua.+?(?=P)'
    | while read; do
        sym="$(awk '{ print $3 }' <<<"$REPLY")";
        var="$(c++filt -pi <<<"$sym")";
        varFull="$(c++filt <<<"$sym")";
        echo "$var = dlsym(libsw_handle, \"$sym\"); // $varFull";
    done
