#!/bin/bash
FILEPATH=/usr/local/sandai/redis-2.6.7/bin
cd ${FILEPATH}

host=`hostname`

for (( i=1;i<=3;i++ )); do
	port="638${i}"
	echo ${port}
	num=`/bin/netstat -nlp |grep :${port} |wc -l`
	echo ${num}
	if [[ ${num} -lt 1 ]]; then
		echo " not running"
		./start_redis.sh ${i}
		title="redis server restart"
		msg="${host} redis server${i} restart"
		/usr/local/monitor-base/bin/sendEmail \
		-s mail.cc.sandai.net \
		-f hub-sys@cc.sandai.net \
		-t shub_alarm@xunlei.com \
		-cc alarm@cc.sandai.net  \
		-xu hub-sys@cc.sandai.net \
		-xp 121212 \
		-u "interface deny crawer redis " \
 		-m "$msg \nmessage \nfrom ${HOSTNAME}"
	else
			echo "running"
	fi
done
