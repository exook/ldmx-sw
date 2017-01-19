#include <iostream>
#include "Framework/EventProcessor.h"
#include "Framework/EventImpl.h"
#include "Framework/EventFile.h"
#include "Framework/Process.h"

namespace ldmxsw {

    Process::Process(const std::string& passname) : passname_{passname} {
    }

    
    void Process::run(int eventlimit) {
	try {
	    int n_events_processed=0;
	    
	    // first, notify everyone that we are starting
	    for (auto module : sequence_)
		module->onProcessStart();
	    
	    // if we have no input files, but do have an event number, run for that number of events on an output file
	    if (inputFiles_.empty() && eventlimit>0) {
		EventFile outFile(outputFiles_[0],true);
		
		for (auto module : sequence_)
		    module->onFileOpen();
		
		EventImpl theEvent(passname_);
		outFile.setupEvent(&theEvent);
		
		while (n_events_processed<eventlimit) {
		    event::EventHeader& eh=theEvent.getEventHeaderMutable();
		    eh.setRun(runForGeneration_);
		    eh.setEventNumber(n_events_processed+1);
		    eh.setTimestamp(TTimeStamp());
		    
		    theEvent.getEventHeader()->Print();
		    
		    for (auto module : sequence_)
			if (dynamic_cast<Producer*>(module)) (dynamic_cast<Producer*>(module))->produce(theEvent);
			else if (dynamic_cast<Analyzer*>(module)) (dynamic_cast<Analyzer*>(module))->analyze(theEvent);
		    
		    outFile.nextEvent();
		    theEvent.Clear();
		    n_events_processed++;
		}
		
		for (auto module : sequence_)	  
		    module->onFileClose();
		
		outFile.close();
	    } else {
		if (!outputFiles_.empty() && outputFiles_.size()!=inputFiles_.size()) {
		    EXCEPTION_RAISE("Process","Unable to handle case of different number of input and output files (other than zero output files)");
		}
		// next, loop through the files
		int ifile=0;
		for (auto infilename : inputFiles_) {
		    EventFile inFile(infilename);
		    std::cout << "Process: Opening file " << infilename << std::endl;
		    EventFile* outFile(0);
		    
		    if (!outputFiles_.empty()) {
			outFile=new EventFile(outputFiles_[ifile],&inFile);
			ifile++;		    
		    
			for (auto rule : dropKeepRules_)
			    outFile->addDrop(rule);
		    }
		
		    for (auto module : sequence_)
			module->onFileOpen();
		    
		    EventImpl theEvent(passname_);
		    if (outFile) outFile->setupEvent(&theEvent);
		    else inFile.setupEvent(&theEvent);
		    
		    EventFile* masterFile=(outFile)?(outFile):(&inFile);
		    
		    while (masterFile->nextEvent()) {	      
			for (auto module : sequence_)
			    if (dynamic_cast<Producer*>(module)) (dynamic_cast<Producer*>(module))->produce(theEvent);
			    else if (dynamic_cast<Analyzer*>(module)) (dynamic_cast<Analyzer*>(module))->analyze(theEvent);
			n_events_processed++;
		    }

		    if (outFile) outFile->close();
		    inFile.close();
		    std::cout << "Process: Closing file " << infilename << std::endl;	  
		    for (auto module : sequence_)	  
			module->onFileClose();
		}
	    }
      
	    // finally, notify everyone that we are stopping
	    for (auto module : sequence_)
		module->onProcessEnd();
	} catch (Exception& e) {
	    std::cerr << "Framework Error [" << e.name() << "] : " << e.message() << std::endl;
	    std::cerr << "  at " << e.module() <<":"<<e.line()<<" in " <<e.function() << std::endl;
	}
    
    }

    void Process::addToSequence(EventProcessor* mod) {
	sequence_.push_back(mod);
    }
    void Process::addFileToProcess(const std::string& filename) {
	inputFiles_.push_back(filename);
    }
    void Process::addDropKeepRule(const std::string& rule) {
	dropKeepRules_.push_back(rule);
    }
    void Process::setOutputFileName(const std::string& filenameOut) {
	outputFiles_.clear();
	outputFiles_.push_back(filenameOut);
    }
    void Process::addOutputFileName(const std::string& filenameOut) {
	outputFiles_.push_back(filenameOut);
    }
  
}
