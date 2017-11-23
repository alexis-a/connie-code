#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage1/connie/data/runs/015

SUFFIX=run015

# output directory
USER_OUT_DIR=/home/connie_ana/sims/addnu_to_data/runs/015

# Processing macros location
MACROS_DIR=/share/storage1/connie/data_analysis/Processor/macros

# Configuration file
CONFIG_FILE="$MACROS_DIR"/drawConfig/simulationConfig_run015_371_to_421.xml

echo
echo "Input dir: " $RAW_DATA_DIR
echo "Conf file: " $CONFIG_FILE
echo "output dir:" $USER_OUT_DIR
echo 

# add events
nohup python $MACROS_DIR/draw_events.py $RAW_DATA_DIR $CONFIG_FILE $USER_OUT_DIR >> $USER_OUT_DIR/draw_$SUFFIX.log 2>&1 &

