python "D:\Dropbox (Cambridge CARES)\SRIHM WorkSpace\24_Python\python mechanismGenerator\solveThermoLES.py" "mechanism.csv" "species.csv" "test.csv" "mechanism2.csv" "species2.csv"
python "D:\Dropbox (Cambridge CARES)\SRIHM WorkSpace\24_Python\python mechanismGenerator\mechGen.py" "mechanism2.csv" "species2.csv" "phases.csv" "mechanism.cti" "template.txt"
python "D:\Users\SRIH01\Documents\cantera\interfaces\cython\cantera\ctml_writer.py" "mechanism.cti"