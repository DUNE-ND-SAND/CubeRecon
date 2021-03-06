#include "ERepSimInput.hxx"
#include "CubeERepSim.hxx"
#include "CubeUnits.hxx"
#include "CubeInfo.hxx"
#include "CubeG4Hit.hxx"
#include "CubeLog.hxx"

namespace {
    void Fill3DST(Cube::Event& event, int h,
                  Cube::WritableHit& wHit) {
        double extraFiberLength
            = (*ERepSim::Input::Get().Property)[
                "3DST.Response.Atten.SensorDist"]
            + (*ERepSim::Input::Get().Property)[
                "3DST.Response.Atten.MirrorDist"];

        /// Get the information from the ERepSim Properties
        double lo =(*ERepSim::Input::Get().Property)["3DST.Response.CubeMin"];
        double hi =(*ERepSim::Input::Get().Property)["3DST.Response.CubeMax"];
        int countX =(*ERepSim::Input::Get().Property)["3DST.Response.Cubes"];
        double pitchX = (hi-lo)/(countX-1.0);
        double lengthX = pitchX*countX + extraFiberLength;

        lo =(*ERepSim::Input::Get().Property)["3DST.Response.BarMin"];
        hi =(*ERepSim::Input::Get().Property)["3DST.Response.BarMax"];
        int countY =(*ERepSim::Input::Get().Property)["3DST.Response.Bars"];
        double pitchY = (hi-lo)/(countY-1.0);
        double lengthY = pitchY*countY + extraFiberLength;

        lo =(*ERepSim::Input::Get().Property)["3DST.Response.PlaneMin"];
        hi =(*ERepSim::Input::Get().Property)["3DST.Response.PlaneMax"];
        int countZ =(*ERepSim::Input::Get().Property)["3DST.Response.Planes"];
        double pitchZ = (hi-lo)/(countZ-1.0);
        double lengthZ = pitchZ*countZ + extraFiberLength;

        double ratio12
            = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Ratio12"];
        double atten1
            = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Tau1"];
        double atten2
            = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Tau2"];
        double reflect
            = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Reflect"];

        int id = (*ERepSim::Input::Get().HitSensorId)[h];
        if (!Cube::Info::Is3DST(id)) {
            throw std::runtime_error("Not the 3DST");
        }

        int det = Cube::Info::SubDetector(id);
        int cube = Cube::Info::CubeNumber(id);
        int bar = Cube::Info::CubeBar(id);
        int pln = Cube::Info::CubePlane(id);
        wHit.SetIdentifier(id);
        wHit.SetPosition(
            TVector3((*ERepSim::Input::Get().HitX)[h],
                     (*ERepSim::Input::Get().HitY)[h],
                     (*ERepSim::Input::Get().HitZ)[h]));
        TVector3 size(pitchX, pitchY, pitchZ);
        if (cube < 0) {
            size.SetX(lengthX);
        }
        if (bar < 0) {
            size.SetY(lengthY);
        }
        if (pln < 0) {
            size.SetZ(lengthZ);
        }
        wHit.SetUncertainty(0.289*size);
        wHit.SetSize(0.5*size);
        wHit.SetTime((*ERepSim::Input::Get().HitTime)[h]);
        wHit.SetTimeUncertainty((*ERepSim::Input::Get().HitTimeWidth)[h]);
        wHit.SetCharge((*ERepSim::Input::Get().HitCharge)[h]);
        int segBegin = (*ERepSim::Input::Get().HitSegmentBegin)[h];
        int segEnd = (*ERepSim::Input::Get().HitSegmentEnd)[h];
        for (int seg = segBegin; seg < segEnd; ++seg) {
            int s = (*ERepSim::Input::Get().SegmentIds)[seg];
            wHit.AddContributor(s);
        }
        wHit.SetProperty("Ratio12",ratio12);
        wHit.SetProperty("Atten1",atten1);
        wHit.SetProperty("Atten2",atten2);
        wHit.SetProperty("Reflectivity",reflect);
#ifdef DOUBLE_CHECK_OUTPUT
#undef DOUBLE_CHECK_OUTPUT
        CUBE_LOG(0) << "3DST Hit  "
                    << " x " << wHit.GetPosition().X()
                    << " y " << wHit.GetPosition().Y()
                    << " z " << wHit.GetPosition().Z()
                    << " sx " << wHit.GetSize().X()
                    << " sy " << wHit.GetSize().Y()
                    << " sz " << wHit.GetSize().Z()
                    << " t " << wHit.GetTime()
                    << " q " << wHit.GetCharge()
                    << " c " << wHit.GetContributorCount()
                    << std::endl;
#endif
    }

