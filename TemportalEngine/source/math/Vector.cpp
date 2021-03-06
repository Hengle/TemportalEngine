#include "math/Vector.hpp"

NS_MATH

Quaternion Quaternion::FromAxisAngle(Vector<f32, 3> axis, f32 radians)
{
	auto halfAngle = radians * 0.5f;
	return Quaternion(axis * std::sin(halfAngle))
		+ Quaternion::Identity * std::cos(halfAngle);
}

Quaternion Quaternion::FromEuler(Vector<f32, 3> euler)
{
	Quaternion quat;
	euler * 0.5f;
	auto sin = Vector3({ std::sin(euler.x()), std::sin(euler.y()), std::sin(euler.z()) });
	auto cos = Vector3({ std::cos(euler.x()), std::cos(euler.y()), std::cos(euler.z()) });
	quat.w(cos.x() * cos.y() * cos.z() - sin.x() * sin.y() * sin.z());
	quat.x(sin.x() * sin.y() * cos.z() + cos.x() * cos.y() * sin.z());
	quat.y(sin.x() * cos.y() * cos.z() + cos.x() * sin.y() * sin.z());
	quat.z(cos.x() * sin.y() * cos.z() - sin.x() * cos.y() * sin.z());
	return quat;
}

// See https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_Angles_Conversion
Vector<f32, 3> Quaternion::euler() const
{
	Vector3 euler;

	euler.x(std::atan2(
		2 * (this->w() * this->x() + this->y() * this->z()),
		1 - 2 * (this->x() * this->x() + this->y() * this->y())
	));

	auto sinp = 2 * (this->w() * this->y() - this->z() * this->x());
	euler.y(
		// use 90 degrees if out of range
		std::abs(sinp) >= 1 ? std::copysign((f32)math::pi2(), sinp) : std::asin(sinp)
	);

	euler.z(std::atan2(
		2 * (this->w() * this->z() + this->x() * this->y()),
		1 - 2 * (this->y() * this->y() + this->z() * this->z())
	));

	return euler;
}

Quaternion Quaternion::conjugate() const
{
	// <-x, -y, -z, w>
	return Quaternion(-createSubvector<3>()) + Quaternion::Identity * w();
}

Quaternion Quaternion::inverseQuat() const
{
	return this->conjugate().normalized();
}

/* glm::rotate
	this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
	this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
	this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
	this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
*/
Quaternion Quaternion::concat(Quaternion const &a, Quaternion const &b)
{
	/* https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
		a1a2 - b1b2 - c1c2 - d1d2
		+ (a1b2 + b1a2 + c1d2 - d1c2)i
		+ (a1c2 - b1d2 + c1a2 + d1b2)j
		+ (a1d2 + b1c2 - c1b2 + d1a2)k
	*/
	/* https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
		where:
			a1+b1i+c1j+d1k = `a<x, y, z, w>`
			a2+b2i+c2j+d2k = `b<x, y, z, w>`
		w: awbw - axbx - ayby - azbz
		x: awbx + axbw + aybz - azby
		y: awby - axbz + aybw + azbx
		z: awbz + axby - aybx + azbw
	*/
	Vector3 aRealVec = a.createSubvector<3>();
	Vector3 bRealVec = b.createSubvector<3>();
	// aw*bw - (ax*bx + ay*by + az*bz)
	float w = a.w() * b.w() - Vector3::dot(aRealVec, bRealVec);
	// <ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx>
	auto crossReal = Vector3::cross(aRealVec, bRealVec);
	/*
		v: aw*b + bw*a + cross
		x: awbx + axbw + (aybz - azby)
		y: awby + aybw + (azbx - axbz)
		z: awbz + azbw + (axby - aybx)
	*/
	Vector3 vec = (a.w() * bRealVec) + (b.w() * aRealVec) + crossReal;
	return Quaternion(vec) + Quaternion::Identity * w;
}

Vector<f32, 3> Quaternion::rotate(Vector<f32, 3> const v) const
{
	auto qVec = createSubvector<3>();
	auto a = 2 * qVec.dot(v) * qVec;
	auto b = (w() * w() - qVec.magnitudeSq()) * v;
	auto c = 2 * w() * Vector3::cross(qVec, v);
	return a + b + c;
}

Quaternion const MultiplyVector(Vector3 const vector, Quaternion const quat)
{
	// r = quat vector3
	// q* = < vxr + wv, -dot(v, r) >
	return Quaternion(
		Vector3::cross(vector, quat.createSubvector<3>())
		+ vector * quat.w()
	) + Quaternion::Identity * -Vector3::dot(vector, quat.createSubvector<3>());
}

Quaternion const IntegrateKinematic(Quaternion const rotation,
	Vector3 const angularVelocity, Vector3 const angularAcceleration, f32 const deltaTime)
{
	// qPrime = q + (dq/dt)dt + (1/2)(d^2q/dt^2)dt^2
	// qPrime = q + [(1/2)wdt * q] + [(1/2)aq - (1/4)|w|^2*q]dt^2
	Quaternion integrated = rotation;
	// integrate velocity
	integrated += MultiplyVector(0.5f * angularVelocity * deltaTime, rotation);
	// integrate acceleration
	integrated += (0.5f * deltaTime * deltaTime) * (
		MultiplyVector(0.5f * angularAcceleration, rotation)
		-
		(0.25f * angularVelocity.magnitudeSq() * rotation)
	);
	// normalize
	return integrated.normalized();
}

Vector2 const Vector2::ZERO = { 0, 0 };
Vector2 const Vector2unitX = { 1, 0 };
Vector2 const Vector2unitY = { 0, 1 };
Vector3 const Vector3::ZERO = { 0, 0, 0 };
Vector3 const Vector3unitX = { 1, 0, 0 };
Vector3 const Vector3unitY = { 0, 1, 0 };
Vector3 const Vector3unitZ = { 0, 0, 1 };
Vector4 const Vector4::ZERO = { 0, 0, 0, 0 };
Vector4 const Vector4unitX = { 1, 0, 0, 0 };
Vector4 const Vector4unitY = { 0, 1, 0, 0 };
Vector4 const Vector4unitZ = { 0, 0, 1, 0 };
Vector4 const Vector4unitW = { 0, 0, 0, 1 };
Quaternion const Quaternion::Identity = math::Vector4unitW;

Vector2Int const Vector2Int::ZERO = { 0, 0 };
Vector3Int const Vector3Int::ZERO = { 0, 0, 0 };

Vector2UInt const Vector2UInt::ZERO = { 0, 0 };
Vector3UInt const Vector3UInt::ZERO = { 0, 0, 0 };

NS_END
