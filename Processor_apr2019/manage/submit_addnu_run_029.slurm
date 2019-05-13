#!/bin/bash
#
#SBATCH --job-name=addnu-sim-029-extE
#SBATCH --ntasks=1
#SBATCH --time=72:00:00

#SBATCH --mail-type=FAIL,END
#SBATCH --mail-user=alexis@nucleares.unam.mx,fmoroni.guillermo@gmail.com

# setup system
source setup-system.sh

echo -e "\n## Slurm job started on $(date +'%d-%m-%Y at %T') #####################"

#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage2/connie/data/runs/029

SUFFIX=run_029

# output directory
USER_OUT_DIR=/share/storage2/connie/sims/addnu_to_data/runs/029_with_009higherenergyconfig

# Processing macros location
MACROS_DIR=/share/storage2/connie/data_analysis/Processor/macros
# Configuration file
CONFIG_FILE="$MACROS_DIR"/drawConfig/simulationConfig_run009_238_to_302_23nov2018.xml

echo
echo "Input dir: " $RAW_DATA_DIR
echo "Conf file: " $CONFIG_FILE
echo "output dir:" $USER_OUT_DIR
echo 

# add events
srun python $MACROS_DIR/draw_events.py $RAW_DATA_DIR $CONFIG_FILE $USER_OUT_DIR >> $USER_OUT_DIR/draw_$SUFFIX.log 2>&1


#-----------------------------------------------------------------------------
# ouptut to slurm log file

echo "## Job name:                         $SLURM_JOB_NAME"
echo "## Job ID:                           $SLURM_JOB_ID"


# append slurm log file to processing log file
echo -e "\n=================================================" >> $OUT_DIR/$TYPE"_"$SUFFIX.log
echo "slurm-$SLURM_JOB_ID.out:" >> $USER_OUT_DIR/draw_$SUFFIX.log
if [ -d "$OUT_DIR" ]; then
  cat slurm-$SLURM_JOB_ID.out >> $USER_OUT_DIR/draw_$SUFFIX.log
  rm slurm-$SLURM_JOB_ID.out
fi


