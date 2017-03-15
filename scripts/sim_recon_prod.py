import sys

from LDMX.Framework import ldmxcfg

p=ldmxcfg.Process("recon")

p.libraries.append("libEventProc.so")

ecalDigis = ldmxcfg.Producer("ecalDigis","ldmx::EcalDigiProducer")
ecalDigis.parameters["meanNoise"] = 0.015
ecalDigis.parameters["readoutThreshold"] = ecalDigis.parameters["meanNoise"]*3

hcalDigis = ldmxcfg.Producer("hcalDigis", "ldmx::HcalDigiProducer")
hcalDigis.parameters["meanNoise"] = 1.5

ecalVeto = ldmxcfg.Producer("ecalVeto", "ldmx::EcalVetoProcessor")
ecalVeto.parameters["num_ecal_layers"] = 50
ecalVeto.parameters["back_ecal_starting_layers"] = 22
ecalVeto.parameters["num_layers_for_med_cal"] = 10
ecalVeto.parameters["total_dep_cut"] = 25.
ecalVeto.parameters["total_outer_cut"] = 15.
ecalVeto.parameters["back_ecal_cut"] = 1.
ecalVeto.parameters["ratio_cut"] = 10.

trigger = ldmxcfg.Producer("trigger", "ldmx::TriggerProcessor")
trigger.parameters["threshold"] = 12.0
trigger.parameters["mode"] = 0
trigger.parameters["start_layer"] = 1
trigger.parameters["end_layer"] = 16

findable_track = ldmxcfg.Producer("findable", "ldmx::FindableTrackProcessor")

p.sequence=[ecalDigis, hcalDigis, ecalVeto, trigger, findable_track]

p.inputFiles=["/nfs/slac/g/ldmx/users/jeremym/prod/scratch/derp_0001.root"]
p.outputFiles=["herpcon_0001.root"]

p.printMe()
