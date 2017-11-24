#!/bin/bash
#
#SBATCH --job-name=processing
#SBATCH --ntasks=1
#SBATCH --time=72:00:00

#SBATCH --mail-type=FAIL,END
#SBATCH --mail-user=alexis@nucleares.unam.mx

# setup system
source setup-system.sh

echo -e "\n## Slurm job started on $(date +'%d-%m-%Y at %T') #####################"

#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage2/connie/data/runs/009
FID=238
LID=239

# base output directory
BASE_OUT_DIR=/share/storage2/connie/data_analysis/test_Processor/processed_data/runs/009_wOS

# Processing macros location
MACROS_DIR=/share/storage2/connie/data_analysis/test_Processor/macros

# damicTools options
export OPTS="-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 -s 12 -s 13 -s 14 -s 15 "

#-----------------------------------------------------------------------------
# output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
OUT_DIR=$BASE_OUT_DIR"/data_"$SUFFIX

# Create lists
python $MACROS_DIR/makeList.py $RAW_DATA_DIR $BASE_OUT_DIR $FID $LID > listMB_$SUFFIX.log
cat listMB_$SUFFIX.log > $OUT_DIR/data_$SUFFIX.log; rm listMB_$SUFFIX.log

# doAll
cd $MACROS_DIR
srun ./doAll.sh $OUT_DIR >> $OUT_DIR/data_$SUFFIX.log 2>&1
cd -

echo -e "## Slurm job finished on $(date +'%d-%m-%Y at %T') ####################"

#-----------------------------------------------------------------------------
# ouptut to slurm log file

echo "## Job name:                         $SLURM_JOB_NAME"
echo "## Job ID:                           $SLURM_JOB_ID"

# append slurm log file to processing log file
echo -e "\n=================================================" >> $OUT_DIR/data_$SUFFIX.log
echo "slurm-$SLURM_JOB_ID.out:" >> $OUT_DIR/data_$SUFFIX.log
cat slurm-$SLURM_JOB_ID.out >> $OUT_DIR/data_$SUFFIX.log
rm slurm-$SLURM_JOB_ID.out
