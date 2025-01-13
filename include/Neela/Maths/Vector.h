#pragma once

#include "Neela/Maths/Maths.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/ISerializeable.h"

#include <cmath>
#include <type_traits>
#include <SDL2/SDL.h>

#define VECTOR_SERIALIZE_VERSION 1

namespace Neela
{
	template<typename T>
	struct Vector2 : IPrintable, ISerializeable
	{
		T X, Y;

		//==== Constructors ====

		explicit Vector2();
		explicit Vector2(T x, T y);
		explicit Vector2(T values);
		explicit Vector2(const SDL_FPoint& sdlPoint);

		//==== Methods ====

		Vector2& Add(const Vector2& other);
		Vector2& Add(T scalar);
		Vector2& Substract(const Vector2& other);
		Vector2& Substract(T scalar);

		// Multiplies each component of this vector with corresponding component of other vector.
		template<typename U>
		Vector2& Multiply(const Vector2<U>& other);
		template<typename U>
		Vector2& Multiply(U scalar);

		// Divides each component of this vector by corresponding component of other vector.
		template<typename U>
		Vector2& Divide(const Vector2<U>& other);
		template<typename U>
		Vector2& Divide(U scalar);

		Vector2& RotateRad(float angle);
		Vector2& RotateDeg(float angle);

		T GetMagnitudeSquared() const;
		T GetMagnitude() const;
		T GetMax() const;
		T GetAbsoluteMax() const;

		SDL_FPoint ToSdlFPoint() const;

		//==== Static Methods ====

		static Vector2 Up();
		static Vector2 Right();
		static Vector2 Down();
		static Vector2 Left();
		static Vector2 One();

		static Vector2 Add(const Vector2& a, const Vector2& b);
		static Vector2 Add(const Vector2& vector, T scalar);
		static Vector2 Substract(const Vector2& a, const Vector2& b);
		static Vector2 Substract(const Vector2& vector, T scalar);

		// Multiplies each component of a with corresponding component of b.
		template<typename U, typename V>
		static Vector2 Multiply(const Vector2<U>& a, const Vector2<V>& b);
		template<typename U, typename V>
		static Vector2 Multiply(const Vector2<U>& vector, V scalar);

		// Divides each component of a by corresponding component of b.
		template<typename U, typename V>
		static Vector2 Divide(const Vector2<U>& a, const Vector2<V>& b);
		template<typename U, typename V>
		static Vector2 Divide(const Vector2<U>& vector, V scalar);

		static Vector2 RotateRad(const Vector2& vector, float angle);
		static Vector2 RotateDeg(const Vector2& vector, float angle);

		//==== IPrintable ====

		const std::string& ToString() const override;

		//==== ISerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& j) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== Operators ====

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator+(float scalar) const;
		Vector2& operator+=(const Vector2& other);
		Vector2& operator+=(float scalar);

		Vector2 operator-(const Vector2& other) const;
		Vector2 operator-(float scalar) const;
		Vector2& operator-=(const Vector2& other);
		Vector2& operator-=(float scalar);

		// Multiplies each component of this vector with corresponding component of other vector.
		template<typename U>
		Vector2 operator*(const Vector2<U>& other) const;
		template<typename U>
		Vector2 operator*(U scalar) const;
		template<typename U>
		Vector2& operator*=(const Vector2<U>& other);
		template<typename U>
		Vector2& operator*=(U scalar);

		// Divides each component of this vector by corresponding component of other vector.
		template<typename U>
		Vector2 operator/(const Vector2<U>& other) const;
		template<typename U>
		Vector2 operator/(U scalar) const;
		template<typename U>
		Vector2& operator/=(const Vector2<U>& other);
		template<typename U>
		Vector2& operator/=(U scalar);

		template<typename U>
		bool operator==(const Vector2<U>& other) const;

		template<typename U>
		operator Vector2<U>() const;
	};

	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
	using Vector2i = Vector2<int>;

	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	template <typename T>
	inline Vector2<T>::Vector2() : Vector2(0, 0)
	{
	}

	template <typename T>
	inline Vector2<T>::Vector2(T values) : Vector2(values, values)
	{
	}

	template <typename T>
	inline Vector2<T>::Vector2(T x, T y) : X(x), Y(y)
	{
		static_assert(std::is_arithmetic_v<T>, "Vector2<T>::Vector2(T x, T y) -> T is not arithmetic.");
	}

