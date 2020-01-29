#!/bin/bash
EXEC_PATH=$(dirname "$0")
export LD_LIBRARY_PATH="${EXEC_PATH}/"
eval "${EXEC_PATH}/asiExe $@"
