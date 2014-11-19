#!/bin/sh
cd /data/ais
. /root/.bashrc

port=$1
while [ 1 ]
do
num=`ps -ef|grep "redis-server"|grep $port|grep -v grep|wc -l`
if [ $num -lt 1 ]
then
    /usr/local/redis-2.8.7/redis-server /usr/local/redis-2.8.7/conf/redis$port.conf
    echo $(date "+%Y-%m-%d %H:%M:%S") "restat redis:$port">>/data/logs/redis$port.restart.log
fi

port2=$2
if [ "$port2" !=  "" ]
then
    num2=`ps -ef|grep "redis-server"|grep $port2|grep -v grep|wc -l`
    if [ $num2 -lt 1 ]
    then
     /usr/local/redis-2.8.7/redis-server /usr/local/redis-2.8.7/conf/redis$port2.conf
     echo $(date "+%Y-%m-%d %H:%M:%S") "restat redis:$port2">>/data/logs/redis$port2.restart.log
    fi
fi

sleep 2
done
