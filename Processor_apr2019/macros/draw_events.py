import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
toolExe = "../tools/draw_events/draw_events.exe"
outPrefix = "draw_"
inputFilePrefix = "runID_"
inputFileSufix = ".fits.fz"
inDir = str(sys.argv[1])
simConfigFile = str(sys.argv[2])
outDir = str(sys.argv[3])
#===================================

#list of files
firstFileList = os.listdir(inDir)
fileList = [file for file in firstFileList if file.endswith(inputFileSufix)]
fileTimeList = [(os.path.join(inDir,file),os.stat(os.path.join(inDir,file)).st_mtime)
                   for file in fileList if file.startswith(inputFilePrefix) and file.endswith(inputFileSufix)]
sortedList = sorted(fileTimeList, key=lambda fileTime: fileTime[1])
fitsFileList = [row[0] for row in sortedList]

opts = ""
for file in fitsFileList:
    inFileName = os.path.join(inDir,file)
    outFileName = os.path.join(outDir,outPrefix + os.path.basename(file))
    print "================================"
    print "In: " + inFileName
    print "Out: " + outFileName
    print
    if not os.path.isfile(outFileName):
        command = toolExe + " -o " + outFileName + " -c " + simConfigFile + " -i " + inFileName  + opts
        os.system(command)
        #print command
    else:
        print "Omitting, output file already exist."
    print
    print "================================"
    print

print "Done!"


#--------------------------------------------------------------
# 07 Aug 2017 (AAA): Fixed time ordering of file list
