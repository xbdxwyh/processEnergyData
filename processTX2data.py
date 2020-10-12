#coding=utf8

import os
import re
import pandas as pd
import numpy as np
#from numpy import mean

from processor import readcsv
from processor import getDir
from processor import getCSVList
from processlog import logReader
'''
here is a problem with this program, it can't handle the program in freq state,
 I don't know why, I'm too lazy to check or change it.
'''




'''
Process the information of the TX2 measurement power consumption script and 
    count the timestamps in the script.
input: Split a Bash according to the '\n' symbol to form a list
output: Return the program start timestamp and end timestamp recorded by the Bash script.
'''
def processTX2BashLog(TX2BashLogFile):
    beginTimeList = []
    endTimeList = []
    for logLine in TX2BashLogFile:
        isMatch = re.search(r"\d{10}",logLine)
        if isMatch and re.search("Loop",logLine) is not None:
            if re.search(r"begin",logLine) is not None:
                beginTimeList.append(float(isMatch.group()))
            elif re.search(r"end",logLine) is not None:
                endTimeList.append(float(isMatch.group()))
            else:
                pass
        else:
            pass
        
    return beginTimeList,endTimeList

#a = logReader('./TX2data/yolo4itvl-low/bash1599914755YOLO4-itvl.log')
#beginTimeList,endTimeList = processTX2BashLog(a)

'''
After processing the darknet framework for prediction, 
    output the time-consuming information of prediction.

Input: Split a Darknet according to the '\n' symbol to form a list
Output: List of forecast time output information.
'''
def processTX2DarknetLog(TX2DarknetLogFile):
    predictList = []
    
    for logLine in TX2DarknetLogFile:
        if re.search("Predicted",logLine) is not None:
            predictList.append(float(re.search(r"\d+\.\d+",logLine).group()))
        else:
            pass
    
    return predictList

#b = logReader('./TX2data/yolo4itvl-low/darknet1599914755YOLO4-itvl.log')
#predictList = processTX2DarknetLog(b)


#columnList = ['timeStamp','total_power(mW)','costTime(ms)']

'''
Process power consumption csv recorded by pwMonitor program.

input: TX2PowerCSVFile,pandas reads dataFrame formed by csv file
        beginTimeList, return form processTX2BashLog function
        endTimeList, anothor return from processTX2BashLog

output: totalPowerList,Calculate the energy consumed from load to end
        costTimeList, calculate Time
        
        resetPowerList,The energy consumed from the end of the calculation to the return to normal
        resetTimeList
'''
def processTX2PowerCSV(TX2PowerCSVFile,beginTimeList,endTimeList):
    index = 0
    #beginTimeList = [float(t) for t in beginTimeList]
    #endTimeList = [float(t) for t in endTimeList]
    totalPowerList = np.zeros((len(beginTimeList),1)).tolist()
    costTimeList = np.zeros((len(beginTimeList),1)).tolist()
    resetPowerList = np.zeros((len(beginTimeList),1)).tolist()
    resetTimeList = np.zeros((len(beginTimeList),1)).tolist()
    flag = 1
    for row in range(1,TX2PowerCSVFile.shape[0]):
        if flag == 1 and float(TX2PowerCSVFile.at[row,'timeStamp'])*1e-3 < beginTimeList[index]:
            pass
        elif float(TX2PowerCSVFile.at[row,'timeStamp'])*1e-3 < endTimeList[index]:
            flag = 0
            totalPowerList[index].append(float(TX2PowerCSVFile.at[row,'total_power(mW)']))
            costTimeList[index].append(float(TX2PowerCSVFile.at[row,'costTime(ms)']))
            pass
        elif flag == 0 and float(TX2PowerCSVFile.at[row,'total_power(mW)']) > 1650:
            resetPowerList[index].append(float(TX2PowerCSVFile.at[row,'total_power(mW)']))
            resetTimeList[index].append(float(TX2PowerCSVFile.at[row,'costTime(ms)']))
        elif index < len(beginTimeList)-1:
            flag = 1
            index = index + 1
            #print(index)
        else:
            break;
    
    return totalPowerList,costTimeList,resetPowerList,resetTimeList

#c = readcsv("./TX2data/yolo4itvl-low/1599915303YOLO4-itvllog.csv")
#totalPowerList,costTimeList,resetPowerList,resetTimeList = processTX2PowerCSV(c,beginTimeList,endTimeList)

