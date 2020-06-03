#pragma once
namespace MathUtilities
{
	/* --- Taken from Box2D : b2Math.h --- */
	// This function is used to ensure that a floating point number is not a NaN or infinity.
	inline bool IsValid(float x)
	{
		int ix = *reinterpret_cast<int*>(&x);
		return (ix & 0x7f800000) != 0x7f800000;
	}

	// This is a approximate yet fast inverse square-root.
	inline float InvSqrt(float x)
	{
		union
		{
			float x;
			int i;
		} convert;

		convert.x = x;
		float xhalf = 0.5f * x;
		convert.i = 0x5f3759df - (convert.i >> 1);
		x = convert.x;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}
}