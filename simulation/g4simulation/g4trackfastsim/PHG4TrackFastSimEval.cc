/*!
 *  \file		PHG4TrackFastSimEval.cc
 *  \brief		Evaluation module for PHG4TrackFastSim output
 *  \details	input: PHG4TruthInfoContainer, SvtxTrackMap with SvtxTrack_FastSim inside
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

#include "PHG4TrackFastSimEval.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack_FastSim.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <phool/getClass.h>
#include <phool/phool.h>

#include <TH2.h>
#include <TTree.h>
#include <TVector3.h>

#include <cmath>
#include <iostream>
#include <map>      // for _Rb_tree_const_ite...
#include <utility>  // for pair

#define LogDebug(exp) std::cout << "DEBUG: " << __FILE__ << ": " << __LINE__ << ": " << exp << "\n"
#define LogError(exp) std::cout << "ERROR: " << __FILE__ << ": " << __LINE__ << ": " << exp << "\n"
#define LogWarning(exp) std::cout << "WARNING: " << __FILE__ << ": " << __LINE__ << ": " << exp << "\n"

using namespace std;

//----------------------------------------------------------------------------//
//-- Constructor:
//--  simple initialization
//----------------------------------------------------------------------------//
PHG4TrackFastSimEval::PHG4TrackFastSimEval(const string &name, const string &filename, const string &trackmapname)
  : SubsysReco(name)
  , _outfile_name(filename)
  , _trackmapname(trackmapname)
  , _event(0)
  , _flags(NONE)
  , _eval_tree_tracks(nullptr)
  , event(-1)
  , gtrackID(-1)
  , gflavor(0)
  , gpx(NAN)
  , gpy(NAN)
  , gpz(NAN)
  , gvx(NAN)
  , gvy(NAN)
  , gvz(NAN)
  , gvt(NAN)
  , trackID(-1)
  , charge(0)
  , nhits(-1)
  , px(NAN)
  , py(NAN)
  , pz(NAN)
  , pcax(NAN)
  , pcay(NAN)
  , pcaz(NAN)
  , dca2d(NAN)
  , _h2d_Delta_mom_vs_truth_mom(nullptr)
  , _h2d_Delta_mom_vs_truth_eta(nullptr)
  , _truth_container(nullptr)
  , _trackmap(nullptr)
{
}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int PHG4TrackFastSimEval::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Openning file " << _outfile_name << endl;
  PHTFileServer::get().open(_outfile_name, "RECREATE");

  // create TTree
  _eval_tree_tracks = new TTree("tracks", "FastSim Eval => tracks");
  _eval_tree_tracks->Branch("event", &event, "event/I");
  _eval_tree_tracks->Branch("gtrackID", &gtrackID, "gtrackID/I");
  _eval_tree_tracks->Branch("gflavor", &gflavor, "gflavor/I");
  _eval_tree_tracks->Branch("gpx", &gpx, "gpx/F");
  _eval_tree_tracks->Branch("gpy", &gpy, "gpy/F");
  _eval_tree_tracks->Branch("gpz", &gpz, "gpz/F");
  _eval_tree_tracks->Branch("gvx", &gvx, "gvx/F");
  _eval_tree_tracks->Branch("gvy", &gvy, "gvy/F");
  _eval_tree_tracks->Branch("gvz", &gvz, "gvz/F");
  _eval_tree_tracks->Branch("gvt", &gvt, "gvt/F");
  _eval_tree_tracks->Branch("trackID", &trackID, "trackID/I");
  _eval_tree_tracks->Branch("charge", &charge, "charge/I");
  _eval_tree_tracks->Branch("nhits", &nhits, "nhits/I");
  _eval_tree_tracks->Branch("px", &px, "px/F");
  _eval_tree_tracks->Branch("py", &py, "py/F");
  _eval_tree_tracks->Branch("pz", &pz, "pz/F");
  _eval_tree_tracks->Branch("pcax", &pcax, "pcax/F");
  _eval_tree_tracks->Branch("pcay", &pcay, "pcay/F");
  _eval_tree_tracks->Branch("pcaz", &pcaz, "pcaz/F");
  _eval_tree_tracks->Branch("dca2d", &dca2d, "dca2d/F");

  _h2d_Delta_mom_vs_truth_eta = new TH2D("_h2d_Delta_mom_vs_truth_eta",
                                         "#frac{#Delta p}{truth p} vs. truth #eta", 54, -4.5, +4.5, 1000, -1,
                                         1);

  _h2d_Delta_mom_vs_truth_mom = new TH2D("_h2d_Delta_mom_vs_truth_mom",
                                         "#frac{#Delta p}{truth p} vs. truth p", 41, -0.5, 40.5, 1000, -1,
                                         1);

  return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//
int PHG4TrackFastSimEval::process_event(PHCompositeNode *topNode)
{
  _event++;
  if (Verbosity() >= 2 and _event % 1000 == 0)
    cout << PHWHERE << "Events processed: " << _event << endl;

  //std::cout << "Opening nodes" << std::endl;
  GetNodes(topNode);

  //std::cout << "Filling trees" << std::endl;
  fill_tree(topNode);
  //std::cout << "DONE" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//
int PHG4TrackFastSimEval::End(PHCompositeNode *topNode)
{
  PHTFileServer::get().cd(_outfile_name);

  _eval_tree_tracks->Write();

  _h2d_Delta_mom_vs_truth_eta->Write();
  _h2d_Delta_mom_vs_truth_mom->Write();

  //PHTFileServer::get().close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- fill_tree():
//--   Fill the trees with truth, track fit, and cluster information
//----------------------------------------------------------------------------//
void PHG4TrackFastSimEval::fill_tree(PHCompositeNode *topNode)
{
  // Make sure to reset all the TTree variables before trying to set them.
  reset_variables();
  //std::cout << "A1" << std::endl;
  event = _event;

  if (!_truth_container)
  {
    LogError("_truth_container not found!");
    return;
  }

  if (!_trackmap)
  {
    LogError("_trackmap not found!");
    return;
  }

  PHG4TruthInfoContainer::ConstRange range =
      _truth_container->GetPrimaryParticleRange();
  //std::cout << "A2" << std::endl;
  for (PHG4TruthInfoContainer::ConstIterator truth_itr = range.first;
       truth_itr != range.second; ++truth_itr)
  {
    PHG4Particle *g4particle = truth_itr->second;
    if (!g4particle)
    {
      LogDebug("");
      continue;
    }
    //std::cout << "B1" << std::endl;

    SvtxTrack_FastSim *track = nullptr;

    //std::cout << "TRACKmap size " << _trackmap->size() << std::endl;
    for (SvtxTrackMap::ConstIter track_itr = _trackmap->begin();
         track_itr != _trackmap->end();
         track_itr++)
    {
      //std::cout << "TRACK * " << track_itr->first << std::endl;
      SvtxTrack_FastSim *temp = dynamic_cast<SvtxTrack_FastSim *>(track_itr->second);
      if (!temp)
      {
        std::cout << "ERROR CASTING PARTICLE!" << std::endl;
        continue;
      }
      //std::cout << " PARTICLE!" << std::endl;

      if ((temp->get_truth_track_id() - g4particle->get_track_id()) == 0)
      {
        track = temp;
      }
    }

    //std::cout << "B2" << std::endl;
    gtrackID = g4particle->get_track_id();
    gflavor = g4particle->get_pid();

    gpx = g4particle->get_px();
    gpy = g4particle->get_py();
    gpz = g4particle->get_pz();

    PHG4VtxPoint *vtx = _truth_container->GetVtx(g4particle->get_vtx_id());
    if (vtx)
    {
      gvx = vtx->get_x();
      gvy = vtx->get_y();
      gvz = vtx->get_z();
      gvt = vtx->get_t();
    }

    if (track)
    {
      //std::cout << "C1" << std::endl;
      trackID = track->get_id();
      charge = track->get_charge();
      nhits = track->size_clusters();

      px = track->get_px();
      py = track->get_py();
      pz = track->get_pz();
      pcax = track->get_x();
      pcay = track->get_y();
      pcaz = track->get_z();
      dca2d = track->get_dca2d();

      TVector3 truth_mom(gpx, gpy, gpz);
      TVector3 reco_mom(px, py, pz);
      //std::cout << "C2" << std::endl;

      _h2d_Delta_mom_vs_truth_mom->Fill(truth_mom.Mag(), (reco_mom.Mag() - truth_mom.Mag()) / truth_mom.Mag());
      _h2d_Delta_mom_vs_truth_eta->Fill(truth_mom.Eta(), (reco_mom.Mag() - truth_mom.Mag()) / truth_mom.Mag());
    }
    //std::cout << "B3" << std::endl;

    _eval_tree_tracks->Fill();
  }
  //std::cout << "A3" << std::endl;

  return;
}

//----------------------------------------------------------------------------//
//-- reset_variables():
//--   Reset all the tree variables to their default values.
//--   Needs to be called at the start of every event
//----------------------------------------------------------------------------//
void PHG4TrackFastSimEval::reset_variables()
{
  event = -9999;

  //-- truth
  gtrackID = -9999;
  gflavor = -9999;
  gpx = -9999;
  gpy = -9999;
  gpz = -9999;
  gvx = -9999;
  gvy = -9999;
  gvz = -9999;

  //-- reco
  trackID = -9999;
  charge = -9999;
  nhits = -9999;
  px = -9999;
  py = -9999;
  pz = -9999;
  dca2d = -9999;
}

//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
int PHG4TrackFastSimEval::GetNodes(PHCompositeNode *topNode)
{
  //DST objects
  //Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
                                                                "G4TruthInfo");
  if (!_truth_container && _event < 2)
  {
    cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree"
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode,
                                               _trackmapname);
  //std::cout << _trackmapname.c_str() << std::endl;
  if (!_trackmap && _event < 2)
  {
    cout << PHWHERE << "SvtxTrackMap node with name "
         << _trackmapname
         << " not found on node tree"
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}
