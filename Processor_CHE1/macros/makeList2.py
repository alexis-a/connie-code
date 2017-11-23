import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
parts = ["p1", "p2", "p3", "p4"]
fileSufix = '.fits.fz'
idLabel = "runID" 
#===================================

if len(sys.argv) <= 4:

   print
   print ">>>>>>> python makeList.py <input directory 1> <input directory 2> <output directory> <Initial image ID> <Final Image ID>"
   print "<input directory 1> : first directory of raw images"
   print "<input directory 2> : second directory of raw images"
   print "<output directory> : output path to write base directory"
   print
   print "base directory : directory where all the intermiate products of a set of images are stored"
   print "Initial image ID & Final Image ID are de initial and final id number of the set of image of interest"
   print "The folder: data_<Initial image ID>_<Final Image ID> is created in the output path."
   print "This folder will contain all the intermediate processing data"
   print "Output: makes a list of images ( *.fits) from the input directories, which have ID between <Initial image ID> and <Final Image ID>."
   print "The list (listForOSI_*.dat) is saved in <output directory>/osi"
   print
   sys.exit(0)

if len(sys.argv) == 6:
	firstDir = str(sys.argv[1])
	secDir = str(sys.argv[2])
	outDir = str(sys.argv[3])
	initID = int(sys.argv[4])
	finID = int(sys.argv[5])
	if not os.path.isdir(firstDir):
		print ""
		print "First input directory does not exist. The program will exit without processing."
		print ""
		sys.exit(0)
	if not os.path.isdir(secDir):
		print
		print "Second input directory does not exist. The program will exit without processing."
		print
		sys.exit(0)
	firsFileList = os.listdir(firstDir)
	secFileList = os.listdir(secDir)
	fileTimeList1 = [(os.path.join(firstDir,file),os.stat(os.path.join(firstDir,file)).st_mtime) for file in firsFileList if file.endswith(fileSufix)] 
	fileTimeList2 = [(os.path.join(secDir,file),os.stat(os.path.join(secDir,file)).st_mtime) for file in secFileList if file.endswith(fileSufix)] 
	fileTimeList = fileTimeList1 + fileTimeList2

if len(sys.argv) == 5:
	firstDir = str(sys.argv[1])
	outDir = str(sys.argv[2])
	initID = int(sys.argv[3])
	finID = int(sys.argv[4])
	secDir = ""
	if not os.path.isdir(firstDir):
		print ""
		print "First input directory does not exist. The program will exit without processing."
		print ""
		sys.exit(0)
	firsFileList = os.listdir(firstDir)
	fileTimeList = [(os.path.join(firstDir,file),os.stat(os.path.join(firstDir,file)).st_mtime) for file in firsFileList if file.endswith(fileSufix)] 

sortedList = sorted(fileTimeList, key=lambda fileTime: fileTime[1])
fitsFileList = [row[0] for row in sortedList]

if not os.path.isdir(outDir):
	print ""
	print "Output directory does not exist. The program will exit without processing."
	print ""
	sys.exit(0)

#print summary
print "====================================="
print "List of file with ID between:"
print str(initID) + " and " + str(finID)
print "Base directory: " + firstDir
print "Aditional directory: " + secDir
print "====================================="

#Create output base directory
sufix = str(initID) + "_to_" + str(finID)
outBaseDir = os.path.join(outDir,"data_" + sufix)
outputDir = os.path.join(outBaseDir,"osi")

if os.path.isdir(outBaseDir):
	print 
	print "The base directory" + outBaseDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create base directory
	os.makedirs(outBaseDir)
	os.makedirs(outputDir)

#create output file
outFileName = []
partsFileList = []
counts = 0
for i in range(0,len(parts)):
	#get part number
	part = parts[i]
	#create output file
	outFileName.append(os.path.join(outputDir,"listForOSI_" + sufix + "_" + part + ".dat"))
	outFile = open(outFileName[i],'w')
	
	#list of files with part number
	partsList = [file for file in fitsFileList if file.endswith(part+fileSufix)]

	#cycle in all the files in the folders
	finalPartsList = []
	for file in partsList:
	 
		indexRunID = file.find(idLabel, 0, len(file))
		if indexRunID == -1:
			print "the name of the file: " + file + " is not supported by this script."
		else:
			imageID = int(file[indexRunID + 10 :indexRunID + 10 + 5])
			if imageID >= initID and imageID <= finID:
				#preguntar si tiene partes
				finalPartsList.append(file)
				outFile.write(file + "\n")
				counts = counts + 1
				print str(counts) + " " + file

	#append final list
	partsFileList.append(finalPartsList)
	#close file
	outFile.close()



#close list files
print
print "Lists of OSI images:"
print
for i in range(0,len(parts)):
	print outFileName[i] + " with " + str(len(partsFileList[i])) + " images"
print
print "Total number of OSI files: " + str(counts) + " from " + str(len(fitsFileList)) + " (*" + fileSufix + ") files"


			








