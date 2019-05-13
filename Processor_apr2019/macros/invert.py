import time, os
import subprocess
import math
import fileinput
import sys
import re

#======= Configuration =============
opts = os.getenv('OPTS', '')
#opts = ""
toolsRoot = "../tools"
subtractEXE = os.path.join(toolsRoot,"subtractImages-master/subtractImages.exe")
parts = [""]
scnFilePrefix = "scn_mbs_osi_"
scnFileSufix = ".fits"
#===================================

if len(sys.argv) <= 1:
	print
	print ">>>>>>> python invert.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "Inverts the pixel values of the images (scn_mbs_osi_*.fits) in the folder <base directory>/scn"
	print "Creates a list of images that were processed (listEXT_*.dat) in <base directory>/scn/"
	print "The output images are saved in <base directory>/inv/images"
	print "Options: "
	print
	sys.exit(0)


print "========================================"
print
print "Inverting pixel values (pval -> -vpal)"
print
print "========================================"
print

#get base directory
baseDir = str(sys.argv[1])
if baseDir.endswith(os.sep): 
	baseDir = baseDir[0:-1]
baseName = os.path.basename(baseDir)
#idInteval = baseName[5:]

# 28nov2017 A.A.-A. get idInteval with reg exp
m = re.search("(\d+_to_\d+)",baseName);
idInteval = m.group(0);

#check base directory
if not os.path.isdir(baseDir):
	print
	print "<base directory> does not exist. The program will exit without processing."
	print 
	sys.exit(0)

#create output directory
invDir =  os.path.join(baseDir,"inv")
outputDir = os.path.join(invDir,"images")
if os.path.isdir(outputDir):
	print 
	print "The directory " + invDir + " should not exist. The program will exit without processing. Check procedure!"
	print 
	sys.exit(0)
else:
	#create output directory
	os.makedirs(invDir)
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
	partFileName.append(os.path.join(invDir,"listINV_" + idInteval + part + ".dat"))
	fileObj = open(partFileName[i],'w')
	for file in partsFileList[i]:
		fileObj.write(file + "\n")
	fileObj.close()

#process images
for i in range(0,len(parts)):
	part = parts[i]

        listFileName = os.path.join(invDir,"listINV_" + idInteval + part + ".dat")
        fileObj = fileinput.input(listFileName)
        isFirst = True
        for file in fileObj:
                fileName = file.replace('\n','')
		baseName = os.path.basename(fileName)
                #if isFirst:
                isFirst = False
                zeroFits = os.path.join(invDir,"zero_" + idInteval + part + ".fits")

		print "================================"
		print "creating zero fits file " + zeroFits
		print
		if not os.path.isfile(zeroFits):
			command = subtractEXE + " " + fileName + " " + fileName + " " + " -o " + zeroFits
		#print command
		os.system(command)

                outName = os.path.join(outputDir,"inv_" + baseName)
                print "================================"
                #print "In: " + fileName
                #print "Out: " + outName
                print
                if not os.path.isfile(outName):
                        command = subtractEXE + " " + zeroFits + " " + fileName + " " + " -o " + outName
			#print command
                        os.system(command)
			print "removing aux zero file " + zeroFits
			rmcmd = "rm -r " + zeroFits
			os.system(rmcmd)
                else:
                        print "Omitting, INV file already exists."
                print
                print "================================"
                print

#invFileList = os.listdir(outputDir)
#invFinalList = [file for file in invFileList if file.endswith(scnFileSufix)] 
#print
#print "Number of files (*" + invFileSufix + ") in <base directory>/inv/images is: " + str(len(invFinalList))
#print



print
print "Invertion done!!"
print


















