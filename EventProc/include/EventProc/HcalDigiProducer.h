/**
 * @file HcalDigiProducer.h
 * @brief Class that performs digitization of simulated HCal data
 * @author Andrew Whitbeck, FNAL
 */

#ifndef EVENTPROC_HCALDIGIPRODUCER_H_
#define EVENTPROC_HCALDIGIPRODUCER_H_

// C++/STL
#include <time.h>
#include <memory>

// ROOT
#include "TString.h"
#include "TRandom3.h"

// LDMX
#include "DetDescr/DetectorID.h"
#include "DetDescr/HcalID.h"
#include "Event/EventConstants.h"
#include "Event/HcalHit.h"
#include "Event/SimCalorimeterHit.h"
#include "Framework/EventProcessor.h"
#include "Tools/NoiseGenerator.h"

namespace ldmx {

    /**
     * @class HcalDigiProducer
     * @brief Performs digitization of simulated HCal data
     */
    class HcalDigiProducer : public Producer {

        public:

            typedef int layer;

            typedef std::pair<double, double> zboundaries;

            HcalDigiProducer(const std::string& name, Process& process);

            virtual ~HcalDigiProducer();

            virtual void configure(const ParameterSet&);

            virtual void produce(Event& event);

            unsigned int generateRandomID(HcalSection sec);

        private:

            TClonesArray* hits_{nullptr};
            std::unique_ptr<TRandom3> random_;
            std::map<layer, zboundaries> hcalLayers_;
            bool verbose_{false};
            std::unique_ptr<DetectorID> detID_;
            
            /** Generator for simulating noise hits. */
            std::unique_ptr<NoiseGenerator> noiseGenerator_;

            double meanNoise_{0};
            int    nProcessed_{0};
            double mev_per_mip_{1.40};
            double pe_per_mip_{13.5};
            double strip_attenuation_length_{100.};
            double strip_position_resolution_{150.};
            int    readoutThreshold_{2};
            int    doStrip_{true};
            int    STRIPS_BACK_PER_LAYER_{31};
            int    NUM_BACK_HCAL_LAYERS_{150};
            int    STRIPS_SIDE_TB_PER_LAYER_{6};
            int    NUM_SIDE_TB_HCAL_LAYERS_{31};
            int    STRIPS_SIDE_LR_PER_LAYER_{31};
            int    NUM_SIDE_LR_HCAL_LAYERS_{63};
            int    SUPER_STRIP_SIZE_{1};
    };

}

#endif
