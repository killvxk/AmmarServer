#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"

sudo ./checksec.sh --proc-all

cd "$STARTDIR"

exit 0
