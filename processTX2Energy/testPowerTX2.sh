#!/bin/bash

#####The following are variables that need to be modified according to your own situation#################

#Set Sleep Time,Sleep once in the begin and end of one Loop 
SLEEPTIME=10

#The Num ber of times you want to execute
SEQNUM=20

#Selece working Mode,itvl is calculating instantaneous power,freq calculating average power
WORKTYPE=itvl

#The Type of algorithm you choose
MODELNAME=yolov3

######################################Variable end################################

./powertest/pwMonitor $WORKTYPE & 

#echo "test success!"

sleep 20

MODEL=${MODELNAME}"-"${WORKTYPE}

RECORD="../../darknet"$(date +%s)${MODEL}".log"
LOG="../../bash"$(date +%s)${MODEL}".log"
#starttime=`date +'%Y-%m-%d %H:%M:%S:%N'`
#start_second=$(date --date="$starttime" +%s.%N)
start_second=$(date +%s)

cd ./yolov4/darknet/

for i in $(seq $SEQNUM);do
    sleep ${SLEEPTIME}
    echo "Loop "$i" Begin!" >> $LOG
    echo "test in loop success"
    #starttime1=`date +'%Y-%m-%d %H:%M:%S:%N'`
    #start_second1=$(date --date="$starttime1" +%s.%N)
    start_second1=$(date +%s)
    #add Code you want to test
    #./yolov4/darknet/darknet detect ./yolov4/darknet/cfg/yolov3.cfg ./yolov4/darknet/yolov3.weights ./yolov4/darknet/dog.jpg >> $RECORD
    ./darknet detect cfg/${MODELNAME}".cfg" yolov3.weights data/dog.jpg >> $RECORD
    #end your code
    #endtime1=`date +'%Y-%m-%d %H:%M:%S:%N'`
    #end_second1=$(date --date="$endtime1" +%s.%N)
    end_second1=$(date +%s)
    echo "Loop "$i" begin in "$start_second1 >> $LOG
    echo "Loop "$i" end in "$end_second1 >> $LOG
    echo "Loop "$i" done" >> $LOG
    sleep ${SLEEPTIME}
done

var=$(ps -ef | grep pwMonitor)
arr=(${var//","/})
echo $arr
kill -9 ${arr[1]}

#endtime=`date +'%Y-%m-%d %H:%M:%S:%N'`
#end_second=$(date --date="$endtime" +%s.%N)
#echo $(($end_second*1000-$start_second*1000))
end_second=$(date +%s)
echo "this bash start in "$start_second >> $LOG
echo "this bash done in "$end_second >> $LOG

mv "../../log.csv" "../../"$(date +%s)${MODEL}"log.csv"

