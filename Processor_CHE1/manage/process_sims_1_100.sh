#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/data/users/jmolina/SimFiles/Simfits/
FID=1
LID=100

# base output directory
BASE_OUT_DIR=/home/alexisaa/processed_sims/Simfits

# Processing macros location
MACROS_DIR=/share/storage1/connie/data_analysis/protools_sep2016/macros

#-----------------------------------------------------------------------------
# output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
OUT_DIR=$BASE_OUT_DIR"/data_"$SUFFIX

# Create lists
python $MACROS_DIR/makeList_sims.py $RAW_DATA_DIR $BASE_OUT_DIR $FID $LID > listMB_$SUFFIX.log 
cat listMB_$SUFFIX.log > $OUT_DIR/data_$SUFFIX.log; rm listMB_$SUFFIX.log

# doAll
cd $MACROS_DIR
nohup $MACROS_DIR/doAll.sh $OUT_DIR >> $OUT_DIR/data_$SUFFIX.log 2>&1 &
cd -
