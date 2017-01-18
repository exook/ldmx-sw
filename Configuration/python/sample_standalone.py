#!/usr/bin/python

import ldmxcfg;

p=ldmxcfg.Process("Fred")
p.libraries.append("EventProc/libEventProc.so")

test1=ldmxcfg.Analyzer("test1","DummyAnalyzer")
testp=ldmxcfg.Producer("testp","DummyProducer")
testhc=ldmxcfg.Producer("hcaldigi","HcalDigiProducer")

testp.parameters["n_particles"]=10
testp.parameters["ave_energy"]=0.50
testp.parameters["direction"]=[0.0,0.0,1.0]
testp.parameters["favorite_directory"]="/tmp"

#p.inputFiles=["test.root"]
p.sequence=[testp,test1,testhc]
p.maxEvents=5
p.run=101

p.printMe()
