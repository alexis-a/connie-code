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

#print hdu info
print ("%s" % hdulist1.info())
print ("%s" % hdulist2.info())

#print header before merging
print
print("Before merging:")
print()
print("Extension 0:")
print(repr(fits.getheader(file1, 0)))
print()
print("Extension 1:")
print(repr(fits.getheader(file1, 1)))
print

print
print("Before merging:")
print()
print("Extension 0:")
print(repr(fits.getheader(file2, 0)))
print()
print("Extension 1:")
print(repr(fits.getheader(file2, 1)))
print


print ("\tNumber of HDUs in input files: %d, %d" % (len(hdulist1),len(hdulist2)))

print ("Merging extension data ...")

if len(hdulist1) != len(hdulist2):
        print ("\nError: input files have different number of extensions.\nWill not continue.\n")
        sys.exit(0)

for ihdu in range (0,len(hdulist1)):
	
	if ihdu!=0:
		if hdulist1[ihdu].data.shape[1] != hdulist2[ihdu].data.shape[1]:
			print ("\nError: input images have different width.\nWill not continue.\n")
			sys.exit(0)

		mergedImg = np.concatenate([ hdulist1[ihdu].data , hdulist2[ihdu].data ])
		hdulist1[ihdu].data = mergedImg

		#hdulist1[ihdu].header.remove('DATASEC')
		#hdulist1[ihdu].header.remove('TRIMSEC')


#print hdu info after merging
print ("%s" % hdulist1.info())

#write to output file
hdulist1.writeto(fileM)

##############################################################
# edit header file of merged file
##############################################################

print 
print ("Editing header of merged output file ... ")
nRows1 = fits.getval(file1,'NAXIS2', ext=1)
nRows2 = fits.getval(file2,'NAXIS2', ext=1)
nRowsM = nRows1 + nRows2
print ("file1 NAXIS2= %s" % repr(nRows1))
print ("file2 NAXIS2= %s" % repr(nRows2))
print ("fileM NAXIS2= %s" % repr(nRowsM))


#Ext 0
fits.setval(fileM, 'TRIMSEC', value='[9:8232,1:'+str(nRowsM)+']')
fits.setval(fileM, 'DATASEC', value='[9:8232,1:'+str(nRowsM)+']')

#Ext >0
for ihdu in range (1,len(hdulist1)):

	fits.setval(fileM, 'TRIMSEC',  value='[9:8232,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'DATASEC',  value='[9:8232,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'DETSEC',   value='[1:8224,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'CCDSEC',   value='[1:8224,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'TRIMSECA', value='[4421:8532,1:'+str(nRowsM)+']',   ext=ihdu)
	fits.setval(fileM, 'DATASECA', value='[4421:8532,1:'+str(nRowsM)+']',   ext=ihdu)
	fits.setval(fileM, 'BIASSECA', value='[4271:4420,1:'+str(nRowsM)+']',   ext=ihdu)
	fits.setval(fileM, 'PRESECA',  value='[8533:8540,1:'+str(nRowsM)+']',   ext=ihdu)
	fits.setval(fileM, 'POSTSECA', value='[4421:8532,1:0]',                 ext=ihdu) #unchanged
	fits.setval(fileM, 'TRIMSECB', value='[9:4120,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'DATASECB', value='[9:4120,1:'+str(nRowsM)+']',      ext=ihdu)
	fits.setval(fileM, 'BIASSECB', value='[4121:4270,1:'+str(nRowsM)+']',   ext=ihdu)
	fits.setval(fileM, 'PRESECB',  value='[1:8,1:'+str(nRowsM)+']',         ext=ihdu) 
	fits.setval(fileM, 'POSTSECB', value='[9:4120,1:0]',                    ext=ihdu) #unchanged

print("After modifications:")
print()
print("Extension 0:")
print(repr(fits.getheader(fileM, 0)))
print()
print("Extension 1:")
print(repr(fits.getheader(fileM, 1)))



print "\tDone!"
print ("\tOutput file: \n\t%s\n" % (fileM))

