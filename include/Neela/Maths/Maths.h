#pragma once

#include <type_traits>
#include <cmath>
#include <random>

namespace Neela
{
	struct Maths
	{
		static constexpr double PI = 3.1415926535;
		static constexpr double DEG2RAD = 0.0174532925;
		static constexpr double RAD2DEG = 57.2957795131;

		template<typename T, typename U>
		static bool	Equals(T a, U b, double tolerance = 0.0001);

		template<typename T>
		static T Abs(T value);

		template<typename T, typename U>
		static T Pow(T value, U exponent);

		template<typename T>
		static T Sqrt(T value);

		template<typename T, typename U, typename V>
		static T Clamp(T value, U min, V max);

		template<typename T, typename U>
		static T Lerp(T min, T max, U progress);

		template<typename T, typename U>
		static T Repeat(T value, U max);

		template<typename T, typename U>
		static T PingPong(T value, U max);

		template<typename T>
		static T Min(T l, T r);

		template<typename T>
		static T Max(T l, T r);

		template<typename T>
		static T Max(const std::vector<T>& vec);

		template<typename T>
		static T Frac(T value);

		// Easing functions

		template<typename T>
		static T EaseOutExpo(T value);

		template<typename T>
		static T EaseOutBack(T value);

		template<typename T>
		static T EaseInOutBack(T value);

		template<typename T>
		static T EaseOutElastic(T value);

		template<typename T>
		static T EaseOutBounce(T value);

		template<typename T>
		static T EaseParabolic2(T value);

		template<typename T>
		static T EaseParabolic4(T value);

		template<typename T>
		static T EaseParabolic6(T value);

		template<typename T>
		static T EaseCosine2(T value);

		template<typename T>
		static T EaseHeartBeat(T value);

		// Returns random int betwenn 0 and 0x7FFFFFFF
		static int32_t RandomInt()
		{
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_int_distribution<> distrib(0);
			return distrib(mt);
		}
	};

	template<typename T, typename U>
	inline bool Maths::Equals(T a, U b, double tolerance)
	{
		static_assert(std::is_arithmetic_v<T>, "Maths::Equals(T a, U b, double tolerance) -> T is not arithmetic.");
		static_assert(std::is_arithmetic_v<U>, "Maths::Equals(T a, U b, double tolerance) -> U is not arithmetic.");

		return abs(a - b) < tolerance ? true : false;
	}

	template<typename T>
	inline T Maths::Abs(T value)
	{
		static_assert(std::is_arithmetic_v<T>, "Maths::Abs(T value) -> T is not arithmetic.");

		return static_cast<T>(abs(value));
	}

	template<typename T, typename U>
	inline T Maths::Pow(T value, U exponent)
	{
		static_assert(std::is_arithmetic_v<T>, "Maths::Pow(T value, U exponent) -> T is not arithmetic.");
		static_assert(std::is_arithmetic_v<U>, "Maths::Pow(T value, U exponent) -> U is not arithmetic.");
		return exponent >= 0
			? static_cast<T>(pow(value, exponent))
			: 1.0 / Maths::Pow(value, -exponent);
	}

	template<typename T>
	inline T Maths::Sqrt(T value)
	{
		return sqrt(value);
	}

	template<typename T, typename U, typename V>
	inline T Maths::Clamp(T value, U min, V max)
	{
		return (value <= min ? min : (value >= max ? max : value));
	}

	template<typename T, typename U>
	inline T Maths::Lerp(T min, T max, U progress)
	{
		return min + (max - min) * progress;
	}

	template<typename T, typename U>
	inline T Maths::Repeat(T value, U max)
	{
		if (value >= max)
		{
			while (value >= max)
				value -= max;
		}
		else if (value < 0)
		{
			while (value < 0)
				value += max;
		}

		return value;
	}

	template<typename T, typename U>
	inline T Maths::PingPong(T value, U max)
	{
		if (max == 0.0)
		{
			return 0.0f;
		}
		else
		{
			return Maths::Abs(Maths::Frac((value - max) / (max * 2.0)) * max * 2.0 - max);
		}
	}

	template<typename T>
	inline T Maths::Min(T l, T r)
	{
		return l < r ? l : r;
	}

	template<typename T>
	inline T Maths::Max(T l, T r)
	{
		return l > r ? l : r;
	}

	template<typename T>
	inline T Maths::Max(const std::vector<T>& vec)
	{
		if (vec.size() <= 0)
			return 0;

		T highest = vec[0];
		for (auto& i : vec)
		{
			if (i > highest)
				highest = i;
		}

		return highest;
	}

	template<typename T>
	inline T Maths::Frac(T value)
	{
		return (value - floor(value));
	}

	template<typename T>
	inline T Maths::EaseOutExpo(T value)
	{
		T pow = Maths::Pow(2.0, -10 * value);
		return Maths::Equals(value, 1) ? 1 : 1 - pow;
	}

	template<typename T>
	inline T Maths::EaseOutBack(T value)
	{
		return 1 + 2.70158 * Maths::Pow(value - 1, 3) + 1.70158 * Maths::Pow(value - 1, 2);
	}

	template<typename T>
	inline T Maths::EaseInOutBack(T value)
	{
		const T c1 = 1.70158;
		const T c2 = c1 * 1.525;

		return value < 0.5
			? (Maths::Pow(2.0 * value, 2) * ((c2 + 1) * 2 * value - c2)) * 0.5
			: (Maths::Pow(2.0 * value - 2, 2) * ((c2 + 1) * (value * 2 - 2) + c2) + 2) * 0.5;
	}

	template<typename T>
	inline T Maths::EaseOutElastic(T value)
	{
		const double c4 = (2.0 * PI) / 3.0;

		return value == 0
			? 0
			: value == 1
			? 1
			: Maths::Pow(2.0, -10.0 * value) * sin((value * 10 - 0.75) * c4) + 1;
	}

	template<typename T>
	inline T Maths::EaseOutBounce(T value)
	{
		if (value < 1.0 / 2.75)
		{
			return 7.5625 * value * value;
		}
		else if (value < 2.0 / 2.75)
		{
			return 7.5625 * (value -= 1.5 / 2.75) * value + 0.75;
		}
		else if (value < 2.5 / 2.75)
		{
			return 7.5625 * (value -= 2.25 / 2.75) * value + 0.9375;
		}
		else
		{
			return 7.5625 * (value -= 2.625 / 2.75) * value + 0.984375;
		}
	}

	template<typename T>
	inline T Maths::EaseParabolic2(T value)
	{
		return -4 * Maths::Pow(value - 0.5, 2.0) + 1;
	}

	template<typename T>
	inline T Maths::EaseParabolic4(T value)
	{
		return -16 * Maths::Pow(value - 0.5, 4.0) + 1;
	}

	template<typename T>
	inline T Maths::EaseParabolic6(T value)
	{
		return -64 * Maths::Pow(value - 0.5, 6.0) + 1;
	}

	template<typename T>
	inline T Maths::EaseCosine2(T value)
	{
		return 0.5 * (cos(2 * value * PI) + 1);
	}

	template<typename T>
	inline T Maths::EaseHeartBeat(T value)
	{
		return 0.78 * (1 + cos(-2 * value * PI + 5));
	}
}