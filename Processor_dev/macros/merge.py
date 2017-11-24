#!/usr/local/anaconda2/bin/python
import os.path  
import sys
import numpy as np
import pprint
from astropy.io import fits

# get argument list using sys module
sys.argv

# Get the total number of args passed to the demo.py
total = len(sys.argv)

if total != 4:
	print "\nUsage:"
	print ("  %s <input file 1> <input file 2> <output file>\n" % (sys.argv[0]))
	sys.exit(0)

# Get the arguments list 
file1 = str(sys.argv[1])
file2 = str(sys.argv[2])
fileM = str(sys.argv[3])

if os.path.isfile(file1)==False:
	print ("\nError: can't open %s\n") %file1
	sys.exit(0)

if os.path.isfile(file2)==False:
	print ("\nError: can't open %s\n") %file2
	sys.exit(0)

if os.path.isfile(fileM)==True:
	print ("\nError: the output file %s already exist\nWill not overwrite.\n") %fileM
	sys.exit(0)

print ("\tWill merge the following two files:\n\t%s\n\t%s" % (file1,file2))
print ("\tProcessing..")

hdulist1  = fits.open(file1)
hdulist2  = fits.open(file2)

#print ("%s" % hdulist1.info())
#print ("%s" % hdulist2.info())

print ("\tNumber of HDUs in input files: %d, %d" % (len(hdulist1),len(hdulist2)))

if len(hdulist1) != len(hdulist2):
        print ("\nError: input files have different number of extensions.\nWill not continue.\n")
        sys.exit(0)

for ihdu in range (0,len(hdulist1)):

	if hdulist1[ihdu].data.shape[1] != hdulist2[ihdu].data.shape[1]:
		print ("\nError: input images have different width.\nWill not continue.\n")
		sys.exit(0)

	mergedImg = np.concatenate([ hdulist1[ihdu].data , hdulist2[ihdu].data ])
	hdulist1[ihdu].data = mergedImg

	#hdulist1[ihdu].header.remove('DATASEC')
	#hdulist1[ihdu].header.remove('TRIMSEC')


hdulist1.writeto(fileM)
print "\tDone!"
print ("\tOutput file: \n\t%s\n" % (fileM))

