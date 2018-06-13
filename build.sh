#!/bin/bash
set -euo pipefail
shopt -s failglob
shopt -s globstar
# IFS=$'\n\t'

SKSE=sksevr_2_00_10/src
SUBFOLDER=sksevr
SRC=src
WD=active
PROJECT=storage_util

function usage() {
	echo '$0 applypatch | genpatch'
}

# https://stackoverflow.com/a/25394801/250076
# We still need this.
windows() { [[ -n "$WINDIR" ]]; }

# Cross-platform symlink function. With one parameter, it will check
# whether the parameter is a symlink. With two parameters, it will create
# a symlink to a file or directory, with syntax: link $linkname $target
link() {
    if [[ -z "$2" ]]; then
        # Link-checking mode.
        if windows; then
            fsutil reparsepoint query "$1" > /dev/null
        else
            [[ -h "$1" ]]
        fi
    else
        # Link-creation mode.
        if windows; then
            # Windows needs to be told if it's a directory or not. Infer that.
            # Also: note that we convert `/` to `\`. In this case it's necessary.
			# path must be relative to folder containing the link (or we could make it absolute?)
			d=$(dirname "$1")
			tg=$(realpath --relative-to="$d" "$2")
            if [[ -d "$2" ]]; then
                cmd <<< "mklink /D \"$1\" \"${tg//\//\\}\""
            else
                cmd <<< "mklink \"$1\" \"${tg//\//\\}\""
            fi
        else
            # You know what? I think ln's parameters are backwards.
            ln -s "$2" "$1"
        fi
    fi
}

# if [[ $# = 0 ]]; then
	# usage()
# fi

if [[ "$#" -eq 0 ]]; then
	echo
fi

if [[ "$1" = "applypatch" ]]; then
	if [[ -d "$SKSE/sksevr" ]]; then
		mv "$SKSE/sksevr" "$SKSE/skse64"
	fi
	if [[ -d "$WD" ]]; then
		rm -r "$WD"
	fi
	cp -r "$SKSE" "$WD"
	link "$WD/skse64/$PROJECT" $(readlink -m "src")
	git apply -p0 patches/**/*.diff
elif [[ "$1" = "genpatch" ]]; then
	if [ -d patches ]; then
		rm -r patches
	fi
	mkdir patches
	for f in $SKSE/**/*.{cpp,h,sln,vcxproj}; do # .{c,cpp,h,hpp,csproj}
		f=${f#$SKSE}
		diff --strip-trailing-cr --brief "$SKSE""$f" "$WD""$f" && RC=$? || RC=$?
		if [[ "$RC" -eq "1" ]]; then
			out=patches/"$f.diff"
			if [[ ! -d $(dirname "$out") ]]; then
				mkdir -p $(dirname "$out")
			fi
			(diff -u "$SKSE""$f" --label "$SKSE""$f" "$WD""$f" --label "$WD""$f" > "$out") || true
		fi
	done
fi

