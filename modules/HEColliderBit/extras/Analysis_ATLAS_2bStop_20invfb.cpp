#include "Analysis.hpp"
#include <vector>
#include <cmath>
#include <memory>
/// @todo Remove the ROOT...
#include "TLorentzVector.h"
#include "TVector2.h"
#include <iomanip>
#include "mt2_bisect.h"

// The ATLAS 2b stop/bottom analysis (20fb^-1)
//
// based on: arXiv:1308.2631
//
//    Code by Martin White, Sky French.
//    Known errors:
//    a) The isolation is already applied in the simulation rather than after overlap removal -> the electron and muon veto technically require a veto on base-line electrons/muons not overlapping with jets
//
//    Known features:
//    a) Must run simulator with 60% b tagging efficiency and ?% mis-id rate


namespace Gambit {


  using namespace std;

  class Analysis_ATLAS_2bStop_20invfb : public Analysis {
  private:

    // Numbers passing cuts
    int _numSRA, _numSRB;
    int _numSRA15, _numSRA20, _numSRA25, _numSRA30, _numSRA35;

    vector<int> cutFlowVector_alt;
    vector<int> cutFlowVector;
    vector<string> cutFlowVector_str;
    int NCUTS; //=2;

    // Debug histos

  public:

    Analysis_ATLAS_2bStop_20invfb() {

      _numSRA = 0 ; _numSRB = 0; _numSRA15 = 0; _numSRA20 = 0; _numSRA25 = 0; _numSRA30 = 0; _numSRA35 = 0;
      NCUTS=2;

      for(int i=0;i<NCUTS;i++){
        cutFlowVector.push_back(0);
        cutFlowVector_str.push_back("");
        cutFlowVector_alt.push_back(0);
      }

    }

    void analyze(const Event* event) {

      // Missing energy
      P4 ptot = event->missingmom();
      double met = event->met();

      // Now define vectors of baseline objects
      vector<Particle*> baselineElectrons;
      for (Particle* electron : event->electrons()) {
        if (electron->pT() > 7. && fabs(electron->eta()) < 2.47) baselineElectrons.push_back(electron);
      }
      vector<Particle*> baselineMuons;
      for (Particle* muon : event->muons()) {
        if (muon->pT() > 6. && fabs(muon->eta()) < 2.4) baselineMuons.push_back(muon);
      }

      vector<Jet*> baselineJets;
      vector<Jet*> bJets;
      vector<Jet*> trueBJets; //for debugging
      for (Jet* jet : event->jets()) {
        if (jet->pT() > 20. && fabs(jet->eta()) < 4.9) baselineJets.push_back(jet);
        if(jet->isBJet() && fabs(jet->eta()) < 2.5 && jet->pT() > 20.) bJets.push_back(jet);
      }

      // Overlap removal
      vector<Particle*> signalElectrons;
      vector<Particle*> signalMuons;
      vector<Particle*> electronsForVeto;
      vector<Particle*> muonsForVeto;
      vector<Jet*> goodJets;
      vector<Jet*> signalJets;

      // Remove any jet within dR=0.2 of an electrons
      for (size_t iJet=0;iJet<baselineJets.size();iJet++) {
        bool overlap=false;
        P4 jetVec=baselineJets.at(iJet)->mom();
        for (size_t iEl=0;iEl<baselineElectrons.size();iEl++) {
          P4 elVec=baselineElectrons.at(iEl)->mom();
          if (elVec.deltaR_eta(jetVec)<0.2)overlap=true;
        }
        if (!overlap&&fabs(baselineJets.at(iJet)->eta())<2.8)goodJets.push_back(baselineJets.at(iJet));
        if (!overlap&&fabs(baselineJets.at(iJet)->eta())<2.8 && baselineJets.at(iJet)->pT()>20.)signalJets.push_back(baselineJets.at(iJet));
      }

      // Remove electrons with dR=0.4 or surviving jets
      for (size_t iEl=0;iEl<baselineElectrons.size();iEl++) {
        bool overlap=false;
        P4 elVec=baselineElectrons.at(iEl)->mom();
        for (size_t iJet=0;iJet<goodJets.size();iJet++) {
          P4 jetVec=goodJets.at(iJet)->mom();
          if (elVec.deltaR_eta(jetVec)<0.4)overlap=true;
        }
        if (!overlap && elVec.pT()>25.)signalElectrons.push_back(baselineElectrons.at(iEl));
        if(!overlap)electronsForVeto.push_back(baselineElectrons.at(iEl));
      }

      // Remove muons with dR=0.4 or surviving jets
      for (size_t iMu=0;iMu<baselineMuons.size();iMu++) {
        bool overlap=false;

        P4 muVec=baselineMuons.at(iMu)->mom();

        for (size_t iJet=0;iJet<goodJets.size();iJet++) {
          P4 jetVec=goodJets.at(iJet)->mom();
          if (muVec.deltaR_eta(jetVec)<0.4)overlap=true;
        }
        if (!overlap && muVec.pT()>25.)signalMuons.push_back(baselineMuons.at(iMu));
        if(!overlap)muonsForVeto.push_back(baselineMuons.at(iMu));
      }

      // We now have the signal electrons, muons, jets and b jets- move on to the analysis

      // Calculate common variables and cuts first
      int nJets = signalJets.size();

      //Jet cuts for each SR
      bool passSRAJetCut=false;
      bool passSRBJetCut=false;
      bool passSRAbJetCut=false;
      bool passSRBbJetCut=false;

      float mbb=0;
      float mCT=0;

      if(nJets>=2){
        if(signalJets[0]->pT() > 130.
           && signalJets[1]->pT() > 50) {
          if(nJets==2) {
            passSRAJetCut=true;
            if(signalJets[0]->isBJet() && signalJets[1]->isBJet()) {
              passSRAbJetCut=true;
              TLorentzVector bjet1;
              TLorentzVector bjet2;
              bjet1.SetPtEtaPhiE(signalJets[0]->pT(),signalJets[0]->eta(),signalJets[0]->phi(),signalJets[0]->E());
              bjet2.SetPtEtaPhiE(signalJets[1]->pT(),signalJets[1]->eta(),signalJets[1]->phi(),signalJets[1]->E());
              mbb = (bjet1+bjet2).M();

              float bjet1_ET = sqrt(bjet1.Pt()*bjet1.Pt()+bjet1.M()*bjet1.M());
              float bjet2_ET = sqrt(bjet2.Pt()*bjet2.Pt()+bjet2.M()*bjet2.M());
              TVector2 bjet1_pT;
              TVector2 bjet2_pT;
              bjet1_pT.Set(bjet1.Px(),bjet1.Py());
              bjet2_pT.Set(bjet2.Px(),bjet2.Py());
              double mct_squared = pow(bjet1_ET+bjet2_ET,2)-(bjet1_pT-bjet2_pT).Mod2();
              mCT = sqrt(mct_squared);
            }
          }
          if(nJets>2) {
            if(signalJets[2]->pT() < 50.) {
              passSRAJetCut=true;
              if(signalJets[0]->isBJet() && signalJets[1]->isBJet()) {
                passSRAbJetCut=true;
                TLorentzVector bjet1;
                TLorentzVector bjet2;
                bjet1.SetPtEtaPhiE(signalJets[0]->pT(),signalJets[0]->eta(),signalJets[0]->phi(),signalJets[0]->E());
                bjet2.SetPtEtaPhiE(signalJets[1]->pT(),signalJets[1]->eta(),signalJets[1]->phi(),signalJets[1]->E());
                mbb = (bjet1+bjet2).M();
                float bjet1_ET = sqrt(bjet1.Pt()*bjet1.Pt()+bjet1.M()*bjet1.M());
                float bjet2_ET = sqrt(bjet2.Pt()*bjet2.Pt()+bjet2.M()*bjet2.M());
                TVector2 bjet1_pT;
                TVector2 bjet2_pT;
                bjet1_pT.Set(bjet1.Px(),bjet1.Py());
                bjet2_pT.Set(bjet2.Px(),bjet2.Py());
                double mct_squared = pow(bjet1_ET+bjet2_ET,2)-(bjet1_pT-bjet2_pT).Mod2();
                mCT = sqrt(mct_squared);
              }
            }
          }
        }
      }

      if(nJets>=3){
        if(signalJets[0]->pT() > 150.
           && signalJets[1]->pT() > 30.
           && signalJets[2]->pT() > 30.) {
          passSRBJetCut=true;
          if(signalJets[1]->isBJet() && signalJets[2]->isBJet()) {
            passSRBbJetCut=true;
            TLorentzVector bjet1;
            TLorentzVector bjet2;
            bjet1.SetPtEtaPhiE(signalJets[1]->pT(),signalJets[1]->eta(),signalJets[1]->phi(),signalJets[1]->E());
            bjet2.SetPtEtaPhiE(signalJets[2]->pT(),signalJets[2]->eta(),signalJets[2]->phi(),signalJets[2]->E());
            mbb = (bjet1+bjet2).M();
            float bjet1_ET = sqrt(bjet1.Pt()*bjet1.Pt()+bjet1.M()*bjet1.M());
            float bjet2_ET = sqrt(bjet2.Pt()*bjet2.Pt()+bjet2.M()*bjet2.M());
            TVector2 bjet1_pT;
            TVector2 bjet2_pT;
            bjet1_pT.Set(bjet1.Px(),bjet1.Py());
            bjet2_pT.Set(bjet2.Px(),bjet2.Py());
            double mct_squared = pow(bjet1_ET+bjet2_ET,2)-(bjet1_pT-bjet2_pT).Mod2();
            mCT = sqrt(mct_squared);

          }
        }

      }

      //Calculate dphi(jet,met) for the three leading jets
      float dphi_jetmet1=9999;
      if(nJets>0)dphi_jetmet1=std::acos(std::cos(signalJets.at(0)->phi()-ptot.phi()));
      float dphi_jetmet2=9999;
      if(nJets>1)dphi_jetmet2=std::acos(std::cos(signalJets.at(1)->phi()-ptot.phi()));
      float dphi_jetmet3=9999;
      if(nJets>2)dphi_jetmet3=std::acos(std::cos(signalJets.at(2)->phi()-ptot.phi()));

      float dphi_min = min(dphi_jetmet1,dphi_jetmet2);
      dphi_min = min(dphi_min,dphi_jetmet3);

      //Calculate meff (all jets with pT>20 GeV, and met)
      float meff = met;
      for (Jet* jet : signalJets) {
        if(jet->pT()>20.)meff += jet->pT();
      }
      float meff2 = met; float meff3 = met; int nummeff=0;
      for (Jet* jet : signalJets) {
        nummeff++;
        if(nummeff<=2 && jet->pT()>20.)meff2 += jet->pT();
        if(nummeff<=3 && jet->pT()>20.)meff3 += jet->pT();
      }


      //Calculate HT,3 (all jets except 3 highest pT)
      float ht3 = 0; int num=0;
      for (Jet* jet : signalJets) {
        num++;
        if(num>3 && jet->pT()>20.)ht3 += jet->pT();
      }

      //Cutflow flags
      bool cut_ElectronVeto=false;
      bool cut_MuonVeto=false;
      //bool cut_2jets=false;
      //bool cut_3jets=false;
      //bool cut_dPhiJet1=false;
      bool cut_METGt150=false;
      bool cut_METGt250=false;
      bool cut_dPhiJets=false;
      bool cut_METmeff2=false;
      bool cut_METmeff3=false;

      if(electronsForVeto.size()==0)cut_ElectronVeto=true;
      if(muonsForVeto.size()==0)cut_MuonVeto=true;
      //if(passSRAJetCut)cut_2jets=true;
      //if(passSRBJetCut)cut_3jets=true;
      //if(dphi_jetmet1>2.5)cut_dPhiJet1=true;
      if(dphi_min>0.4)cut_dPhiJets=true;
      if(met>150.)cut_METGt150=true;
      if(met>250.)cut_METGt250=true;
      if(met/meff2>0.25)cut_METmeff2=true;
      if(met/meff3>0.25)cut_METmeff3=true;

      cutFlowVector_str[0] = "No cuts ";
      cutFlowVector_str[1] = "Lepton veto ";

      for(int j=0;j<NCUTS;j++){
        if(
           (j==0) ||

           (j==1 && cut_MuonVeto && cut_ElectronVeto) )

          cutFlowVector[j]++;
      }

      //We're now ready to apply the cuts for each signal region
      //_numSRA, _numSRB, _numSRA15, _numSRA20, _numSRA25, _numSRA30, _numSRA35;

      if(cut_ElectronVeto && cut_MuonVeto && cut_METGt150 && passSRAJetCut && passSRAbJetCut && cut_dPhiJets && cut_METmeff2 && mbb>200.) {
        _numSRA++;
        if(mCT>150.) _numSRA15++;
        if(mCT>200.) _numSRA20++;
        if(mCT>250.) _numSRA25++;
        if(mCT>300.) _numSRA30++;
        if(mCT>350.) _numSRA35++;
      }
      if(cut_ElectronVeto && cut_MuonVeto && cut_METGt250 && passSRBJetCut && passSRBbJetCut && cut_dPhiJets && cut_METmeff3 && ht3<50.) _numSRB ++;


      return;

    }

