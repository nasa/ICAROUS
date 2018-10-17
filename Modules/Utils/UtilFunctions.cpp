//
// Created by swee on 6/24/18.
//

#include "UtilFunctions.h"
#include <Position.h>
#include <Velocity.h>
#include <math.h>
#include <EuclideanProjection.h>
#include <Projection.h>

using namespace larcfm;

double ComputeDistance(double positionA[],double positionB[]){

    Position A = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m/s");
    Position B = Position::makeLatLonAlt(positionB[0],"degree",positionB[1],"degree",positionB[2],"m/s");

    return A.distanceH(B);
}

void ConvertVnedToTrkGsVs(double vn,double ve,double vz,double *Trk,double *Gs,double *Vs){
   double angle = 360 + atan2(ve, vn) * 180 / M_PI;
   *Trk = fmod(angle, 360);
   *Gs = sqrt(pow(vn, 2) + pow(ve, 2));
   *Vs = vz;
}

void ConvertTrkGsVsToVned(double Trk,double Gs,double Vs,double *vn,double *ve,double *vd){
    Velocity vel = Velocity::makeTrkGsVs(Trk, "degree", Gs, "m/s", Vs, "m/s");
    *vn = vel.y;
    *ve = vel.x;
    *vd = vel.z;
}

bool CheckTurnConflict(double low, double high, double newHeading, double oldHeading) {
    if(newHeading < 0){
        newHeading = 360 + newHeading;
    }

    if(oldHeading < 0){
        oldHeading = 360 + oldHeading;
    }

    // Get direction of turn
    double psi   = newHeading - oldHeading;
    double psi_c = 360 - std::abs(psi);
    bool rightTurn = false;
    if(psi > 0){
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = false;
        }
        else{
            rightTurn = true;
        }
    }else{
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = true;
        }
        else{
            rightTurn = false;
        }
    }

    double A,B,X,Y,diff;
    if(rightTurn){
        diff = oldHeading;
        A = oldHeading - diff;
        B = newHeading - diff;
        X = low - diff;
        Y = high - diff;

        if(B < 0){
            B = 360 + B;
        }

        if(X < 0){
            X = 360 + X;
        }

        if(Y < 0){
            Y = 360 + Y;
        }

        if(A < X && B > Y){
            return true;
        }
    }else{
        diff = 360 - oldHeading;
        A    = oldHeading + diff;
        B    = newHeading + diff;
        X = low + diff;
        Y = high + diff;

        if(B > 360){
            B = B - 360;
        }

        if(X > 360){
            X = X - 360;
        }

        if(Y > 360){
            Y = Y - 360;
        }

        if(A > Y && B < X){
            return true;
        }
    }

    return false;
}

void ComputeLatLngAlt(double origin[],double xyz[],double output[]){
    Position CurrentPos = Position::makeLatLonAlt(origin[0],"degree",origin[1],"degree",origin[2],"m");
    EuclideanProjection proj = Projection::createProjection(CurrentPos);
    const Vect3 cartPos(xyz[0],xyz[1],xyz[2]);
    LatLonAlt transformedPos = proj.inverse(cartPos);
    output[0] = transformedPos.latitude();
    output[1] = transformedPos.longitude();
    output[2] = transformedPos.alt();
}


void ComputeTrackingResolution(double targetPos[],double currentPos[],double currentVel[],double heading,double distH,double distV,
                                      double PropGains[],double outputVel[],double* outputHeading){

    // Get coordinates of object to track
    Position CurrentPos = Position::makeLatLonAlt(currentPos[0],"degree",currentPos[1],"degree",currentPos[2],"m");
    Position Target     = Position::makeLatLonAlt(targetPos[0],"degree",targetPos[1],"degree",targetPos[2],"m");
    double distHx  = distH*sin(heading *M_PI/180); // Heading is measured from North
    double distHy  = distH*cos(heading *M_PI/180);

    double Kx_trk = PropGains[0];
    double Ky_trk = PropGains[1];
    double Kz_trk = PropGains[2];

    Vect3 delPos(distHx,distHy,distV);

    // Project from LatLonAlt to cartesian coordinates

    EuclideanProjection proj = Projection::createProjection(CurrentPos.mkAlt(0));
    Vect3 vecCP  =  proj.project(CurrentPos);
    Vect3 vecTP  =  proj.project(Target);

    Vect3 vecTPf =  vecTP.Add(delPos);

    // Relative vector to object
    Vect3 Rel    = vecTPf.Sub(vecCP);

    // Compute velocity commands that will enure smooth tracking of object
    double dx = Rel.x;
    double dy = Rel.y;
    double dz = Rel.z;

    // Velocity is proportional to distance from object.
    double Vx = SaturateVelocity(Kx_trk * dx,2.0);
    double Vy = SaturateVelocity(Ky_trk * dy,2.0);
    double Vz = SaturateVelocity(Kz_trk * dz,2.0);

    //printf("vecCP:%f,%f,%f\n",vecCP.x,vecCP.y,vecCP.z);
    //printf("vecTPf:%f,%f,%f\n",vecTPf.x,vecTPf.y,vecTPf.z);
    //printf("dx,dy,dz = %f,%f,%f\n",dx,dy,dz);

    double RefHeading = atan2(Vx,Vy) * 180/M_PI;

    if(RefHeading < 0){
        RefHeading = 360 + RefHeading;
    }

    outputVel[0] = Vy;
    outputVel[1] = Vx;
    outputVel[2] = -Vz;

    *outputHeading = RefHeading;

    //printf("Heading = %f, Velocities %1.3f, %1.3f, %1.3f\n",heading,Vx,Vy,Vz);
}

double SaturateVelocity(double V, double Vsat){
    if(abs(V) > Vsat){
        return sign(V)*Vsat;
    }
    else{
        return V;
    }
}

