import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
opts = "-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 -s 12 -s 13 -s 14 -s 15 "
#opts = "-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 "
toolsRoot = "../tools"
osiEXE= os.path.join(toolsRoot,"subtractOverscan-master_connie_from_15Aug2016/subtractOverscan.exe")
parts = ["p1", "p2", "p3", "p4"]
rawFileSufix = ".fits.fz"
osiFileSufix = ".fits"
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> python computeOSI.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "It processes the images listed in the list: <base directory>/osi/listForOSI_*.dat (created by makeListMB.sh)"
	print "The processed images are saved in: <base directory>/osi/images/"
	print "Options: -s (defines the hdus to process)"
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

#check osi directory
osiDir = os.path.join(baseDir,"osi")
if not os.path.isdir(osiDir):
	print
	print "<base directory>/osi does not exist. The program will exit without processing."
	print 
	sys.exit(0)

#create output directory
outputDir = os.path.join(osiDir,"images")
if os.path.isdir(outputDir):
	print 
	print "The directory " + outputDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create output directory
	os.makedirs(outputDir)

#do processing for each part
for part in parts:
	listFileName = os.path.join(osiDir,"listForOSI_" + idInteval + "_" + part + ".dat")
	fileObj = fileinput.input(listFileName)
	for file in fileObj:
		fileName = file.replace('\n','')
		baseName = os.path.basename(fileName).replace(rawFileSufix,osiFileSufix)
		outName = os.path.join(outputDir,"osi_" + baseName)
		print "================================"
		print "In: " + fileName
		print "Out: " + outName
		print
		if not os.path.isfile(outName):
			command = osiEXE + " -o " + outName + " " + opts + " " + fileName
			os.system(command)
		else:
			print "Omitting, OSI file already exist."
		print
		print "================================"
		print

osiFileList = os.listdir(outputDir)
osiFinalList = [file for file in osiFileList if file.endswith(osiFileSufix)] 
print
print "Number of files (*" + osiFileSufix + ") in <base directory>/osi/images is: " + str(len(osiFinalList))
print















