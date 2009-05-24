#include "collision.h"
#include "OgreOpcode.h"


namespace Collision {


class CollisionPacket {
public:
	Vector3 eRadius; 
	Vector3 R3Velocity;
	Vector3 R3Position;
	Vector3 velocity;
	Vector3 normalizedVelocity;
	Vector3 basePoint;
	bool foundCollision;
	double nearestDistance;
	Vector3 intersectionPoint;
};


void doOgreOpcodeCollision(CollisionPacket& colData, float sweepOffset, 
						   CollisionListener* listener)
{
	Vector3 pos_R3 = colData.basePoint * colData.eRadius;
	Vector3 vel_R3 = colData.velocity * colData.eRadius;
	
	OgreOpcode::CollisionPair** reports;

	Real radius = colData.eRadius.x;

	Vector3 offset = vel_R3;
	offset.normalise();
	offset *= sweepOffset;

	OgreOpcode::Details::CollisionClass collClass = OgreOpcode::COLLTYPE_ALWAYS_EXACT;

	int count = OgreOpcode::CollisionManager::getSingletonPtr()->getDefaultContext()->
		sweptSphereCheck(pos_R3, vel_R3 + offset, radius, collClass, reports);

	if (count) {
		int closest = 0;
		Real d = reports[0]->distance;
		for (int i = 1; i < count; i++) {
			if (reports[i]->distance < d) {
				d = reports[i]->distance;
				closest = i;
			}
		}

		colData.foundCollision = true;
		colData.nearestDistance = reports[closest]->distance;
		colData.intersectionPoint = reports[closest]->contact / colData.eRadius;

		if (listener) {
			String otherName = reports[closest]->other_object->getName();
			listener->collisionOccurred(otherName);
		}
	}
}

const float unitsPerMeter = GV::UnitsPerMeter;

const float unitScale = unitsPerMeter / 10.0f;
const float veryCloseDistance = 0.5 * unitScale; 

Vector3 collideWithWorld(int recursionDepth, const Vector3& pos, 
						 const Vector3& vel, CollisionPacket& colData, 
						 CollisionListener* listener, bool gravityStep, 
						 const Degree& slopeSlideThresold)
{
	if (recursionDepth > 5) 
		return pos;

	colData.velocity = vel;
	colData.normalizedVelocity = vel;
	colData.normalizedVelocity.normalise();
	colData.basePoint = pos;
	colData.foundCollision = false;

	doOgreOpcodeCollision(colData, veryCloseDistance * 5, listener);

	if (colData.foundCollision == false) {
		return pos + vel;
	}

	Vector3 destinationPoint = pos + vel;
	Vector3 newBasePoint = pos;

	if (colData.nearestDistance >= veryCloseDistance) {
		Vector3 V = vel;
		V.normalise();
		V = V * (colData.nearestDistance - veryCloseDistance);
		newBasePoint = colData.basePoint + V;

		V.normalise();
		colData.intersectionPoint -= veryCloseDistance * V;
	}

	Vector3 slidePlaneOrigin = colData.intersectionPoint;
	Vector3 slidePlaneNormal = newBasePoint - colData.intersectionPoint;
	slidePlaneNormal.normalise();
	Plane slidingPlane(slidePlaneNormal, slidePlaneOrigin);

	Vector3 newDestinationPoint = destinationPoint - 
		slidingPlane.getDistance(destinationPoint) *
		slidePlaneNormal;

	Vector3 newVelocityVector = newDestinationPoint - colData.intersectionPoint;

	if (newVelocityVector.length() < veryCloseDistance) {
		return newBasePoint;
	}

	if (gravityStep) {
		const Radian tolerance = Radian(slopeSlideThresold);
		Vector3 gravity = vel;
		gravity.normalise();
		if (slidePlaneNormal.directionEquals(-gravity, tolerance)) {
			return newBasePoint;
		}
	}

	return collideWithWorld(recursionDepth++, newBasePoint, newVelocityVector, 
		colData, listener, gravityStep, slopeSlideThresold);
}


Vector3 collideAndSlide(const Vector3& pos, const Vector3& vel, Real radius, 
						const Vector3& gravity, CollisionListener* listener,
						const Degree& slopeSlideThresold)
{
	CollisionPacket colData;

	// Information about the move being requested: (in R3)
	colData.R3Position = pos;
	colData.R3Velocity = vel;

	// convert simple sphere radius into ellipsoid radius
	// (algorithm works with ellipsoids but for now ogreopcode works only with spheres)
	colData.eRadius = Vector3(radius, radius, radius);

	// calculate position and velocity in eSpace
	Vector3 eSpacePosition = colData.R3Position / colData.eRadius;
	Vector3 eSpaceVelocity = colData.R3Velocity / colData.eRadius;

	// Iterate until we have our final position.
	Vector3 finalPosition = collideWithWorld(0, eSpacePosition, eSpaceVelocity, 
		colData, listener, false, Degree(0));

	// Add gravity pull:
	if (gravity != Vector3::ZERO) {
		// Set the new R3 position (convert back from eSpace to R3)
		colData.R3Position = finalPosition * colData.eRadius;
		colData.R3Velocity = gravity;
		eSpacePosition = colData.R3Position / colData.eRadius;
		eSpaceVelocity = gravity / colData.eRadius;
		finalPosition = collideWithWorld(0, eSpacePosition, eSpaceVelocity, 
			colData, listener, true, slopeSlideThresold);
	}

	// Convert final result back to R3:
	return finalPosition * colData.eRadius;
}
} // namespace