#import pandas as pd
import os
import ROOT
import sys
import uproot
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
from scipy.optimize import curve_fit
from scipy import asarray as ar,exp
import argparse
import subprocess
#from LDMX.Framework import ldmxcfg

#def gaus(x,a,b,c):
#    return a*np.exp((-(x-b)**2)/(2*c**2))

def plot(root_file_path):
    histogram_type = "alladc"
    infile = uproot.open(root_file_path)
    listOfKeys = infile.keys()
    with PdfPages(root_file_path.split(".root")[0]+".pdf") as pdf:
        figure1, ax1 = plt.subplots(figsize=(18.3*(1/2.54)*1.7, 13.875*(1/2.54)*1.32))

        for key in listOfKeys:
            if key.split("_")[0] != histogram_type: continue
            print(key)
            data = infile[key].to_numpy()
            bins=data[1]
            counts=data[0]

#            data_bin_centers = data[1][:-1]+(data[1][1:]-data[1][:-1])/2
#            x = data_bin_centers    
#            y = counts
#            n = sum(counts)
#            mean = np.mean(counts)
#            sigma = sum(counts*(x-mean)**2)/n
#            popt,pcov = curve_fit(gaus,x,y,p0=[350,100,2])

            ax1.hist(bins[:-1], bins, weights=counts,histtype='step')

            ax1.set_xlim(80,120)
            ax1.set_title(f"{key}, {sum(counts)}")
            ax1.set_xlabel("ADC", ha='right', x=1.0)
            ax1.set_ylabel("Counts", ha='right', y=1.0)
            pdf.savefig()
            ax1.clear()

    infile.close()


def get_arguments():
    parser = argparse.ArgumentParser(
                    prog = "mine.py",
                    description =   '''Heyo!''',
                    epilog = 'Enjoy!')
    parser.add_argument('--input', type=str, required=True, help='Path to input data set for compression')
    args = parser.parse_args()
    return args.input


def main():
    raw_path = get_arguments()
    unpacked_path = raw_path.replace('.raw','_-_unpacked.root')
    plot(unpacked_path)
        

if __name__ == "__main__":
    main()
