#!/usr/bin/env bash

dry="--dry-run"
if [[ "$1" == "-i" ]]; then
	dry=""
fi


git ls-files --cached --others --exclude-standard --	\
	'app/src/main/cpp/**.c' 'app/src/main/cpp/**.h'		\
	':!app/src/main/cpp/lua' ':!app/src/main/cpp/libs'	\
	':!app/src/main/cpp/lua_libs/lfs.*'					\
| xargs clang-format --verbose -i "${dry}"

if [[ "$1" != "-i" ]]; then
	echo "Dry run complete, use -i to edit files in-place."
fi
