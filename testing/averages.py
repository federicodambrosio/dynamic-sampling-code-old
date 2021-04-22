# This code computes the averages along N and R from the test code output
import numpy as np
from math import pow,sqrt


rad = "2ar-"				#short name of the structure
end = ".txt"				#format
distr = ["b1","b2","b3"]	#short name of distributions

for dis in distr:
	data = np.loadtxt(rad+dis+end)
	
	N = [];
	R = [];
	avgR = [];
	avgErrR = [];
	pointsR = [];
	avgN = [];
	avgErrN = [];
	pointsN = [];


	for i in range(len(data)):
		r = data[i,1];
		duplicate = False;
		for rr in R:
			if rr == r:
				duplicate = True;
	
		if not duplicate:
			R.append(r)
			avgR.append(0.)
			avgErrR.append(0.)
			pointsR.append(0)
			
		n = data[i,0];
		duplicate = False;
		for nn in N:
			if nn == n:
				duplicate = True;
	
		if not duplicate:
			N.append(n)
			avgN.append(0.)
			avgErrN.append(0.)
			pointsN.append(0)

	for i in range(len(data)):
		r = data[i,1];
		idR = -1
		for j in range(len(R)):
			if r == R[j]:
				idR = j;
	
		if idR == -1:
			print("ERR")
	
		
		avgR[idR] += data[i,2];
		avgErrR[idR] += pow(data[i,3],2)
		pointsR[idR] += 1
		
		n = data[i,0];
		idN = -1
		for j in range(len(N)):
			if n == N[j]:
				idN = j;
	
		if idN == -1:
			print("ERR")
	
		
		avgN[idN] += data[i,2];
		avgErrN[idN] += pow(data[i,3],2)
		pointsN[idN] += 1


	out = open(rad+dis+"-R"+end,'w')
	for i in range(len(R)):
		avgR[i] /= float(pointsR[i])
		avgErrR[i] /= float(pointsR[i])
		out.write("{0}\t{1}\t{2}\n".format(1./R[i],avgR[i],sqrt(avgErrR[i]/pointsR[i])))
	
	out.close()	
	
	out = open(rad+dis+"-N"+end,'w')
	for i in range(len(N)):
		avgN[i] /= float(pointsN[i])
		avgErrN[i] /= float(pointsN[i])
		out.write("{0}\t{1}\t{2}\n".format(N[i],avgN[i],sqrt(avgErrN[i]/pointsN[i])))
	
	out.close()	
	


	
	
	
			