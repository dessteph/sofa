/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/component/collision/detection/intersection/MeshMinProximityIntersection.h>

#include <sofa/component/collision/detection/intersection/DiscreteIntersection.h>
#include <sofa/core/collision/Intersection.inl>
#include <sofa/core/collision/IntersectorFactory.h>


namespace sofa::component::collision::detection::intersection
{

using namespace sofa::type;
using namespace sofa::defaulttype;
using namespace sofa::core::collision;
using namespace sofa::component::collision::geometry;

IntersectorCreator<MinProximityIntersection, MeshMinProximityIntersection> MeshMinProximityIntersectors("Mesh");

MeshMinProximityIntersection::MeshMinProximityIntersection(MinProximityIntersection* object, bool addSelf)
    : intersection(object)
{
    if (addSelf)
    {
        if (intersection->usePointPoint.getValue())
            intersection->intersectors.add<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
        else
            intersection->intersectors.ignore<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>();

        if(intersection->useLinePoint.getValue())
            intersection->intersectors.add<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
        else
            intersection->intersectors.ignore<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>();

        if(intersection->useLineLine.getValue())
            intersection->intersectors.add<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
        else
            intersection->intersectors.ignore<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>>();

        intersection->intersectors.add<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
        intersection->intersectors.ignore<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>>();
        intersection->intersectors.ignore<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>>();

        if (intersection->useSphereTriangle.getValue())
        {
            intersection->intersectors.add<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
            intersection->intersectors.add<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
            intersection->intersectors.add<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
            intersection->intersectors.add<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel, MeshMinProximityIntersection>(this);
            intersection->intersectors.add<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>, MeshMinProximityIntersection>(this);
            intersection->intersectors.add<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel, MeshMinProximityIntersection>(this);
        }
        else
        {
            intersection->intersectors.ignore<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>>();
            intersection->intersectors.ignore<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>>();
            intersection->intersectors.ignore<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>();
            intersection->intersectors.ignore<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel>();
            intersection->intersectors.ignore<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>>();
            intersection->intersectors.ignore<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel>();
        }
    }
}

bool MeshMinProximityIntersection::testIntersection(Line& e1, Line& e2)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    const Vector3 AB = e1.p2()-e1.p1();
    const Vector3 CD = e2.p2()-e2.p1();
    const Vector3 AC = e2.p1()-e1.p1();
    Matrix2 A;
    Vector2 b;

    A[0][0] = AB*AB;
    A[1][1] = CD*CD;
    A[0][1] = A[1][0] = -CD*AB;
    b[0] = AB*AC;
    b[1] = -CD*AC;

    const SReal det = type::determinant(A);

    SReal alpha = 0.5;
    SReal beta = 0.5;

    if (det < -1.0e-18 || det > 1.0e-18)
    {
        alpha = (b[0]*A[1][1] - b[1]*A[0][1])/det;
        beta  = (b[1]*A[0][0] - b[0]*A[1][0])/det;
        if (alpha < 0.000001 || alpha > 0.999999 ||
            beta  < 0.000001 || beta  > 0.999999 )
            return false;
    }

    const Vector3 PQ = AC + CD * beta - AB * alpha;

    return PQ.norm2() < alarmDist * alarmDist;
}

int MeshMinProximityIntersection::computeIntersection(Line& e1, Line& e2, OutputVector* contacts)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    const Vector3 AB = e1.p2()-e1.p1();
    const Vector3 CD = e2.p2()-e2.p1();
    const Vector3 AC = e2.p1()-e1.p1();
    Matrix2 A;
    Vector2 b;

    A[0][0] = AB*AB;
    A[1][1] = CD*CD;
    A[0][1] = A[1][0] = -CD*AB;
    b[0] = AB*AC;
    b[1] = -CD*AC;
    const SReal det = type::determinant(A);

    SReal alpha = 0.5;
    SReal beta = 0.5;

    if (det < -1.0e-15 || det > 1.0e-15)
    {
        alpha = (b[0]*A[1][1] - b[1]*A[0][1])/det;
        beta  = (b[1]*A[0][0] - b[0]*A[1][0])/det;
        if (alpha < 0.000001 || alpha > 0.999999 ||
            beta  < 0.000001 || beta  > 0.999999 )
            return 0;
    }

    Vector3 P,Q,PQ;
    P = e1.p1() + AB * alpha;
    Q = e2.p1() + CD * beta;

    PQ  = Q - P;
    if (PQ.norm2() >= alarmDist*alarmDist)
        return 0;

    contacts->resize(contacts->size()+1);
    DetectionOutput *detection = &*(contacts->end()-1);

#ifdef SOFA_DETECTIONOUTPUT_FREEMOTION
    if (e1.hasFreePosition() && e2.hasFreePosition())
    {
        Vector3 Pfree,Qfree,ABfree,CDfree;
        ABfree = e1.p2Free()-e1.p1Free();
        CDfree = e2.p2Free()-e2.p1Free();
        Pfree = e1.p1Free() + ABfree * alpha;
        Qfree = e2.p1Free() + CDfree * beta;

        detection->freePoint[0] = Pfree;
        detection->freePoint[1] = Qfree;
    }
