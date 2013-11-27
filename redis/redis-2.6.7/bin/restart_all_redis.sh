#!/bin/bash

cd /usr/local/sandai/redis-2.6.7/bin

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/

for i in `seq 1 3`
do
  echo "stop redis ${i} ..."
  ./stop_redis.sh ${i}

  echo "start redis ${i} ..."
  ./start_redis.sh ${i}
  sleep 1

done