    void finalize() {

      using namespace std;

      double scale_to = 507.3;
      double trigger_cleaning_eff = 0.90;

      cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;
      cout << "NOT CHECKED - cut-flow not yet available via HEPData? "<<std::endl;
      cout << "------------------------------------------------------------------------------------------------------------------------------"<<std::endl;

      std::cout<< right << setw(40) << "CUT" << setw(20) << "RAW" << setw(20) << "SCALED" << setw(20) << "%" << setw(20) << "clean adj RAW"<< setw(20) << "clean adj %" << endl;
      for(int j=0; j<NCUTS; j++) {
        std::cout << right << setw(40) << cutFlowVector_str[j].c_str() << setw(20) << cutFlowVector[j] << setw(20) << cutFlowVector[j]*scale_to/cutFlowVector[0] << setw(20) << 100.*cutFlowVector[j]/cutFlowVector[0] << "%" << setw(20) << trigger_cleaning_eff*cutFlowVector[j]*scale_to/cutFlowVector[0] << setw(20) << trigger_cleaning_eff*100.*cutFlowVector[j]/cutFlowVector[0]<< "%" << endl;
      }
      cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;

      cout << "RESULTS 2B " << _numSRA << " "  << _numSRB << " " << _numSRA15 << " " <<  _numSRA20 << " " <<  _numSRA25 << " " << _numSRA30 << " " <<  _numSRA35 << endl;


    }


    double loglikelihood() {
      /// @todo Implement!
      return 0;
    }

  };


  DEFINE_ANAFACTORY(ATLAS_2bStop_20invfb)


}
