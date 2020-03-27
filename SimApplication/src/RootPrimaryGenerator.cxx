/**
 * @file RootPrimaryGenerator.cxx
 * @brief Primary generator used to generate primaries from SimParticles. 
 * @author Nhan Tran, Fermilab
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimApplication/RootPrimaryGenerator.h"

//----------------//
//   C++ StdLib   //
//----------------//
#include <unordered_map>

//------------//
//   Geant4   //
//------------//
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

//-------------//
//   ldmx-sw   //
//-------------//
#include "Event/EventConstants.h"
#include "Event/SimParticle.h"
#include "Event/SimTrackerHit.h"
#include "Framework/Parameters.h"
#include "SimApplication/UserPrimaryParticleInformation.h"

namespace ldmx {

    RootPrimaryGenerator::RootPrimaryGenerator( const std::string& name , Parameters& parameters )
        : PrimaryGenerator( name , parameters ) {

        filename_ = parameters_.getParameter< std::string >( "filePath" );
        ifile_ = new TFile(filename_);
        itree_ = (TTree*) ifile_->Get(EventConstants::EVENT_TREE_NAME.c_str());
        eventHeader_ = 0;
        itree_->SetBranchAddress(EventConstants::EVENT_HEADER.c_str(), &eventHeader_);
        itree_->SetBranchAddress("SimParticles_sim", &simParticles_);
        itree_->SetBranchAddress("EcalScoringPlaneHits_sim", &ecalSPParticles_);
        evtCtr_ = 0;
        nEvts_ = itree_->GetEntriesFast();
        runMode_ = parameters_.getParameter< int >( "runMode" );
    }

    RootPrimaryGenerator::~RootPrimaryGenerator() { 

        //TODO cleanup?
        ifile_->Close();
        delete ifile_;
    }

    void RootPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent) {

        // TODO: Instead of having two different modes, each mode should exists
        //       as its own primary generator. 

        if (evtCtr_ >= nEvts_) {
            std::cout << "[ RootPrimaryGenerator ]: End of file reached." << std::endl;
            G4RunManager::GetRunManager()->AbortRun(true);
            anEvent->SetEventAborted();
        }

        itree_->GetEntry(evtCtr_);

        // Mode == 0; regenerate the same events (with the useSeed option toggled on)
        // Mode == 1; generate events from the ecal scoring plane hits
        int theMode = runMode_;

        if (theMode == 1) {

            // In this mode, we need to loop through all ECal scoring plane hits
            // and find the subset of unique hits created by particles exiting
            // the ECal.  These particles will be stored in a container and 
            // re-fired into the HCal. 
            std::unordered_map<int, SimTrackerHit*> spHits; 

            // Loop through all of the ECal scoring plane hits. 
            for (SimTrackerHit &spHit : ecalSPParticles_ ) {

                // First, start by skipping all hits that were created by 
                // particles entering the ECal volume. 
                if (spHit.getLayerID() == 1 and spHit.getMomentum()[2] > 0) continue;
                if (spHit.getLayerID() == 2 and spHit.getMomentum()[2] < 0) continue; 
                if (spHit.getLayerID() == 3 and spHit.getMomentum()[1] < 0) continue; 
                if (spHit.getLayerID() == 4 and spHit.getMomentum()[1] > 0) continue; 
                if (spHit.getLayerID() == 5 and spHit.getMomentum()[0] > 0) continue; 
                if (spHit.getLayerID() == 6 and spHit.getMomentum()[0] < 0) continue;

                // Don't consider particles created outside of the HCal readout
                // window.  Currently, this is estimated to be 50 ns.  
                // TODO: This value should be made configurable. 
                if (spHit.getTime() > 50) continue; 

                if (spHits.find(spHit.getTrackID()) == spHits.end()) {
                    spHits[spHit.getTrackID()] = &spHit; 
                } else {  
                        
                   float currentPMag = sqrt(
                                      pow(spHit.getMomentum()[0], 2) +
                                      pow(spHit.getMomentum()[1], 2) +
                                      pow(spHit.getMomentum()[2], 2)); 
                   float pMag = sqrt(
                                      pow(spHits[spHit.getTrackID()]->getMomentum()[0], 2) +
                                      pow(spHits[spHit.getTrackID()]->getMomentum()[1], 2) +
                                      pow(spHits[spHit.getTrackID()]->getMomentum()[2], 2)); 

                    if (pMag < currentPMag) spHits[spHit.getTrackID()] = &spHit; 
                } 
            } 

            for (auto const& spHit : spHits) { 

                auto cVertex{new G4PrimaryVertex()};
                cVertex->SetPosition(spHit.second->getPosition()[0]*mm, spHit.second->getPosition()[1]*mm, spHit.second->getPosition()[2]*mm);
                cVertex->SetWeight(1.);

                auto primary{new G4PrimaryParticle()};
                primary->SetPDGcode(spHit.second->getPdgID());
                primary->SetMomentum(spHit.second->getMomentum()[0]*MeV, spHit.second->getMomentum()[1]*MeV, spHit.second->getMomentum()[2]*MeV);

                auto primaryInfo{new UserPrimaryParticleInformation()};
                primaryInfo->setHepEvtStatus(1.);
                primary->SetUserInformation(primaryInfo);

                cVertex->SetPrimary(primary);
                anEvent->AddPrimaryVertex(cVertex);

            }   

        } else if (theMode == 0) {

            // put in protection for if we run out of ROOT events
            std::vector<G4PrimaryVertex*> vertices;
            for (const SimParticle &sp : simParticles_ ) {

                // check if particle has status 1
                if (sp.getGenStatus() != 1)
                    continue;

                bool vertexExists = false;
                G4PrimaryVertex* curvertex = new G4PrimaryVertex();
                for (unsigned int iV = 0; iV < vertices.size(); ++iV) {
                    double cur_vx = sp.getVertex()[0];
                    double cur_vy = sp.getVertex()[1];
                    double cur_vz = sp.getVertex()[2];
                    double i_vx = vertices.at(iV)->GetX0();
                    double i_vy = vertices.at(iV)->GetY0();
                    double i_vz = vertices.at(iV)->GetZ0();
                    if ((cur_vx == i_vx) && (cur_vy == i_vy) && (cur_vz == i_vz)) {
                        vertexExists = true;
                        curvertex = vertices.at(iV);
                    }
                }
                if (vertexExists == false) {
                    curvertex->SetPosition(sp.getVertex()[0], sp.getVertex()[1], sp.getVertex()[2]);
                    curvertex->SetWeight(1.);
                    anEvent->AddPrimaryVertex(curvertex);
                }

                G4PrimaryParticle* primary = new G4PrimaryParticle();
                primary->SetPDGcode(sp.getPdgID());
                primary->SetMomentum(sp.getMomentum()[0] * MeV, sp.getMomentum()[1] * MeV, sp.getMomentum()[2] * MeV);
                primary->SetMass(sp.getMass() * MeV);

                UserPrimaryParticleInformation* primaryInfo = new UserPrimaryParticleInformation();
                primaryInfo->setHepEvtStatus(1.);
                primary->SetUserInformation(primaryInfo);

                curvertex->SetPrimary(primary);

            }
        }
        else {
            std::cerr << "Mode value is invalid!" << std::endl;
        }

        std::ofstream tmpout("tmpEvent.rndm");
        std::string eventSeed = eventHeader_->getStringParameter("eventSeed");
        tmpout << eventSeed;
        tmpout.close();

        // move to the next event
        evtCtr_++;

    }

} //ldmx

DECLARE_GENERATOR( ldmx , RootPrimaryGenerator )
