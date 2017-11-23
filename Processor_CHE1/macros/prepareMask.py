import time, os
import subprocess
import math
import fileinput
import sys

#======= Configuration =============
mad_cut = 6
opts = ""
toolsRoot = "../tools"
trimEXE = os.path.join(toolsRoot,"trimSide-master/trimSide.exe")
makeMask = os.path.join(toolsRoot,"makeMask-master/makeMask.exe")
parts = ["p1", "p2", "p3", "p4"]
#===================================
if len(sys.argv) <= 1:
	print
	print ">>>>>>> python prepareMask.py <base directory>"
	print "<base directory> : directory where all the intermiate products of a set of images are stored"
	print "Produces a mask of bad pixels using the MAD image (mad*.fits) in <base directory>/mbs/"
	print "The output mask image is saved in <base directory>/mbs/"
	print "Option: -c (MAD CUT)"
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

#check output directory
outputDir = os.path.join(baseDir,"mbs")
if not os.path.isdir(baseDir):
	print
	print "The directory <base directory>/mbs does not exist. The program will exit without processing."
	print 
	sys.exit(0)


#check mad files
madName = []
madTrimName = []
for i in range(0,len(parts)):
	part = parts[i]
	madName.append(os.path.join(outputDir,"mad_" + idInteval + "_" + part + ".fits"))
	madTrimName.append(os.path.join(outputDir,"trim_mad_" + idInteval + "_" + part + ".fits"))
	if not os.path.isfile(madName[i]):
		print
		print "The mad file " + madName[i] + " does not exist."
		print "The program will exit without processing."
		print 
		sys.exit(0)


#do the processing
maskFileName = []
for i in range(0,len(parts)):
	part = parts[i]
	maskFileName.append(os.path.join(outputDir,"mask" + str(mad_cut) + "_sel_L_" + idInteval + "_" + part + ".fits"))

	trimCommand = trimEXE + " -l " + " -o " + madTrimName[i] + " " + madName[i]
	print trimCommand
	maskCommand = makeMask + " -c " + str(mad_cut) + " -o " + maskFileName[i] + " " + madTrimName[i]
	os.system(trimCommand)
	os.system(maskCommand)

print
print "Mask done!!"
print








