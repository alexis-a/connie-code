#!/usr/local/anaconda2/bin/python

ls1 = 60
ls2 = 60
ls3 = 60
ls4 = 61

missmatch = False

if   ls1 != ls2:
	print "\nMerge lists for p1 and p2 have not equal lengths!"
	missmatch = True
if ls1 != ls3:
	print "\nMerge lists for p1 and p3 have not equal lengths!"
	missmatch = True
if ls1 != ls4:
	print "\nMerge lists for p1 and p4 have not equal lengths!"
	missmatch = True
if ls2 != ls3:
	print "\nMerge lists for p2 and p3 have not equal lengths!"
	missmatch = True
if ls2 != ls4:
	print "\nMerge lists for p2 and p4 have not equal lengths!"
	missmatch = True
if ls3 != ls4:
	print "\nMerge lists for p3 and p4 have not equal lengths!"
	missmatch = True
if not missmatch:
	print "\nSizes of merge lists match ... [OK]\n"
else:
	print"\nError: found missmatches in merging lists. Execution aborted."
	#exit
