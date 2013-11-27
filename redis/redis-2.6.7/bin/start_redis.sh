#!/bin/bash

i=$1

cd `dirname $0`

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/

echo "start redis ${i} ..."
rm ../log/redis${i}.log
./redis-server ../conf/redis2.6-${i}.conf >> ../log/redis${i}.log &