#endif

    const SReal contactDist = intersection->getContactDistance() + e1.getProximity() + e2.getProximity();

    detection->elem = std::pair<core::CollisionElementIterator, core::CollisionElementIterator>(e1, e2);
    detection->id = (e1.getCollisionModel()->getSize() > e2.getCollisionModel()->getSize()) ? e1.getIndex() : e2.getIndex();
    detection->point[0] = P;
    detection->point[1] = Q;
    detection->normal = PQ;
    detection->value = detection->normal.norm();

    if(detection->value>1e-15)
    {
        detection->normal /= detection->value;
    }
    else
    {
        msg_warning(intersection) << "Null distance between contact detected";
        detection->normal= Vector3(1,0,0);
    }
    detection->value -= contactDist;

    return 1;
}

bool MeshMinProximityIntersection::testIntersection(Triangle& e2, Point& e1)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    const Vector3 AB = e2.p2()-e2.p1();
    const Vector3 AC = e2.p3()-e2.p1();
    const Vector3 AP = e1.p() -e2.p1();
    Matrix2 A;
    Vector2 b;

    // We want to find alpha,beta so that:
    // AQ = AB*alpha+AC*beta
    // PQ.AB = 0 and PQ.AC = 0
    // (AQ-AP).AB = 0 and (AQ-AP).AC = 0
    // AQ.AB = AP.AB and AQ.AC = AP.AC
    //
    // (AB*alpha+AC*beta).AB = AP.AB and
    // (AB*alpha+AC*beta).AC = AP.AC
    //
    // AB.AB*alpha + AC.AB*beta = AP.AB and
    // AB.AC*alpha + AC.AC*beta = AP.AC
    //
    // A . [alpha beta] = b
    A[0][0] = AB*AB;
    A[1][1] = AC*AC;
    A[0][1] = A[1][0] = AB*AC;
    b[0] = AP*AB;
    b[1] = AP*AC;
    const SReal det = type::determinant(A);

    SReal alpha = 0.5;
    SReal beta = 0.5;

    alpha = (b[0]*A[1][1] - b[1]*A[0][1])/det;
    beta  = (b[1]*A[0][0] - b[0]*A[1][0])/det;
    if (alpha < 0.000001 ||
            beta  < 0.000001 ||
            alpha + beta  > 0.999999)
        return false;

    const Vector3 PQ = AB * alpha + AC * beta - AP;

    return PQ.norm2() < alarmDist * alarmDist;
}

int MeshMinProximityIntersection::computeIntersection(Triangle& e2, Point& e1, OutputVector* contacts)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    const Vector3 AB = e2.p2()-e2.p1();
    const Vector3 AC = e2.p3()-e2.p1();
    const Vector3 AP = e1.p() -e2.p1();
    Matrix2 A;
    Vector2 b;

    A[0][0] = AB*AB;
    A[1][1] = AC*AC;
    A[0][1] = A[1][0] = AB*AC;
    b[0] = AP*AB;
    b[1] = AP*AC;

    const SReal det = type::determinant(A);

    SReal alpha = 0.5;
    SReal beta = 0.5;

    alpha = (b[0]*A[1][1] - b[1]*A[0][1])/det;
    beta  = (b[1]*A[0][0] - b[0]*A[1][0])/det;
    if (alpha < 0.000001 ||
            beta  < 0.000001 ||
            alpha + beta  > 0.999999)
        return 0;

    Vector3 P,Q,QP; //PQ
    P = e1.p();
    Q = e2.p1() + AB * alpha + AC * beta;
    QP = P-Q;

    if (QP.norm2() >= alarmDist*alarmDist)
        return 0;

    //Vector3 PQ = Q-P;

    contacts->resize(contacts->size()+1);
    DetectionOutput *detection = &*(contacts->end()-1);

#ifdef SOFA_DETECTIONOUTPUT_FREEMOTION
    if (e1.hasFreePosition() && e2.hasFreePosition())
    {
        Vector3 Pfree,Qfree,ABfree,ACfree;
        ABfree = e2.p2Free()-e2.p1Free();
        ACfree = e2.p3Free()-e2.p1Free();
        Pfree = e1.pFree();
        Qfree = e2.p1Free() + ABfree * alpha + ACfree * beta;

        detection->freePoint[0] = Qfree;
        detection->freePoint[1] = Pfree;
    }
