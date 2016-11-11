#!/bin/sh -f

makeAbsolute() {
    case $1 in
        /*)
            # already absolute, return it
            echo "$1"
            ;;
        *)
            # relative, prepend $2 made absolute
            echo `makeAbsolute "$2" "$PWD"`/"$1" | sed 's,/\.$,,'
            ;;
    esac
}

me=`which "$0"` # Search $PATH if necessary
# if it's a symbolic link, get path of destination
if test -L "$me"; then
    # Try readlink(1)
    readlink=`type readlink 2>/dev/null` || readlink=
    if test -n "$readlink"; then
        # We have readlink(1), so we can use it. Assuming GNU readlink (for -f).
        me=`readlink -nf "$me"`
    else
        # No readlink(1), so let's try ls -l
        me=`ls -l "$me" | sed 's/^.*-> //'`
        base=`dirname "$me"`
        me=`makeAbsolute "$me" "$base"`
    fi
else
    #base=`dirname "$me"`
    base=""
    me=`makeAbsolute "$me" "$base"`
fi

export BASEDIR="$(dirname "$me")"

export ADD_LD_LIBRARY_PATH="$BASEDIR"/bin

if [ "$LD_LIBRARY_PATH" != "" ]; then
    export LD_LIBRARY_PATH="$ADD_LD_LIBRARY_PATH":"$LD_LIBRARY_PATH"
else
    export LD_LIBRARY_PATH="$ADD_LD_LIBRARY_PATH"
fi

echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
output_log=QueryManager_`whoami`_`date +%Y.%m.%d-%H:%M:%S`.log
echo "LOG FILE = $output_log"
"$BASEDIR"/bin/QueryManager.exe "$@" |& tee $output_log
echo "LOG FILE = $output_log"
