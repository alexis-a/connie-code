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
python -u $macrosDir/subtractCorrNoise.py $BASEDIR
python -u $macrosDir/mergeParts.py $BASEDIR
python -u $macrosDir/extract.py $BASEDIR


echo "Processing ended at " `date`
