#!/bin/bash
BASEDIR=$1

if [ ! -d "$BASEDIR" ]
then
echo ""
echo "Base directory does not exit"
echo "The routine will exit without processing"
echo ""
exit
fi

echo "Processing started at " `date`

macrosDir="."
python -u $macrosDir/computeOSI.py $BASEDIR
python -u $macrosDir/computeMB.py $BASEDIR
python -u $macrosDir/computeMBS.py $BASEDIR
python -u $macrosDir/prepareMask.py $BASEDIR
python -u $macrosDir/subtractCorrNoise.py $BASEDIR
python -u $macrosDir/mergeParts.py $BASEDIR


echo "Processing ended at " `date`
