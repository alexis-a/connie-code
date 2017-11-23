#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage1/connie/data/runs/025
FID=2037
LID=2097

# base output directory
BASE_OUT_DIR=/share/storage1/connie/data_analysis/processed_data/runs/025

# Processing macros location
MACROS_DIR=/share/storage1/connie/data_analysis/Processor/macros

#-----------------------------------------------------------------------------
# output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
OUT_DIR=$BASE_OUT_DIR"/data_"$SUFFIX

echo ''
echo 'Processing output directory: '
echo  $OUT_DIR
echo ''

# doEx
cd $MACROS_DIR
nohup ./doInv.sh $OUT_DIR >> $OUT_DIR/data_$SUFFIX.log.inv 2>&1 &
cd -

