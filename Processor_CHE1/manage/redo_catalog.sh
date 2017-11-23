#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage1/connie/data/runs/009
FID=238
LID=302

# user output directory
USER_OUT_DIR=/share/storage1/connie/data_analysis/processed_data/runs/009

# location of processing macros
MACROS_DIR=/share/storage1/connie/data_analysis/Processor/macros

#-----------------------------------------------------------------------------
# base output directory (already created by makeList.py)
SUFFIX=$FID"_to_"$LID
BASE_OUT_DIR=$USER_OUT_DIR"/data_"$SUFFIX

echo ''
echo 'Processing output directory: '
echo  $BASE_OUT_DIR
echo ''

# doEx
cd $MACROS_DIR
nohup ./doEx.sh $BASE_OUT_DIR >> $BASE_OUT_DIR/data_$SUFFIX.log.ext 2>&1 &
cd -

