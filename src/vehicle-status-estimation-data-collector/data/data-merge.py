import numpy as np
import pandas as pd
import glob
#merging csv files into txt file. Since excel has 1,048,576 rows, we are using txt file.
extension = 'csv'
all_filenames = [j for j in glob.glob('*.{}'.format(extension))]
combined_csv = pd.concat([pd.read_csv(f, header=None, low_memory=False) for f in all_filenames],axis=0)
np.savetxt("combined-data.txt", combined_csv.values, fmt='%1.7s', delimiter =",")