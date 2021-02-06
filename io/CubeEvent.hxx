#ifndef CubeEvent_hxx_seen
#define CubeEvent_hxx_seen
#include "CubeAlgorithmResult.hxx"
#include "CubeG4Hit.hxx"
#include "CubeG4Trajectory.hxx"

#include <TRef.h>
#include <TROOT.h>

#include <map>

namespace Cube {
    class Event;
}

/// An event processed through the Cube reconstruction.
class Cube::Event : public Cube::AlgorithmResult {
public:
    Event(void);
    virtual ~Event();

    static Cube::Event* CurrentEvent();
    void MakeCurrentEvent() const;

    virtual void Initialize(int run = -1, int event = -1,
                            TObject* evt = NULL) {
        SetName("CubeEvent");
        SetTitle("Hits generated by ERepSim");
        fRunId = run;
        fEventId = event;
        fEDepSimEvent = TRef(evt);
        G4Hits.clear();
        G4Trajectories.clear();
        Cube::AlgorithmResult::Initialize();
    }

    /// The run number for this event.
    int GetRunId() const {return fRunId;}

    /// The event number.
    int GetEventId() const {return fEventId;}

    void ls(Option_t *opt = "") const {
        TROOT::IndentLevel();
        std::cout << "%%% Event : " << fRunId << " / " << fEventId << std::endl;
        TROOT::IncreaseDirLevel();
        fEDepSimEvent.ls();
        Cube::AlgorithmResult::ls(opt);
        TROOT::DecreaseDirLevel();
    }

private:

    /// The run number
    int fRunId;

    /// The event number.
    int fEventId;

    /// The parent EDepSim event (if available).
    TRef fEDepSimEvent;

public:
    /////////////////////////////////////////////////////////////////////////
    /// The stuff below this comment "doesn't exist".  This is a cheaters way
    /// to get some of the important truth information needed to debug the
    /// reconstruction.
    ////////////////////////////////////////////////////////////////////////

    /// All of the TG4HitSegments in the event.  The index is the segment id,
    /// which is a unique (and fairly arbitrary) value for each segment.  The
    /// value is the actual hit segment.  The segment id can be "mostly"
    /// ignored.
    typedef std::map<int,Cube::Handle<Cube::G4Hit>> G4HitContainer;
    G4HitContainer G4Hits;

    /// All of the trajectory information.  The index is the trajectory
    /// identifier so that you can easily find a trajectory.
    typedef std::map<int,Cube::Handle<Cube::G4Trajectory>>
    G4TrajectoryContainer;
    G4TrajectoryContainer G4Trajectories;

    ClassDef(Event,1);
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/cube-build.sh force"
// End:
