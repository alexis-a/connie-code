import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
toolsRoot = "../tools"
mbEXE = os.path.join(toolsRoot,"checkConsistencyAndComputeMedian-master/checkConsistencyAndComputeMedian.exe")
parts = ["p1", "p2", "p3", "p4"]
osiFilePrefix = "osi_"
osiFileSufix = ".fits"
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> ./computeMB.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "Creates Master Bias and MAD images (masterBias*.fits, mad*.fits) in <base directory>/mbs"
	print "To create imagees, uses the OSI images from: <base directory>/osi/osi_*.fits"
	print "Generates a list of used images (listMB_*.dat) in:<base directory>/mbs/"
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
outputDir = os.path.join(baseDir,"mbs")
if os.path.isdir(outputDir):
	print 
	print "The directory " + outputDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create output directory
	os.makedirs(outputDir)

#list of files to process sorted by creation time
osiPath = os.path.join(baseDir,"osi")
osiImagesPath = os.path.join(osiPath,"images")
fileList = os.listdir(osiImagesPath)
fileTimeList = [(os.path.join(osiImagesPath,file),os.stat(os.path.join(osiImagesPath,file)).st_mtime)
				 for file in fileList if file.startswith(osiFilePrefix) and file.endswith(osiFileSufix)]
sortedList = sorted(fileTimeList, key=lambda fileTime: fileTime[1])
fitsFileList = [row[0] for row in sortedList]


#group files by region and write into list files
partsFileList = []
partFileName = []
for i in range(0,len(parts)):

	part = parts[i]
	#list of files by part
	partSufix = part + osiFileSufix
	partsFileList.append([file for file in fitsFileList if file.endswith(partSufix)])

	#output list file
	partFileName.append(os.path.join(outputDir,"listMB_" + idInteval + "_" + part + ".dat"))
	fileObj = open(partFileName[i],'w')
	for file in partsFileList[i]:
		fileObj.write(file + "\n")
	fileObj.close()


#do the processing
mbName = []
madName = []
for i in range(0,len(parts)):

	part = parts[i]
	mbName.append(os.path.join(outputDir,"masterBias_" + idInteval + "_" + part + ".fits"))
	madName.append(os.path.join(outputDir,"mad_" + idInteval + "_" + part + ".fits"))

	#do the processing
	print
	print "Compute master bias and MAD of part " + part
	print
	mbCommand = mbEXE + " -i " + partFileName[i] + " -o " + mbName[i]
	madCommand = mbEXE + " -i " + partFileName[i] + " -o " + madName[i] + " -m"
	os.system(mbCommand)
	os.system(madCommand)
	#print "Master bias and MAD part " + part +" done!"

#print output
print
print "Number of images used on each master bias:"
for i in range(0,len(parts)):
	print mbName[i] + ": " + str(len(partsFileList[i])) + " images"
print






