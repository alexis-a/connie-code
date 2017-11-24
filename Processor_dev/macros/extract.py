import time, os
import subprocess
import math
import fileinput
import sys
import re

#======= Configuration =============
opts = ""
toolsRoot = "../tools"
extractEXE = os.path.join(toolsRoot,"extract/extract.exe")
extractConfig = os.path.join(os.getcwd(),"extractConfigFS.xml")
parts = [""]
scnFilePrefix = "scn_mbs_osi_"
scnFileSufix = ".fits"
mskFileSufix = ".fits"
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> python extract.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "Extract objests from images (scn_mbs_osi_*.fits) in the folder <base directory>/scn"
	print "Creates a list of images that were processed (listEXT_*.dat) in <base directory>/ext/"
	print "The output root catalog is saved in <base directory>/ext/catalog"
	print "Options: -m (mask image file) -c (extraction configuration file)"
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

#create output directory
extDir =  os.path.join(baseDir,"ext")
outputDir = os.path.join(extDir,"catalog")
if os.path.isdir(outputDir):
	print 
	print "The directory " + extDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create output directory
	os.makedirs(extDir)
	os.makedirs(outputDir)

#list of files to process sorted by creation time
scnPath = os.path.join(baseDir,"scn")

scnMergedPath = os.path.join(scnPath,"merged")
fileList = os.listdir(scnMergedPath)
fileTimeList = [(os.path.join(scnMergedPath,file),os.stat(os.path.join(scnMergedPath,file)).st_mtime) 
				for file in fileList if file.startswith(scnFilePrefix) and file.endswith(scnFileSufix)]

sortedList = sorted(fileTimeList, key=lambda fileTime: fileTime[1])
fitsFileList = [row[0] for row in sortedList]

#group files by region and write into list files
partsFileList = []
partFileName = []
scnDir = os.path.join(baseDir,"scn")
for i in range(0,len(parts)):

	part = parts[i]
	#list of files by part
	partSufix = part + scnFileSufix
	pattern = re.compile("\d\d" + partSufix)
	partsFileList.append([file for file in fitsFileList if pattern.search(file)])

	#output list file
	partFileName.append(os.path.join(extDir,"listEXT_" + idInteval + part + ".dat"))
	fileObj = open(partFileName[i],'w')
	for file in partsFileList[i]:
		fileObj.write(file + "\n")
	fileObj.close()

#create catalogs and list files
maskFileName = []
outFileName = []
mbsDir = os.path.join(baseDir,"mbs")
for i in range(0,len(parts)):
	part = parts[i]
	maskFileName.append(os.path.join(mbsDir,"mask6_sel_L_" + idInteval + part + mskFileSufix))
	if not os.path.isfile(maskFileName[i]):
		print
		print "The mask file " + maskFileName[i] + " does not exist."
		print "The program will exit without processing."
		print 
		sys.exit(0)

	print "Mask file: "+ maskFileName[i]

	#outfile name
	outFileName.append(os.path.join(outputDir,"catalog_" + idInteval + part + ".root"))

	#list of files
	fileObj = fileinput.input(partFileName[i])
	listToExtract = ""
	for file in fileObj:
		fileName = file.replace('\n','')
		listToExtract = listToExtract + " " + fileName

	#do the processing
	command = extractEXE + " -o " + outFileName[i] + " -m " + maskFileName[i] + " -c " + extractConfig + " " + listToExtract
	os.system(command)
	os.system("rm -f extractConfig.xml")
	

print
print "Extraction done!!"
print
