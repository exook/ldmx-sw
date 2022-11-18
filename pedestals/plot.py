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
from scipy.fft import fft, fftfreq
#from LDMX.Framework import ldmxcfg

def gaus(x,a,b,c):
    return a*np.exp((-(x-b)**2)/(2*c**2))

def plot(root_file_path):
    #histogram_type = "alladc"
    histogram_type = "maxadc"
    specific_eid = "411044098"
    infile = uproot.open(root_file_path)
    listOfKeys = infile.keys()
    with PdfPages(root_file_path.split(".root")[0]+".pdf") as pdf:
        #figure1,  = plt.subplots(figsize=(18.3*(1/2.54)*1.7, 13.875*(1/2.54)*1.32))
        figure1, (ax1,ax2) = plt.subplots(2,1,figsize=(2*25*(1/2.54), 2*13.875*(1/2.54)),sharex=False)

        for key in listOfKeys:
            if key.split("_")[0] != histogram_type: continue
            if key.split("_")[2] != specific_eid+";1": continue
            print(key)
            data = infile[key].to_numpy()
            bins=data[1]
            counts=data[0]

            data_bin_centers = data[1][:-1]+(data[1][1:]-data[1][:-1])/2
            x = data_bin_centers    
            y = counts
            n = sum(counts)
            mean = np.mean(counts)
            sigma = sum(counts*(x-mean)**2)/n
            popt,pcov = curve_fit(gaus,x,y,p0=[1250,225,100])
            linspace=np.linspace(x[0],x[-1],num=10000)
            ax1.plot(linspace, gaus(linspace,*popt), lw=1,color="red")
            #print("x",list(x))
            #print("y",list(y))
            N = 600
            # sample spacing
            T = 1.0 / 800.0
            yf = fft(y)
            xf = fftfreq(N, T)[:N//2]
            ax2.plot(xf, 2.0/N * np.abs(yf[0:N//2]))
            ax2.set_xlim(0,1000)

            ax1.hist(bins[:-1], bins, weights=counts,histtype='step')

            ax1.set_xlim(180,300)
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