    void FillTPC(Cube::Event& event, int h,
                  Cube::WritableHit& wHit) {

        double pitchX = 10.0;
        double pitchY = 10.0;
        double pitchZ = 10.0;

        int id = (*ERepSim::Input::Get().HitSensorId)[h];
        if (!Cube::Info::IsTPC(id)) {
            throw std::runtime_error("Not the TPC");
        }

        int det = Cube::Info::SubDetector(id);
        int anode = Cube::Info::TPCAnode(id);
        const double vdrift = 0.078; // mm/ns
        if (anode == 0) wHit.SetProperty("DriftVelocity",-vdrift);
        else if (anode == 1) wHit.SetProperty("DriftVelocity",vdrift);

        wHit.SetIdentifier(id);
        wHit.SetPosition(
            TVector3((*ERepSim::Input::Get().HitX)[h],
                     (*ERepSim::Input::Get().HitY)[h],
                     (*ERepSim::Input::Get().HitZ)[h]));
        wHit.SetTime((*ERepSim::Input::Get().HitTime)[h]);
        wHit.SetTimeUncertainty((*ERepSim::Input::Get().HitTimeWidth)[h]);
        wHit.SetCharge((*ERepSim::Input::Get().HitCharge)[h]);
        pitchX = 2.0*2.0*vdrift*wHit.GetTimeUncertainty();
        TVector3 size(pitchX, pitchY, pitchZ);
        wHit.SetUncertainty(0.289*size);
        wHit.SetSize(0.5*size);
        int segBegin = (*ERepSim::Input::Get().HitSegmentBegin)[h];
        int segEnd = (*ERepSim::Input::Get().HitSegmentEnd)[h];
        for (int seg = segBegin; seg < segEnd; ++seg) {
            int s = (*ERepSim::Input::Get().SegmentIds)[seg];
            wHit.AddContributor(s);
        }
#ifdef  DOUBLE_CHECK_OUTPUT
#undef  DOUBLE_CHECK_OUTPUT
        CUBE_LOG(0) << "TPC Hit  "
                    << " x " << wHit.GetPosition().X()
                    << " y " << wHit.GetPosition().Y()
                    << " z " << wHit.GetPosition().Z()
                    << " sx " << wHit.GetSize().X()
                    << " sy " << wHit.GetSize().Y()
                    << " sz " << wHit.GetSize().Z()
                    << " t " << wHit.GetTime()
                    << " q " << wHit.GetCharge()
                    << " c " << wHit.GetContributorCount()
                    << std::endl;
#endif
    }

    void FillECal(Cube::Event& event, int h,
                  Cube::WritableHit& wHit) {

        int id = (*ERepSim::Input::Get().HitSensorId)[h];
        if (!Cube::Info::IsECal(id)) {
            throw std::runtime_error("Not the ECal");
        }

        double tWidth = (*ERepSim::Input::Get().HitTimeWidth)[h];
        double vphoton = 17.09*unit::cm/unit::ns;

        double pitchX = 40.0*unit::mm;
        double pitchY = 40.0*unit::mm;
        double pitchZ = 40.0*unit::mm;

        if (!Cube::Info::ECalEnd(id)) wHit.SetProperty("Velocity",-vphoton);
        else wHit.SetProperty("Velocity",vphoton);

        if (Cube::Info::ECalModule(id) < 30) {
            // A barrel cell.
            pitchX = vphoton*tWidth;
        }
        else {
            // An endcap
            pitchY = vphoton*tWidth;
        }

        wHit.SetIdentifier(id);
        wHit.SetPosition(
            TVector3((*ERepSim::Input::Get().HitX)[h],
                     (*ERepSim::Input::Get().HitY)[h],
                     (*ERepSim::Input::Get().HitZ)[h]));
        wHit.SetTime((*ERepSim::Input::Get().HitTime)[h]);
        wHit.SetTimeUncertainty(1.0*unit::ns);
        wHit.SetCharge((*ERepSim::Input::Get().HitCharge)[h]);
        TVector3 size(pitchX, pitchY, pitchZ);
        wHit.SetUncertainty(0.289*size);
        wHit.SetSize(0.5*size);
        int segBegin = (*ERepSim::Input::Get().HitSegmentBegin)[h];
        int segEnd = (*ERepSim::Input::Get().HitSegmentEnd)[h];
        for (int seg = segBegin; seg < segEnd; ++seg) {
            int s = (*ERepSim::Input::Get().SegmentIds)[seg];
            wHit.AddContributor(s);
        }
#ifdef  DOUBLE_CHECK_OUTPUT
#undef  DOUBLE_CHECK_OUTPUT
        CUBE_LOG(0) << "ECAL Hit  "
                    << " x " << wHit.GetPosition().X()
                    << " y " << wHit.GetPosition().Y()
                    << " z " << wHit.GetPosition().Z()
                    << " sx " << wHit.GetSize().X()
                    << " sy " << wHit.GetSize().Y()
                    << " sz " << wHit.GetSize().Z()
                    << " t " << wHit.GetTime()
                    << " q " << wHit.GetCharge()
                    << " c " << wHit.GetContributorCount()
                    << std::endl;
#endif
    }
}

