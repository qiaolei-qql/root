#!/bin/sh
#g++ --shared -fPIC preload.c thread.cpp thread.h -o libpreload.so
#g++ --shared -fPIC preload.c -o libpreload.so
export LD_LIBRARY_PATH=".:/home/qiaolei/workspace/redis_zk_register/libs:$LD_LIBRARY_PATH" 
gcc -g test.c  -lpthread -o a.out 
export ZK_REGISTER_NODE_NAME=10.10.21.109:00
export ZK_REGISTER_PATH=/redis_group
export ZK_REGISTER_ZKTIMEOUT=3000
export ZK_REGISTER_ZKHOST_LIST=0.0.0.0:2181
echo "$LD_LIBRARY_PATH"
export LD_PRELOAD="/home/qiaolei/workspace/redis_zk_register/libs/libpreload.so" 
./a.out
#killall redis-server
#./redis-server ./redis1.conf & #>> ./redis${i}.log &