#endif

    const SReal contactDist = intersection->getContactDistance() + e1.getProximity() + e2.getProximity();

    detection->elem = std::pair<core::CollisionElementIterator, core::CollisionElementIterator>(e2, e1);
    detection->id = e1.getIndex();
    detection->point[0]=Q;
    detection->point[1]=P;
    detection->normal = QP;
    detection->value = detection->normal.norm();
    if(detection->value>1e-15)
    {
        detection->normal /= detection->value;
    }
    else
    {
        msg_warning(intersection) << "Null distance between contact detected";
        detection->normal= Vector3(1,0,0);
    }
    detection->value -= contactDist;

    if(intersection->getUseSurfaceNormals())
    {
        const auto normalIndex = e2.getIndex();
        detection->normal = e2.model->getNormals()[normalIndex];
    }    

    return 1;
}

bool MeshMinProximityIntersection::testIntersection(Line& e2, Point& e1)
{

    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();
    const Vector3 AB = e2.p2()-e2.p1();
    const Vector3 AP = e1.p()-e2.p1();

    SReal A;
    SReal b;
    A = AB*AB;
    b = AP*AB;

    SReal alpha = 0.5;

    alpha = b/A;
    if (alpha < 0.000001 || alpha > 0.999999)
        return false;

    Vector3 P,Q,PQ;
    P = e1.p();
    Q = e2.p1() + AB * alpha;
    PQ = Q-P;

    return PQ.norm2() < alarmDist * alarmDist;
}

int MeshMinProximityIntersection::computeIntersection(Line& e2, Point& e1, OutputVector* contacts)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();
    const Vector3 AB = e2.p2()-e2.p1();
    const Vector3 AP = e1.p()-e2.p1();

    SReal A;
    SReal b;
    A = AB*AB;
    b = AP*AB;

    SReal alpha = 0.5;

    Vector3 P,Q,QP;

    alpha = b/A;

    if (alpha <= 0.0){
        Q = e2.p1();
    }
    else if (alpha >= 1.0){
        Q = e2.p2();
    }
    else{
        Q = e2.p1() + AB * alpha;
    }

    P = e1.p();
    QP = P-Q;

    if (QP.norm2() >= alarmDist*alarmDist)
        return 0;

    contacts->resize(contacts->size()+1);
    DetectionOutput *detection = &*(contacts->end()-1);

#ifdef SOFA_DETECTIONOUTPUT_FREEMOTION
    if (e1.hasFreePosition() && e2.hasFreePosition())
    {
        Vector3 ABfree = e2.p2Free()-e2.p1Free();
        Vector3 Pfree = e1.pFree();
        Vector3 Qfree = e2.p1Free() + ABfree * alpha;
        detection->freePoint[0] = Qfree;
        detection->freePoint[1] = Pfree;
    }
#endif

    const SReal contactDist = intersection->getContactDistance() + e1.getProximity() + e2.getProximity();

    detection->elem = std::pair<core::CollisionElementIterator, core::CollisionElementIterator>(e2, e1);
    detection->id = e1.getIndex();
    detection->point[0]=Q;
    detection->point[1]=P;
    detection->normal=QP;
    detection->value = detection->normal.norm();
    if(detection->value>1e-15)
    {
        detection->normal /= detection->value;
    }
    else
    {
        msg_warning(intersection) << "Null distance between contact detected";
        detection->normal= Vector3(1,0,0);
    }
    detection->value -= contactDist;

    return 1;
}

bool MeshMinProximityIntersection::testIntersection(Point& e1, Point& e2)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    const Vector3 PQ = e2.p()-e1.p();

    return PQ.norm2() < alarmDist * alarmDist;
}

int MeshMinProximityIntersection::computeIntersection(Point& e1, Point& e2, OutputVector* contacts)
{
    const SReal alarmDist = intersection->getAlarmDistance() + e1.getProximity() + e2.getProximity();

    Vector3 P,Q,PQ;
    P = e1.p();
    Q = e2.p();
    PQ = Q-P;

    if (PQ.norm2() >= alarmDist*alarmDist)
        return 0;

    contacts->resize(contacts->size()+1);
    DetectionOutput *detection = &*(contacts->end()-1);

#ifdef SOFA_DETECTIONOUTPUT_FREEMOTION
    if (e1.hasFreePosition() && e2.hasFreePosition())
    {
        Vector3 Pfree, Qfree;
        Pfree = e1.pFree();
        Qfree = e2.pFree();

        detection->freePoint[0] = Pfree;
        detection->freePoint[1] = Qfree;
    }
#endif

    const SReal contactDist = intersection->getContactDistance() + e1.getProximity() + e2.getProximity();

    detection->elem = std::pair<core::CollisionElementIterator, core::CollisionElementIterator>(e1, e2);
    detection->id = (e1.getCollisionModel()->getSize() > e2.getCollisionModel()->getSize()) ? e1.getIndex() : e2.getIndex();
    detection->point[0]=P;
    detection->point[1]=Q;
    detection->normal=PQ;
    detection->value = detection->normal.norm();

    if(detection->value>1e-15)
    {
        detection->normal /= detection->value;
    }
    else
    {
        msg_warning(intersection) << "Null distance between contact detected";
        detection->normal= Vector3(1,0,0);
    }
    detection->value -= contactDist;

    return 1;
}

} // namespace sofa::component::collision::detection::intersection
