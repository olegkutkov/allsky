'''
 Fits processor for bolids cloud network

 Copyright 2017 Vsevolod Finogenov, URFU.
 Copyright 2017 Anisia Likova, URFU.
 Copyright 2017 Anastasiya Ovchinnikova, URFU.
 Copyright 2017 Vadim Krushinskiy, URFU.
 Copyright 2017 Nikolay Kruglikov, URFU.
 Copyright 2017 Oleg Kutkov, CRAO

 This program is free software. 

'''

import config
import os
import sys
import numpy as nu
import pyfits as py
import matplotlib.pyplot
import matplotlib.cm as cm
import scipy.ndimage.filters as filters
import scipy.ndimage as ndimage
import PIL
import shutil
import warnings
import ftplib


def check_paths(path):
	save_path = os.path.join(path, config.SAVE_PATH)

	if not os.path.exists(save_path):
		os.makedirs(save_path)

	nice_path = os.path.join(path, config.NICE_PATH)

	if not os.path.exists(nice_path):
		os.makedirs(nice_path)

	dest_path = os.path.join(path, config.DEST_PATH)

	if not os.path.exists(dest_path):
		os.makedirs(dest_path)

def main(args):
	if len(args) < 2:
		print 'Please pass working directory path'
		sys.exit(1)

	workdir = args[1]

	check_paths(workdir)

	
	

if __name__ == "__main__":
	main(sys.argv)

