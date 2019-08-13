/**
 * @file EventDisplay.h
 * @author Josh Hiltbrand, University of Minnesota
 */

#ifndef EVENTDISPLAY_EVENTDISPLAY_H_
#define EVENTDISPLAY_EVENTDISPLAY_H_

#include "TGTextEntry.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLViewer.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TString.h"
#include "TRint.h"

#include "TEveBrowser.h"
#include "TEveElement.h"
#include "TEveManager.h"
#include "TEveEventManager.h"
#include "TEveViewer.h"

#include "EventDisplay/EventObjects.h"
#include "EventDisplay/EveDetectorGeometry.h"

#include <iostream>

namespace ldmx {

    class EventDisplay : public TGMainFrame {

        public:

            /**
             * Constructor
             * Builds window frame and and control panel.
             * Imports geometry from EveDetectorGeometry.
             */
            EventDisplay( TEveManager* manager , bool verbose );

            /**
             * Destructor
             * Deletes hanging pointers from constructor and closes the TFile.
             */
            ~EventDisplay() {

                file_->Close();
                delete file_;
                delete tree_;
                delete theDetector_;
                delete eventObjects_;
                
                delete ecalDigiHits_;
                delete hcalDigiHits_;
                delete recoilHits_;
                delete ecalClusters_;
                delete ecalSimParticles_;

                delete textBox1_;
                delete textBox2_;
                delete textBox3_;
                delete textBox4_;
                delete textBox5_;
                delete textBox6_;
                delete textBox7_;
                delete textBox8_;
                delete textBox9_;
                delete textBox10_;
                delete textBox11_;

                delete manager_;
            }

            /**
             * Opens input file and attempts to obtain the necessary information from it.
             *
             * Attempts to import the event objects from the event tree using the 'Get...'
             * methods below.
             *
             * @param file name of file with events
             * @return true if successfully opened file and found tree named eventTreeName_
             * @return false unable to open file or find tree named eventTreeName_
             */
            bool SetFile(const TString file);

            /**
             * Goes back one event unless the current event number is not positive.
             *
             * @sa GotoEvent(int)
             */
            void PreviousEvent();

            /**
             * Goes forward one event unless the current event number equals the maximum event number.
             *
             * @sa GotoEvent(int)
             */
            void NextEvent();

            /**
             * Attempts to get branch from event tree.
             *
             * @param ecalDigisCollName name of ecalDigis collection
             * @return bool whether or not it was successful
             */
            bool GetECALDigisColl(const TString ecalDigisCollName);

            /**
             * Gets ecalDigis collection name from text box
             */
            void GetECALDigisCollInput();

            /**
             * Attempts to get branch from event tree.
             *
             * @param clustersCollName name of clusters collection
             * @return bool whether or not it was successful
             */
            bool GetClustersColl(const TString clustersCollName);

            /**
             * Gets clusters collection name from text box
             */
            void GetClustersCollInput();

            /**
             * Attempts to get branch from event tree.
             *
             * @param trackerHitsCollName name of trackerHits collection
             * @return bool whether or not it was successful
             */
            bool GetTrackerHitsColl(const TString trackerHitsCollName);

            /**
             * Gets trackerHits collection name from text box
             */
            void GetTrackerHitsCollInput();

            /**
             * Attempts to get branch from event tree.
             *
             * @param ecalSimParticlesCollName name of ecalSimParticles collection
             * @return bool whether or not it was successful
             */
            bool GetEcalSimParticlesColl(const TString ecalSimParticlesCollName);

            /**
             * Gets ecalSimParticles collection name from text box
             */
            void GetEcalSimParticlesCollInput();

            /**
             * Attempts to get branch from event tree.
             *
             * @param hcalDigisCollName name of hcalDigis collection
             * @return bool whether or not it was successful
             */
            bool GetHCALDigisColl(const TString hcalDigisCollName);

            /**
             * Gets hcalDigis collection name from text box
             */
            void GetHCALDigisCollInput();

            /**
             * Goes to the input event index if it is not outside the bounds.
             *
             * Destroys the elements from previous event and re-initializes the eventObjects_ instance.
             * Draws all objects that were able to be found.
             * 
             * @param event index for the event we want to go to.
             * @return bool success check
             */
            bool GotoEvent(int event);

            /**
             * Takes input of event index from text box.
             */
            bool GotoEvent();

            /**
             * Sets threshold energy from a SimParticle to be drawn from text box.
             * Re-draws the display.
             * @return bool success check
             */
            bool SetSimThresh();

            /**
             * Sets event tree from input text box.
             *
             * @return bool success check
             */
            bool SetEventTree();

            /**
             * Colors cluster objects and redraws.
             */
            void ColorClusters();

        private:

            bool verbose_; //* verbosity flag

            TFile* file_; //* Event file
            TTree* tree_; //* Event tree

            TClonesArray* ecalDigiHits_; //* current ecalDigis collection
            TClonesArray* hcalDigiHits_; //* current hcalDigis collection
            TClonesArray* recoilHits_; //* curent recoil hits collection
            TClonesArray* ecalClusters_; //* current ecal clusters collection
            TClonesArray* ecalSimParticles_; //* current ecal sim particles collection

            bool foundECALDigis_ = false; //* flag check if ecalDigis collection has been found
            bool foundHCALDigis_ = false;//* flag check if hcalDigis collection has been found
            bool foundClusters_ = false;//* flag check if clusters collection has been found
            bool foundTrackerHits_ = false;//* flag check if tracker hits collection has been found
            bool foundEcalSPHits_ = false;//* flag check if ecal sim particles collection has been found

            int eventNum_ = -1; //* current event number
            int eventNumMax_; ///* maximum event index for the current tree

            TString clustersCollName_ = "ecalClusters_recon"; //* name of ecal clusters collection in event tree
            TString ecalDigisCollName_ = "ecalDigis_recon"; //* name of ecalDigis collection in event tree
            TString hcalDigisCollName_ = "hcalDigis_recon"; //* name of hcalDigis collection in event tree
            TString trackerHitsCollName_ = "RecoilSimHits_sim"; //* name of recoil hitss collection in event tree
            TString ecalSimParticlesCollName_ = "EcalScoringPlaneHits_sim"; //* name of ecal sim particles collection in event tree
            TString eventTreeName_ = "LDMX_Events"; //* name of event tree

            EveDetectorGeometry* theDetector_{nullptr}; //* detector geometry instance
            EventObjects* eventObjects_{nullptr}; //* drawing methods for event objects

            TGTextEntry* textBox1_;
            TGTextEntry* textBox2_;
            TGTextEntry* textBox3_;
            TGTextEntry* textBox4_;
            TGTextEntry* textBox5_;
            TGTextEntry* textBox6_;
            TGTextEntry* textBox7_;
            TGTextEntry* textBox8_;
            TGTextEntry* textBox9_;
            TGTextEntry* textBox10_;
            TGTextEntry* textBox11_;

            TEveManager* manager_{nullptr}; //* event display manager

            ClassDef(EventDisplay, 1);
    };
}

#endif
