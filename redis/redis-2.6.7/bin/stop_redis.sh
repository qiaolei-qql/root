#!/bin/bash

idx=$1

port="638"${idx}

echo "stop redis ${idx} port:${port}"

pid=`netstat -nlp | grep ${port} | awk  '{print $7}' | awk -F[/] '{print $1}'`

if [ -n "$pid" ]; then
  echo "kill -9 ${pid}"
  kill -9 ${pid}
fi


