//  GAMBIT: Global and Modular BSM Inference Tool   
//  //  ********************************************
//  //                                              
//  //  Functions for Delphes3Backend
//  //                                              
//  //  ********************************************
//  //                                              
//  //  Authors                                     
//  //  =======                                     
//  //                                              
//  //  (add name and date if you modify)           
//  //                                              
//  //  Abram Krislock
//  //  2013 Apr 19, Apr 23
//  //                                              
//  //  ********************************************
//                                                  
//                                                  
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdlib>

#include "Delphes3Backend.hpp"

using namespace std;

namespace GAMBIT
{
  namespace HEColliderBit
  {
    namespace Delphes3Backend
    {
      // To read Delphes Config File
      ExRootConfReader *confReader;
      // Modularity of Delphes is set by said Config File.
      Delphes *modularDelphes;
      // Factory production of particle "candidates"
      DelphesFactory *factory;
      // Delphes particle arrays: Pre-Detector-Sim
      TObjArray *allParticleOutputArray;
      TObjArray *stableParticleOutputArray;
      TObjArray *partonOutputArray;
      // Database of PDG codes and particle info
      TDatabasePDG *pdg;
      // Used within the event Converters below
      Candidate *candidate;
      TParticlePDG *pdgParticle;
      Int_t pdgCode;

      void initialize(string configFileName)
      {
        try
        {
          // To read Delphes Config File
          confReader = new ExRootConfReader();
          confReader->ReadFile(configFileName.c_str());

          // Modularity of Delphes set by Config File
          modularDelphes = new Delphes("Delphes");
          modularDelphes->SetConfReader(confReader);

          // Factory production of particle "candidates"
          factory = modularDelphes->GetFactory();

          // Delphes particle arrays: Pre-Detector-Sim
          allParticleOutputArray = modularDelphes->ExportArray("allParticles");
          stableParticleOutputArray = modularDelphes->ExportArray("stableParticles");
          partonOutputArray = modularDelphes->ExportArray("partons");

          // Database of PDG codes and particle info
          pdg = TDatabasePDG::Instance();

          modularDelphes->InitTask();
        }
        catch(runtime_error &e)
        {
          cerr << "** ERROR: " << e.what() << endl;
          exit(EXIT_FAILURE);
        }
      }


      void finalize()
      {
        try
        {
          // Finalization and deletion of stuff
          modularDelphes->FinishTask();
          delete confReader;
          delete modularDelphes;
        }
        catch(runtime_error &e)
        {
          cerr << "** ERROR: " << e.what() << endl;
          exit(EXIT_FAILURE);
        }
      }


      void analyzeEvent(Pythia8::Event &eventIn, ReconstructedEvent &eventOut)
      {
        try
        {
          modularDelphes->Clear();
          convertInput(eventIn);
          modularDelphes->ProcessTask();
          convertOutput(eventOut);
        }
        catch(runtime_error &e)
        {
          cerr << "** ERROR: " << e.what() << endl;
          exit(EXIT_FAILURE);
        }
      }


      void convertInput(Pythia8::Event &event)
      {
        for (int ip = 0; ip < event.size(); ++ip)
        {
          const Pythia8::Particle& p = event[ip];
          candidate = factory->NewCandidate();

          candidate->PID = p.id();
          pdgCode = TMath::Abs(candidate->PID);

          candidate->Status = p.status();

          pdgParticle = pdg->GetParticle(p.id());
          candidate->Charge = pdgParticle ? Int_t(pdgParticle->Charge()/3.0) : -999;
          candidate->Mass = pdgParticle ? pdgParticle->Mass() : -999.9;

          candidate->Momentum.SetPxPyPzE(p.px(), p.py(), p.pz(), p.e());
          candidate->Position.SetXYZT(p.xProd(), p.yProd(), p.zProd(), p.tProd());

          allParticleOutputArray->Add(candidate);
          if(!pdgParticle)
            continue;
          if(p.isFinal())
          {
            stableParticleOutputArray->Add(candidate);
          }
          else if(pdgCode <= 5 || pdgCode == 21 || pdgCode == 15)
          {
            partonOutputArray->Add(candidate);
          }
        }
      }


      // ReconstructedEvent is a boring class in the HEColliderBit namespace
      //    See the DelphesGambit.hpp file for details
      void convertOutput(ReconstructedEvent &event)
      {
        event.missingET = -1.;
        event.missingEPhi = -1.;
        // Delphes particle arrays: Post-Detector Sim
        //    MISSING ET:
        const TObjArray *arrayMissingET = modularDelphes->ImportArray("MissingET/momentum");
        if((candidate = static_cast<Candidate*>(arrayMissingET->At(0))))
        {
          const TLorentzVector &momentum = candidate->Momentum;
          event.missingET = momentum.Pt();
          event.missingEPhi = momentum.Phi();
        }

        // Delphes particle arrays: Post-Detector Sim
        //    PHOTONS:
        const TObjArray *arrayPhotons = modularDelphes->ImportArray("UniqueObjectFinder/photons");
        TIter iteratorPhotons(arrayPhotons);
        iteratorPhotons.Reset();
        while((candidate = static_cast<Candidate*>(iteratorPhotons.Next())))
        {
          const TLorentzVector &momentum = candidate->Momentum;
          Pythia8::Vec4 photon;
          photon.p(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
          event.photons.push_back(photon);
        }

        // Delphes particle arrays: Post-Detector Sim
        //    ELECTRONS:
        const TObjArray *arrayElectrons = modularDelphes->ImportArray("UniqueObjectFinder/electrons");
        TIter iteratorElectrons(arrayElectrons);
        iteratorElectrons.Reset();
        while((candidate = static_cast<Candidate*>(iteratorElectrons.Next())))
        {
          const TLorentzVector &momentum = candidate->Momentum;
          Pythia8::Vec4 electron;
          electron.p(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
          event.electrons.push_back(electron);
        }

        // Delphes particle arrays: Post-Detector Sim
        //    MUONS:
        const TObjArray *arrayMuons = modularDelphes->ImportArray("MuonIsolation/muons");
        TIter iteratorMuons(arrayMuons);
        iteratorMuons.Reset();
        while((candidate = static_cast<Candidate*>(iteratorMuons.Next())))
        {
          const TLorentzVector &momentum = candidate->Momentum;
          Pythia8::Vec4 muon;
          muon.p(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
          event.muons.push_back(muon);
        }

        // Delphes particle arrays: Post-Detector Sim
        //    JETS:
        const TObjArray *arrayJets = modularDelphes->ImportArray("UniqueObjectFinder/jets");
        TIter iteratorJets(arrayJets);
        iteratorJets.Reset();
        while((candidate = static_cast<Candidate*>(iteratorJets.Next())))
        {
          const TLorentzVector &momentum = candidate->Momentum;
          Pythia8::Vec4 jet;
          jet.p(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
          // TODO: Is it possible for a jet to be BOTH b and tau tagged??
          //       If so, we need to handle that case somehow.
          if(candidate->BTag == 1)
            event.bjets.push_back(jet);
          else if(candidate->TauTag == 1)
            event.taus.push_back(jet);
          else
            event.jets.push_back(jet);
        }
      }
    }
  }
}