/// A function to convert data in an ERepSim tree into a reconstruction event.
/// This assumes that the GetEntry has been called for all of the necessary
/// trees.
void Cube::ConvertERepSim(Cube::Event& event) {
    event.Initialize(ERepSim::Input::Get().RunId,
                     ERepSim::Input::Get().EventId,
                     NULL);

    double extraFiberLength
        = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.SensorDist"]
        + (*ERepSim::Input::Get().Property)["3DST.Response.Atten.MirrorDist"];

    /// Get the information from the ERepSim Properties
    double lo =(*ERepSim::Input::Get().Property)["3DST.Response.CubeMin"];
    double hi =(*ERepSim::Input::Get().Property)["3DST.Response.CubeMax"];
    int countX =(*ERepSim::Input::Get().Property)["3DST.Response.Cubes"];
    double pitchX = (hi-lo)/(countX-1.0);
    double lengthX = pitchX*countX + extraFiberLength;

    lo =(*ERepSim::Input::Get().Property)["3DST.Response.BarMin"];
    hi =(*ERepSim::Input::Get().Property)["3DST.Response.BarMax"];
    int countY =(*ERepSim::Input::Get().Property)["3DST.Response.Bars"];
    double pitchY = (hi-lo)/(countY-1.0);
    double lengthY = pitchY*countY + extraFiberLength;

    lo =(*ERepSim::Input::Get().Property)["3DST.Response.PlaneMin"];
    hi =(*ERepSim::Input::Get().Property)["3DST.Response.PlaneMax"];
    int countZ =(*ERepSim::Input::Get().Property)["3DST.Response.Planes"];
    double pitchZ = (hi-lo)/(countZ-1.0);
    double lengthZ = pitchZ*countZ + extraFiberLength;

    double ratio12
        = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Ratio12"];
    double atten1
        = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Tau1"];
    double atten2
        = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Tau2"];
    double reflect
        = (*ERepSim::Input::Get().Property)["3DST.Response.Atten.Reflect"];

    CUBE_LOG(0) << "3DST with " << countX
              << " x " << countY
              << " x " << countZ
              << " cubes"
              << std::endl;
    CUBE_LOG(0) << "     Pitch is " << pitchX
              << " x " << pitchY
              << " x " << pitchZ
              << " mm"
              << std::endl;
    CUBE_LOG(0) << "     Attenuation is "
              << atten1 << " mm (" << ratio12 << ")"
              << ", plus " << atten2 << " mm (" << 1.0-ratio12 << ")"
              << " and reflectivity of " << reflect
              << std::endl;

    /// Add the trajectories to the event.
    for (int s = 0; s < ERepSim::Input::Get().TrajectoryId->size(); ++s) {
        Cube::Handle<Cube::G4Trajectory> traj(new Cube::G4Trajectory);
        traj->SetTrackId((*ERepSim::Input::Get().TrajectoryId)[s]);
        traj->SetParentId((*ERepSim::Input::Get().TrajectoryParent)[s]);
        traj->SetPDGCode((*ERepSim::Input::Get().TrajectoryPDG)[s]);
        traj->SetInitialPosition(
            TLorentzVector(
                (*ERepSim::Input::Get().TrajectoryX)[s],
                (*ERepSim::Input::Get().TrajectoryY)[s],
                (*ERepSim::Input::Get().TrajectoryZ)[s],
                (*ERepSim::Input::Get().TrajectoryT)[s]));
        traj->SetInitialMomentum(
            TLorentzVector(
                (*ERepSim::Input::Get().TrajectoryPx)[s],
                (*ERepSim::Input::Get().TrajectoryPy)[s],
                (*ERepSim::Input::Get().TrajectoryPz)[s],
                (*ERepSim::Input::Get().TrajectoryPe)[s]));
        event.G4Trajectories[traj->GetTrackId()] = traj;
    }

    int xzHits = 0;
    int yzHits = 0;
    int xyHits = 0;

    // Save the hit segment information!
    std::map<int, Cube::Handle<Cube::G4Hit>> segmentMap;
    for (int s = 0; s < ERepSim::Input::Get().SegmentIds->size(); ++s) {
        int sid = (*ERepSim::Input::Get().SegmentIds)[s];
        std::map<int, Cube::Handle<Cube::G4Hit>>::iterator entry
            = segmentMap.find(sid);
        if (entry != segmentMap.end()) {
            continue;
        }
        Cube::Handle<Cube::G4Hit> seg(new Cube::G4Hit);
        segmentMap[sid] = seg;
        seg->SetSegmentId(sid);
        int trackId = (*ERepSim::Input::Get().SegmentTrackId)[s];
        Cube::Event::G4TrajectoryContainer::iterator traj
            = event.G4Trajectories.find(trackId);
        if (traj != event.G4Trajectories.end()) {
            int pdg = 0;
            if (traj->second) {
                pdg = traj->second->GetPDGCode();
            }
            else {
                CUBE_ERROR << "Trajectory id without a trajectory pointer."
                           << trackId
                           << std::endl;
            }
            seg->SetPrimaryId(trackId);
            seg->SetPDG(pdg);
        }
        else {
            CUBE_ERROR << "Segment for non existing trajectory id: "
                       << trackId
                       << std::endl;
        }
        seg->SetEnergyDeposit((*ERepSim::Input::Get().SegmentEnergy)[s]);
        seg->SetStart(
            TLorentzVector(
                (*ERepSim::Input::Get().SegmentX1)[s],
                (*ERepSim::Input::Get().SegmentY1)[s],
                (*ERepSim::Input::Get().SegmentZ1)[s],
                (*ERepSim::Input::Get().SegmentT)[s]));
        seg->SetStop(
            TLorentzVector(
                (*ERepSim::Input::Get().SegmentX2)[s],
                (*ERepSim::Input::Get().SegmentY2)[s],
                (*ERepSim::Input::Get().SegmentZ2)[s],
                (*ERepSim::Input::Get().SegmentT)[s]));
    }

    /// Add the segments to the event.  The copy is so we could change the
    /// data format later.
    for (std::map<int, Cube::Handle<Cube::G4Hit>>::iterator s
             = segmentMap.begin();
         s != segmentMap.end(); ++s) {
        event.G4Hits[s->first] = s->second;
    }

    /// Get the hits out of the ERepSim trees.
    int hits3DST = 0;
    int hitsTPC = 0;
    int hitsECal = 0;
    Cube::Handle<Cube::HitSelection> hits(new Cube::HitSelection("Raw"));
    int hitsUnknown = 0;
    for (std::size_t h = 0;
         h < ERepSim::Input::Get().HitSensorId->size(); ++h) {
        Cube::WritableHit wHit;
        int id = (*ERepSim::Input::Get().HitSensorId)[h];
        int det = Cube::Info::SubDetector(id);
        if (Cube::Info::Is3DST(id)) {
            ++hits3DST;
            Fill3DST(event,h,wHit);
        }
        else if (Cube::Info::IsTPC(id)) {
            ++hitsTPC;
            FillTPC(event,h,wHit);
        }
        else if (Cube::Info::IsECal(id)) {
            ++hitsECal;
            FillECal(event,h,wHit);
        }
        else {
            ++hitsUnknown;
            continue;
        }
        Cube::Handle<Cube::Hit> hit(new Cube::Hit(wHit));
        hits->push_back(hit);
    }
    event.AddHitSelection(hits);

    CUBE_LOG(0) << event.GetName() << ": " << event.GetRunId()
                << "/" << event.GetEventId()
                << " -- hits: " << hits->size()
                << " 3DST: " << hits3DST
                << " TPC: " << hitsTPC
                << " ECal: " << hitsECal
                << " Unknown: " << hitsUnknown
              << std::endl;

}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/cube-build.sh force"
// End:
