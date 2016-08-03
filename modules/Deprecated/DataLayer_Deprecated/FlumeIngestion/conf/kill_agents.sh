#!/bin/bash

PIDS=`netstat -tlnp | awk '/:616 */ {split($NF,a,"/"); print a[1]}'`

for PID in $PIDS
do
	echo "Killing process: " ${PID}
	`kill -9 ${PID}`
done

