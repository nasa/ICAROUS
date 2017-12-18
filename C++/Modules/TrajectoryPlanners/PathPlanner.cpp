//
// Created by Swee Balachandran on 12/14/17.
//
#include <cstring>
#include "PathPlanner.h"

using namespace std;

PathPlanner::PathPlanner(FlightData* fd) {
    fdata = fd;
}

int64_t PathPlanner::FindPath(algorithm search, char *planID, double *fromPosition, double *toPosition,
                              double trk, double gs,double vs) {

    int64_t retval = -1;
    switch(search){

        case _ASTAR_:
            retval = FindPathAstar(planID,fromPosition,toPosition);
            break;
        case _RRT_:
            retval = FindPathRRT(planID,fromPosition,toPosition,trk,gs,vs);
            break;
        default:
            break;
    }

    return retval;
}


Plan PathPlanner::ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed){
    // Compute go above plan
    Plan ResolutionPlan2;
    double ETA = 0;
    double distH,distV;

    NavPoint nvpt1(start,ETA);
    ResolutionPlan2.addNavPoint(nvpt1);

    // Second waypoint directly above WP1
    Position wp2 = start.mkAlt(altFence+1);
    distV = wp2.distanceV(start);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt2(wp2,ETA);
    ResolutionPlan2.addNavPoint(nvpt2);

    // Third waypoint directly above exit point
    Position wp3 = goal.mkAlt(altFence+1);
    distH = wp3.distanceH(wp2);
    ETA = ETA + distH/rSpeed;
    NavPoint nvpt3(wp3,ETA);
    ResolutionPlan2.addNavPoint(nvpt3);

    // Final waypoint
    distV = goal.distanceH(wp3);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt4(goal,ETA);
    ResolutionPlan2.addNavPoint(nvpt4);

    return ResolutionPlan2;
}

void PathPlanner::GetWaypoint(char *planID, int wpID, double *waypoint) {
    for(Plan pl: outputPlans){
        if(strcmp(pl.getName().c_str(),planID)){
            continue;
        }else{
            Position pos = pl.getPos(wpID);
            waypoint[0] = pos.latitude();
            waypoint[1] = pos.longitude();
            waypoint[2] = pos.alt();
            break;
        }
    }
}