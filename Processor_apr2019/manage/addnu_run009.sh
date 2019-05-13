#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage2/connie/data/runs/009

SUFFIX=run_009

# output directory
USER_OUT_DIR=/home/connie_ana/sims/addnu_to_data/runs/009_eff_02feb2018

# Processing macros location
MACROS_DIR=/share/storage2/connie/data_analysis/Processor/macros
# Configuration file
CONFIG_FILE="$MACROS_DIR"/drawConfig/simulationConfig_run009_NEW.xml

echo
echo "Input dir: " $RAW_DATA_DIR
echo "Conf file: " $CONFIG_FILE
echo "output dir:" $USER_OUT_DIR
echo 

# add events
python $MACROS_DIR/draw_events.py $RAW_DATA_DIR $CONFIG_FILE $USER_OUT_DIR

