from LDMX.Framework import ldmxcfg
p = ldmxcfg.Process('target_mumu')
from LDMX.Biasing import target
from LDMX.SimCore import generators
import LDMX.Ecal.EcalGeometry
import LDMX.Hcal.HcalGeometry
p.sequence = [
    target.gamma_mumu(
        'ldmx-det-v12' ,
        generators.single_4gev_e_upstream_tagger()
        )
    ]
p.maxEvents = 1000
p.maxTriesPerEvent = 1000
p.outputFiles = [ 'target_mumu.root' ]
