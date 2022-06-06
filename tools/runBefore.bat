python "..\tools\solveThermoLES.py" ".\mechanism.csv" ".\species.csv" ".\test.csv" ".\mechanism2.csv" ".\species2.csv"
python "..\tools\mechGen.py" ".\mechanism2.csv" ".\species2.csv" ".\phases.csv" ".\mechanism.cti" ".\template.txt"
python "..\tools\ctml_writer.py" ".\mechanism.cti"