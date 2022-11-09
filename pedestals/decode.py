"""Basic HGCROCv2RawDataFile reformatting configuration"""

import argparse, sys

# Run argparse
parser = argparse.ArgumentParser(f'ldmx fire {sys.argv[0]}')
parser.add_argument('--input', type=str, required=True)
parser.add_argument('--max_events',type=int, required=True)
parser.add_argument('--pause',action='store_true')
grp = parser.add_mutually_exclusive_group()
grp.add_argument('--keep_eids',action='store_true',
        help='Dont translate electronic into detector IDs.')
grp.add_argument('--recon',help='Attempt to reconstruct.',action='store_true')
parser.add_argument('--pedestals',default=None,type=str)
arg = parser.parse_args()

from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('unpack')
p.maxEvents = arg.max_events
p.termLogLevel = 0
p.logFrequency = 1

import LDMX.Hcal.hgcrocFormat as hcal_format
import LDMX.Hcal.digi as hcal_digi
import LDMX.Hcal.HcalGeometry
import LDMX.Hcal.hcal_hardcoded_conditions
from LDMX.DQM import dqm
from LDMX.Packing import rawio
import os
raw_file_path = arg.input
decoded_file_path = arg.input.replace('.raw','_-_decoded.root')
ntuple_file_path = arg.input.replace('.raw','_-_ntuple.root')
p.outputFiles = [decoded_file_path]
p.histogramFile = ntuple_file_path

if arg.keep_eids :
    tbl = None
else :
    tbl = f'{os.environ["LDMX_BASE"]}/ldmx-sw/Hcal/data/testbeam_connections.csv'

# sequence
#   1. decode event packet into digi collection
#   2. ntuplize digi collection
p.sequence = [ 
        hcal_format.HcalRawDecoder(
            input_file = arg.input,
            connections_table = tbl,
            output_name = 'ChipSettingsTestDigis'
            ),
        dqm.NtuplizeHgcrocDigiCollection(
            input_name = 'ChipSettingsTestDigis',
            pedestal_table = arg.pedestals
            )
        ]

# add recon if requested
if arg.recon :
    recon = hcal_digi.HcalRecProducer()
    recon.digiCollName = 'ChipSettingsTestDigis'
    p.sequence.append(recon)

if arg.pause :
    p.pause()
