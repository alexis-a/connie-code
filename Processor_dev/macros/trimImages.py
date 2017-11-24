import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
opts = "-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 -s 12 -s 13 -s 14 -s 15 "
#opts = "-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 "
toolsRoot = "../tools"
trimEXE = os.path.join(toolsRoot,"trimSide-master/trimSide.exe")
parts = ["p1", "p2", "p3", "p4"]
mbsFilePrefix = "mbs_osi_"
mbsFileSufix = ".fits"
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> python subtractCorrNoise.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "Subtract correlated noise to images in the folder <base directory>/mbs/images/mbs_osi_"
	print "Calculates the noise from the right side of the image. Uses all the hdus."
	print "The output images have only the indicated hdus (option -s), and the processed right side."
	print "Creates a list of images that were processed (listSCN_*.dat) in <base directory>/scn/"
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

scnDir = os.path.join(baseDir,"scn")
outputDir = os.path.join(scnDir,"images")
if os.path.isdir(scnDir):
	print 
	print "The directory " + scnDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create output directory
	os.makedirs(outputDir)

#list of files to process sorted by creation time
mbsPath = os.path.join(baseDir,"mbs")
mbsImagesPath = os.path.join(mbsPath,"images")
fileList = os.listdir(mbsImagesPath)
fileTimeList = [(os.path.join(mbsImagesPath,file),os.stat(os.path.join(mbsImagesPath,file)).st_mtime)
				 for file in fileList if file.startswith(mbsFilePrefix) and file.endswith(mbsFileSufix)]
sortedList = sorted(fileTimeList, key=lambda fileTime: fileTime[1])
fitsFileList = [row[0] for row in sortedList]	

#group files by region and write into list files
partsFileList = []
partFileName = []
for i in range(0,len(parts)):

	part = parts[i]
	#list of files by part
	partSufix = part + mbsFileSufix
	partsFileList.append([file for file in fitsFileList if file.endswith(partSufix)])

	#output list file
	partFileName.append(os.path.join(scnDir,"listSCN_" + idInteval + "_" + part + ".dat"))
	fileObj = open(partFileName[i],'w')
	for file in partsFileList[i]:
		fileObj.write(file + "\n")
	fileObj.close()


#do the processing
for i in range(0,len(parts)):

	part = parts[i]

	#do the processing
	for fileName in partsFileList[i]:
		outName = os.path.join(outputDir,"scn_" + os.path.basename(fileName))
		print
		print "In: " + fileName
		print"Out: " + outName
		print
		trimCommand = trimEXE + " -l " + " -o " + outName + " " + fileName
		os.system(trimCommand)

print
print "Noise reduction done!!"
print
	



