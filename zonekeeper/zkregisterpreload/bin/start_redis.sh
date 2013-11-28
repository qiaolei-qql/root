#!/bin/bash

i=$1

#CWD=$(cd `dirname $0`; pwd)
#cd ${CWD}
echo "start redis ${i} ..."

export ZK_REGISTER_NODE_NAME=10.10.21.109:00
export ZK_REGISTER_PATH=/redis_group
export ZK_REGISTER_ZKTIMEOUT=3000
export ZK_REGISTER_ZKHOST_LIST=0.0.0.0:2181
export LD_LIBRARY_PATH="../libs:$LD_LIBRARY_PATH:/usr/local/lib/"
export LD_PRELOAD="libzk_register_preload.so" 
./redis-server redis1.conf & #>> ./redis${i}.log &
unset LD_PRELOAD
