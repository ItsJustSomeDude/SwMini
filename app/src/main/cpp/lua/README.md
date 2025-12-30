# Lua

> Lua 5.1 - Copyright (C) 1994-2012 Lua.org, PUC-Rio.
>
> MIT License
>
> https://www.lua.org/versions.html#5.1

Files in this folder are pulled from a distribution of Lua 5.1, then modified using the below steps
as well as some pointed changes. These individual pointed changes are _sometimes_ marked with a
comment, when I remembered to mark them...

Find/Replace to convert Vanilla Lua's .h files into Pointers for use with hooked SwLua:
Find:
(LUA(?:LIB)?_API[^(]+?)\(?(lua(?:open|L|I)?_[a-z]+)\)?
Replace:
$1(*$2)

Needed on:
lua.h
lauxlib.h
lualib.h - ONLY on the libs bundled in vanilla.

Find to select all the pointer definitions from header files. Use ... -> Select All Occurrences,
Then copy/paste to lua_core.c
Find:
(?<=LUA(?:LIB)?_API )[\s\S]+?;

Needed on the same header files as the pointer addition F/R.

Command to dump all lua symbols/mangled names from vanilla lib is available in lua_core.c.
