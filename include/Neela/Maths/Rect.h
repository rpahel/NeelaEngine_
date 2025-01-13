#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"
#include "Neela/Interfaces/IPrintable.h"

#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>

#define RECT_SERIALIZE_VERSION 1

namespace Neela
{
	struct NEELA_ENGINE_API Rect : IPrintable, ISerializeable
	{
	public:

		int32_t W, H;
		int32_t X, Y;

		//==== Constructors ====

		explicit Rect() : Rect(0, 0) {}
		explicit Rect(int32_t w, int32_t h, int32_t x = 0, int32_t y = 0) : W(w), H(h), X(x), Y(y) {}
		explicit Rect(const Vector2i& size, const Vector2i& position = Vector2i(0)) : Rect(size.X, size.Y, position.X, position.Y) {}

		//==== Methods ====

		Vector2i GetSizeVector()		const { return Vector2i(W, H); }
		Vector2i GetPositionVector()	const { return Vector2i(X, Y); }
		SDL_Rect GetSdlRect()			const { return { X, Y, W, H }; }

		bool Contains(const Vector2i& point) const
		{
			return
				point.X >= X && point.Y >= Y &&
				point.X <= X + W && point.Y <= Y + H;
		}

		//==== IPrintable ====

		const std::string& ToString() const
		{
			nlohmann::ordered_json json;
			Serialize(json);
			return json.dump(2);
		}

		//==== ISerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override
		{
			Serializer::Serialize<uint8_t>(byteArray, RECT_SERIALIZE_VERSION);
			Serializer::Serialize<int32_t>(byteArray, W);
			Serializer::Serialize<int32_t>(byteArray, H);
			Serializer::Serialize<int32_t>(byteArray, X);
			Serializer::Serialize<int32_t>(byteArray, Y);
		}

		void Serialize(nlohmann::ordered_json& j) const override
		{
			j["W"] = W;
			j["H"] = H;
			j["X"] = X;
			j["Y"] = Y;
		}

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override
		{
			const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
			if (version > RECT_SERIALIZE_VERSION)
			{
				fmt::print(stderr, fg(fmt::color::red), "Rect::Deserialize() -> Rect has unsupported version {} (Current version is {}).\n", version, RECT_SERIALIZE_VERSION);
				return false;
			}

			W = Serializer::Deserialize<int32_t>(byteArray, offset);
			H = Serializer::Deserialize<int32_t>(byteArray, offset);
			X = Serializer::Deserialize<int32_t>(byteArray, offset);
			Y = Serializer::Deserialize<int32_t>(byteArray, offset);

			return true;
		}

		bool Deserialize(nlohmann::ordered_json& json) override
		{
			W = json.value("W", 0);
			H = json.value("H", 0);
			X = json.value("X", 0);
			Y = json.value("Y", 0);

			return true;
		}
	};
}