	template<typename T>
	inline Vector2<T>::Vector2(const SDL_FPoint& sdlPoint) :
		X(sdlPoint.x),
		Y(sdlPoint.y)
	{
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	template <typename T>
	Vector2<T>& Vector2<T>::Add(const Vector2& other)
	{
		X += other.X;
		Y += other.Y;

		return *this;
	}

	template <typename T>
	Vector2<T>& Vector2<T>::Add(T scalar)
	{
		X += scalar;
		Y += scalar;

		return *this;
	}

	template <typename T>
	Vector2<T>& Vector2<T>::Substract(const Vector2& other)
	{
		X -= other.X;
		Y -= other.Y;

		return *this;
	}

	template <typename T>
	Vector2<T>& Vector2<T>::Substract(T scalar)
	{
		X -= scalar;
		Y -= scalar;

		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::Multiply(const Vector2<U>& other)
	{
		X *= other.X;
		Y *= other.Y;

		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::Multiply(U scalar)
	{
		X *= scalar;
		Y *= scalar;

		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::Divide(const Vector2<U>& other)
	{
		X /= other.X;
		Y /= other.Y;

		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::Divide(U scalar)
	{
		X /= scalar;
		Y /= scalar;

		return *this;
	}

	template <typename T>
	Vector2<T>& Vector2<T>::RotateRad(float angle)
	{
		T cosAngle = static_cast<T>(cos(angle));
		T sinAngle = static_cast<T>(sin(angle));
		T newX = cosAngle * X - sinAngle * Y;
		T newY = sinAngle * X + cosAngle * Y;
		X = newX;
		Y = newY;
		return *this;
	}

	template <typename T>
	Vector2<T>& Vector2<T>::RotateDeg(float angle)
	{
		return RotateRad(angle * Maths::DEG2RAD);
	}

	template<typename T>
	inline T Vector2<T>::GetMagnitudeSquared() const
	{
		return X * X + Y * Y;
	}

	template<typename T>
	inline T Vector2<T>::GetMagnitude() const
	{
		return Maths::Sqrt(GetMagnitudeSquared());
	}

	template<typename T>
	inline T Vector2<T>::GetMax() const
	{
		return X >= Y ? X : Y;
	}

	template<typename T>
	inline T Vector2<T>::GetAbsoluteMax() const
	{
		return Maths::Abs(X) >= Maths::Abs(Y) ? X : Y;
	}

	template<typename T>
	inline SDL_FPoint Vector2<T>::ToSdlFPoint() const
	{
		return { X, Y };
	}

	//====================================================================================
	//==== IPRINTABLE
	//====================================================================================

	template<typename T>
	inline const std::string& Vector2<T>::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return json.dump(2);
	}

	//====================================================================================
	//==== ISERIALIZEABLE
	//====================================================================================

	template<typename T>
	inline void Vector2<T>::Serialize(std::vector<uint8_t>& byteArray) const
	{
		Serializer::Serialize<uint8_t>(byteArray, VECTOR_SERIALIZE_VERSION);
		Serializer::Serialize<T>(byteArray, X);
		Serializer::Serialize<T>(byteArray, Y);
	}

	template<typename T>
	inline void Vector2<T>::Serialize(nlohmann::ordered_json& j) const
	{
		j["X"] = X;
		j["Y"] = Y;
	}

	template<typename T>
	inline bool Vector2<T>::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > VECTOR_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "Vector2<T>::Deserialize() -> Vector2 has unsupported version {} (Current version is {}).\n", version, VECTOR_SERIALIZE_VERSION);
			return false;
		}

		X = Serializer::Deserialize<T>(byteArray, offset);
		Y = Serializer::Deserialize<T>(byteArray, offset);

		return true;
	}

	template<typename T>
	inline bool Vector2<T>::Deserialize(nlohmann::ordered_json& json)
	{
		X = json.value("X", static_cast<T>(0));
		Y = json.value("Y", static_cast<T>(0));

		return true;
	}

	//====================================================================================
	//==== PUBLIC STATIC METHODS
	//====================================================================================

	template<typename T>
	inline Vector2<T> Vector2<T>::Up()
	{
		return Vector2(0, 1);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Right()
	{
		return Vector2(1, 0);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Down()
	{
		return Vector2(0, -1);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Left()
	{
		return Vector2(-1, 0);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::One()
	{
		return Vector2(1);
	}

	template <typename T>
	Vector2<T> Vector2<T>::Add(const Vector2& a, const Vector2& b)
	{
		return Vector2(a).Add(b);
	}

	template <typename T>
	Vector2<T> Vector2<T>::Add(const Vector2& vector, T scalar)
	{
		return Vector2(vector).Add(scalar);
	}

	template <typename T>
	Vector2<T> Vector2<T>::Substract(const Vector2& a, const Vector2& b)
	{
		return Vector2(a).Substract(b);
	}

	template <typename T>
	Vector2<T> Vector2<T>::Substract(const Vector2& vector, T scalar)
	{
		return Vector2(vector).Substract(scalar);
	}

	template <typename T>
	template<typename U, typename V>
	Vector2<T> Vector2<T>::Multiply(const Vector2<U>& a, const Vector2<V>& b)
	{
		return Vector2(a).Multiply(b);
	}

	template <typename T>
	template<typename U, typename V>
	Vector2<T> Vector2<T>::Multiply(const Vector2<U>& vector, V scalar)
	{
		return Vector2(vector).Multiply(scalar);
	}

	template <typename T>
	template<typename U, typename V>
	Vector2<T> Vector2<T>::Divide(const Vector2<U>& a, const Vector2<V>& b)
	{
		return Vector2(a).Divide(b);
	}

	template <typename T>
	template<typename U, typename V>
	Vector2<T> Vector2<T>::Divide(const Vector2<U>& vector, V scalar)
	{
		return Vector2(vector).Divide(scalar);
	}

	template <typename T>
	Vector2<T> Vector2<T>::RotateRad(const Vector2& vector, float angle)
	{
		return Vector2(vector).RotateRad(angle);
	}

	template <typename T>
	Vector2<T> Vector2<T>::RotateDeg(const Vector2& vector, float angle)
	{
		return Vector2(vector).RotateDeg(angle);
	}

	//====================================================================================
	//==== PUBLIC OPERATORS
	//====================================================================================

	template <typename T>
	Vector2<T> Vector2<T>::operator+(const Vector2& other) const
	{
		return Vector2(*this).Add(other);
	}

	template <typename T>
	Vector2<T> Vector2<T>::operator+(float scalar) const
	{
		return Vector2(*this).Add(scalar);
	}

	template <typename T>
	Vector2<T>& Vector2<T>::operator+=(const Vector2& other)
	{
		return Add(other);
	}

	template <typename T>
	Vector2<T>& Vector2<T>::operator+=(float scalar)
	{
		return Add(scalar);
	}

	template <typename T>
	Vector2<T> Vector2<T>::operator-(const Vector2& other) const
	{
		return Vector2(*this).Substract(other);
	}

	template <typename T>
	Vector2<T> Vector2<T>::operator-(float scalar) const
	{
		return Vector2(*this).Substract(scalar);
	}

	template <typename T>
	Vector2<T>& Vector2<T>::operator-=(const Vector2& other)
	{
		return Substract(other);
	}

	template <typename T>
	Vector2<T>& Vector2<T>::operator-=(float scalar)
	{
		return Substract(scalar);
	}

	template <typename T>
	template <typename U>
	Vector2<T> Vector2<T>::operator*(const Vector2<U>& other) const
	{
		return Vector2(*this).Multiply(other);
	}

	template <typename T>
	template <typename U>
	Vector2<T> Vector2<T>::operator*(U scalar) const
	{
		return Vector2(*this).Multiply(scalar);
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::operator*=(const Vector2<U>& other)
	{
		return Multiply(other);
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::operator*=(U scalar)
	{
		return Multiply(scalar);
	}

	template <typename T>
	template <typename U>
	Vector2<T> Vector2<T>::operator/(const Vector2<U>& other) const
	{
		return Vector2(*this).Divide(other);
	}

	template <typename T>
	template <typename U>
	Vector2<T> Vector2<T>::operator/(U scalar) const
	{
		return Vector2(*this).Divide(scalar);
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::operator/=(const Vector2<U>& other)
	{
		return Divide(other);
	}

	template <typename T>
	template <typename U>
	Vector2<T>& Vector2<T>::operator/=(U scalar)
	{
		return Divide(scalar);
	}

	template <typename T>
	template <typename U>
	inline bool Vector2<T>::operator==(const Vector2<U>& other) const
	{
		return (X == other.X && Y == other.Y);
	}

	template<typename T>
	template<typename U>
	inline Vector2<T>::operator Vector2<U>() const
	{
		return Vector2<U>(this->X, this->Y);
	}
}