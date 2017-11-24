import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
toolsRoot = "../tools"
siEXE = os.path.join(toolsRoot,"subtractImages-master/subtractImages.exe")
parts = ["p1", "p2", "p3", "p4"]
opts = ""
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> python computeMBS.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "It makes subraction of the Master Bias image (<base directory>/mbs/masterBias*.fits) to images listed in the list: listMB_*.dat, which is in <base directory>/mbs/"
	print "The processed images are saved <base directory>/mbs/images"
	print
	sys.exit(0)

#get base directory
baseDir = str(sys.argv[1])
if baseDir.endswith(os.sep): 
	baseDir = baseDir[0:-1]
baseName = os.path.basename(baseDir)
idInteval = baseName[5:]

#check base directory
if not os.path.isdir(baseDir):
	print
	print "<base directory> does not exist. The program will exit without processing."
	print 
	sys.exit(0)

#output directory
mbsDir = os.path.join(baseDir,"mbs")
outputDir = os.path.join(mbsDir,"images")
if os.path.isdir(outputDir):
	print 
	print "The directory " + outputDir + " should not exist."
	print "The program will exit without processing. Check procedure!"
	print
	sys.exit(0)
else:
	#create output directory
	os.makedirs(outputDir)



mbName = []
partFileName = []
for i in range(0,len(parts)):

	part = parts[i]
	#master bias filename
	mbName.append(os.path.join(mbsDir,"masterBias_" + idInteval + "_" + part + ".fits"))
	#list filename
	partFileName.append(os.path.join(mbsDir,"listMB_" + idInteval + "_" + part + ".dat"))
	#open listFile
	fileObj = fileinput.input(partFileName[i])
	#do the processing
	for file in fileObj:
		fileName = file.replace('\n','')
		outName = os.path.join(outputDir,"mbs_" + os.path.basename(fileName))
		print
		print "In: " + fileName
		print"Out: " + outName
		print
		command = siEXE + " -o " + outName + " " + opts + " " + fileName + " " + mbName[i]
		os.system(command)


print
print "Subtraction done!!"
print