'''
Calculate the energy and time of loading and calculation respectively

input:  totalPowerList,costTimeList,  return from processTX2PowerCSV function above
        predictList,                    return from processTX2DarknetLog
        threshold                       The power of TX2 at no load, the test found to be 1460

output: loadTimeList,loadEnergyList,calculateTimeList,calculateEnergyList
    A list of the energy consumed each time in the program that executes the set times, 
        the specific meaning is like variable name
'''
def getTimeEnergy(totalPowerList,costTimeList,predictList,threshold):
    #totalPowerList = totalPowerList[::-1]
    #costTimeList = costTimeList[::-1]
    
    loadTimeList = []
    loadEnergyList = []
    calculateEnergyList = []
    calculateTimeList = []
    
    for totalPowerLine,costTimeLine,predictTime in zip(totalPowerList,costTimeList,predictList):
        totalPowerLine = totalPowerLine[::-1]
        costTimeLine = costTimeLine[::-1]
        loadTime = 0
        loadEnergy = 0
        calculateEnergy = 0
        calculateTime = 0
        for power,time in zip(totalPowerLine,costTimeLine):
            if calculateTime < predictTime:
                calculateTime = calculateTime + time
                calculateEnergy = calculateEnergy + (power-threshold)*time*1e-6
            else:
                loadTime = loadTime + time
                loadEnergy = loadEnergy + (power-threshold)*time*1e-6
        loadTimeList.append(loadTime*1e-3)
        loadEnergyList.append(loadEnergy)
        calculateEnergyList.append(calculateEnergy)
        calculateTimeList.append(calculateTime*1e-3)
    
    return loadTimeList,loadEnergyList,calculateTimeList,calculateEnergyList

#aaa,bbb,ccc,ddd = getTimeEnergy(totalPowerList,costTimeList,predictList,1460)

'''
Calculate the energy consumption and time to return to normal
'''
def getResetEnergy(resetPowerList,resetTimeList,threshold):
    resetTimeCost = []
    resetEnergyCost = []
    
    for resetPowerLine,resetTimeLine in zip(resetPowerList,resetTimeList):
        resetTimeCost.append(np.sum(resetTimeLine)*1e-3)
        resetEnergyCost.append(np.sum([(x-threshold)*y for x,y in zip(resetPowerLine,resetTimeLine)])*1e-6)
    
    return resetEnergyCost,resetTimeCost

#aaaaa,bbbbb = getResetEnergy(resetPowerList,resetTimeList,1460)


'''
Too lazy to write, it's relatively simple, see for yourself
'''
def processTX2Idol(TX2IdolData):
    idolTimeList = []
    idolPowerList = []
    
    for row in range(1,TX2IdolData.shape[0]):
        idolTimeList.append(float(TX2IdolData.at[row,'costTime(ms)']))
        idolPowerList.append(float(TX2IdolData.at[row,'total_power(mW)']))
    
    return np.sum([x*y for x,y in zip(idolTimeList,idolPowerList)])/np.sum(idolTimeList)

def getMean(dataList):
    return float(np.mean(dataList))

def getCoupleMean(timeList,energyList):
    return getMean(timeList),getMean(energyList)

def getCostOut(loadTimeList,loadEnergyList,calculateTimeList,calculateEnergyList,resetEnergyCost,resetTimeCost):
    return getCoupleMean(loadTimeList,loadEnergyList),getCoupleMean(calculateTimeList,calculateEnergyList),getCoupleMean(resetTimeCost,resetEnergyCost)



if __name__ == '__main__':
    dirName='./TX2data'
    subDirName = getDir(dirName)
    
    
    
    for subDir in subDirName:
        dataDirName = dirName + "/" + subDir
        
        csvDfList = []
        bashLogList = []
        darknetLogList = []
        
        if os.path.isdir(dataDirName) and re.search("low",subDir) is not None:
            dataNameList = getDir(dataDirName)
            for dataName in dataNameList:
                if re.search("csv",dataName) is not None:
                    csvDfList.append(readcsv(dataDirName+"/"+dataName))
                    print("csv----"+dataName)
                elif re.search("bash",dataName) is not None:
                    bashLogList.append(logReader(dataDirName+"/"+dataName))
                    print("bash-----"+dataName)
                elif re.search("darknet",dataName) is not None:
                    darknetLogList.append(logReader(dataDirName+"/"+dataName))
                    print("darknet----"+dataName)
                else:
                    pass
            print("the Out is :\n")
            beginTimeList,endTimeList = processTX2BashLog(bashLogList[0])
            predictList = processTX2DarknetLog(darknetLogList[0])
            totalPowerList,costTimeList,resetPowerList,resetTimeList = processTX2PowerCSV(csvDfList[0],beginTimeList,endTimeList)
            
            loadTimeList,loadEnergyList,calculateTimeList,calculateEnergyList = getTimeEnergy(totalPowerList,costTimeList,predictList,1460)
            resetEnergyCost,resetTimeCost = getResetEnergy(resetPowerList,resetTimeList,1460)
            
            print(subDir,getCostOut(loadTimeList,loadEnergyList,calculateTimeList,calculateEnergyList,resetEnergyCost,resetTimeCost))
            
            print("end!\n")
        else:
            pass
        
            