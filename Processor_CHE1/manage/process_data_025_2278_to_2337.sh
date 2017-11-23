#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage1/connie/data/runs/025
FID=2278
LID=2337

# user output directory
USER_OUT_DIR=/share/storage1/connie/data_analysis/processed_data/runs/025

# Processing macros location
MACROS_DIR=/share/storage1/connie/data_analysis/Processor/macros

#-----------------------------------------------------------------------------
# base output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
BASE_OUT_DIR=$USER_OUT_DIR"/data_"$SUFFIX

# Create lists
python $MACROS_DIR/makeList.py $RAW_DATA_DIR $USER_OUT_DIR $FID $LID > listMB_$SUFFIX.log 
cat listMB_$SUFFIX.log > $BASE_OUT_DIR/data_$SUFFIX.log; rm listMB_$SUFFIX.log

echo ''
echo 'Processing output directory: '
echo  $BASE_OUT_DIR
echo ''

# doAll
cd $MACROS_DIR
nohup ./doAll.sh $BASE_OUT_DIR >> $BASE_OUT_DIR/data_$SUFFIX.log 2>&1 &
cd -

