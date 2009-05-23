#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "OgreVector3.h"

namespace Collision {

	using namespace Ogre;

	class CollisionListener
	{
	public:
		CollisionListener() {}
		virtual void collisionOccurred(String obj) {}
	};

	Vector3 collideAndSlide(const Vector3& pos, const Vector3& vel, Real radius, 
		const Vector3& gravity, CollisionListener* listener = 0,
		const Degree& slopeSlideThresold = Degree(45));
}

#endif