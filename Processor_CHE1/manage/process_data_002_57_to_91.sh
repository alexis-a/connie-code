#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage1/connie/data/2016/scienceTest/15Aug2016/002
FID=57
LID=91

# base output directory
BASE_OUT_DIR=/share/storage1/connie/data_analysis/processed_data/2016/scienceTest/15Aug2016/002

# Processing macros location
MACROS_DIR=/share/storage1/connie/data_analysis/Processor/macros

#-----------------------------------------------------------------------------
# output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
OUT_DIR=$BASE_OUT_DIR"/data_"$SUFFIX

# Create lists
python $MACROS_DIR/makeList2.py $RAW_DATA_DIR $BASE_OUT_DIR $FID $LID > listMB_$SUFFIX.log 
cat listMB_$SUFFIX.log > $OUT_DIR/data_$SUFFIX.log; rm listMB_$SUFFIX.log

# doAll
cd $MACROS_DIR
nohup ./doAll.sh $OUT_DIR >> $OUT_DIR/data_$SUFFIX.log 2>&1 &
cd -
