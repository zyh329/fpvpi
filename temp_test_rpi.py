#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import pylab
import numpy as np
import time as tm
from MS5611 import MS5611

__AUTHOR__ = "Gabriel Mariano Marcelino"
__DATE__ = "09/09/2015"

def getCPUtemperature():
	res = os.popen(‘vcgencmd measure_temp’).readline()
	return int(float(res.replace(“temp=”,””).replace(“‘C\n”,””)))

def getBarTemperature(baro):
	baro.refreshTemperature()
	tm.sleep(0.01) # Waiting for temperature data ready 10ms
	baro.readTemperature()
	baro.calculatePressureAndTemperature()
	return baro.TEMP

baro = MS5611()
baro.initialize()

time = []
cpu_temp = []
bar_temp = []

for i in xrange(0,44):
	cpu_temp.append(getCPUtemperature())
	bar_temp.append(getBarTemperature(baro))
	minute.append(i)
	tm.sleep(15)
	

pylab.plot(time, cpu_temp, "-b", label='CPU temperature')
pylab.plot(time, bar_temp, "-r", label='Barometer temperature')

pylab.title('Tempo x Temperatura (Tampa fechada)')
pylab.xlabel('Tempo [s]')
pylab.ylabel(u'Temperatura [\u00B0C]')
pylab.legend(loc='best')
pylab.axis([0,600,0,80])
pylab.savefig('temp_test.pdf', bbox_inches='tight', dpi=600, transparent=True)
