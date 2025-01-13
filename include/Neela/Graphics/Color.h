#pragma once

#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/ISerializeable.h"
#include "Neela/Maths/Maths.h"

#include <SDL2/SDL.h>
#include <cassert>
#include <nlohmann/json.hpp>

#define COLOR_SERIALIZE_VERSION 1

namespace Neela
{
	struct Color : IPrintable, ISerializeable
	{
		uint8_t R, G, B, A;

		//==== Constructors ====

		explicit Color() : Color(0, 0, 0)
		{
		}

		explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255u) : R(r), G(g), B(b), A(a)
		{
		}

		explicit Color(const SDL_Color& sdl_color) : R(sdl_color.r), G(sdl_color.g), B(sdl_color.b), A(sdl_color.a)
		{
		}

		Color(uint32_t hex) : Color(0, 0, 0)
		{
			const Color color = MakeFromHex(hex);
			R = color.R;
			G = color.G;
			B = color.B;
			A = color.A;
		}

		//==== Methods ====

		SDL_Color ToSdlColor() const
		{
			return { R, G, B, A };
		}

		uint32_t ToHex() const
		{
			uint32_t retvalue = 0;
			(retvalue |= R) <<= 8;
			(retvalue |= G) <<= 8;
			(retvalue |= B) <<= 8;
			(retvalue |= A);
			return retvalue;
		}

		int GetHue() const
		{
			const float r = (float)R / 255.0f;
			const float g = (float)G / 255.0f;
			const float b = (float)B / 255.0f;

			const float min = Maths::Min(r, Maths::Min(g, b));
			const float max = Maths::Max(r, Maths::Max(g, b));

			float hue = 0;

			if (r == max)
			{
				hue = (g - b) / (max - min);
			}
			else if (g == max)
			{
				hue = 2.0f + (b - r) / (max - min);
			}
			else
			{
				hue = 4.0f + (r - g) / (max - min);
			}

			hue *= 60;
			if(hue < 0)
				hue += 360;

			return hue;
		}

		//==== Static Methods ====

		static Color GetRandomColor(bool randomAlpha = false)
		{
			return Color(Maths::RandomInt() % 256, Maths::RandomInt() % 256, Maths::RandomInt() % 256, randomAlpha ? Maths::RandomInt() % 256 : 255);
		}

		static Color MakeFromHSV(double hue, double saturation = 100, double value = 100, double alpha = 100)
		{
			if (value <= 0)
				return Color();

			if (value >= 100 && saturation <= 0)
				return Color::White;

			double r = 0, g = 0, b = 0;

			const double h = Maths::Repeat(hue, 360) / 360;
			const double s = Maths::Clamp(saturation, 0, 100) / 100;
			const double v = Maths::Clamp(value, 0, 100) / 100;
			const double a = Maths::Clamp(alpha, 0, 100) / 100;

			int i = h * 6;
			double f = h * 6 - i;
			double p = v * (1 - s);
			double q = v * (1 - f * s);
			double t = v * (1 - (1 - f) * s);

			switch (i % 6)
			{
				case 0: r = v; g = t; b = p; break;
				case 1: r = q; g = v; b = p; break;
				case 2: r = p; g = v; b = t; break;
				case 3: r = p; g = q; b = v; break;
				case 4: r = t; g = p; b = v; break;
				case 5: r = v; g = p; b = q; break;
			}

			return Color(r * 255, g * 255, b * 255, a * 255);
		}

		static Color MakeFromHex(uint32_t hex)
		{
			Color color;
			color.R = (hex & 0xFF000000) >> 24;
			color.G = (hex & 0x00FF0000) >> 16;
			color.B = (hex & 0x0000FF00) >> 8;
			color.A = (hex & 0x000000FF);
			return color;
		}

		static const Color Black;
		static const Color White;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Magenta;
		static const Color Yellow;
		static const Color Cyan;
		static const Color LightGray;
		static const Color Gray;
		static const Color DarkGray;
		static const Color Clear;

		//==== IPrintable ====

		const std::string& ToString() const override
		{
			nlohmann::ordered_json json;
			Serialize(json);
			return json.dump(2);
		}

		//==== ISerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override
		{
			Serializer::Serialize<uint8_t>(byteArray, COLOR_SERIALIZE_VERSION);
			Serializer::Serialize<uint32_t>(byteArray, ToHex());
		}

		void Serialize(nlohmann::ordered_json& json) const override
		{
			json["R"] = R;
			json["G"] = G;
			json["B"] = B;
			json["A"] = A;
		}

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override
		{
			const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
			if (version > COLOR_SERIALIZE_VERSION)
			{
				fmt::print(stderr, fg(fmt::color::red), "Color::Deserialize() -> Color has unsupported version {} (Current version is {}).\n", version, COLOR_SERIALIZE_VERSION);
				return false;
			}

			Color color(Serializer::Deserialize<uint32_t>(byteArray, offset));
			R = color.R;
			G = color.G;
			B = color.B;
			A = color.A;

			return true;
		}

		bool Deserialize(nlohmann::ordered_json& json) override
		{
			R = json.value("R", 255);
			G = json.value("G", 255);
			B = json.value("B", 255);
			A = json.value("A", 255);

			return true;
		}

		//==== Operators ====

		Color operator*(float scalar) const
		{
			return Color(R * scalar, G * scalar, B * scalar, A);
		}

		bool operator==(const Color& other) const
		{
			return R == other.R && G == other.G && B == other.B && A == other.A;
		}
	};

	inline const Color Color::Black		= Color(0x000000FF);
	inline const Color Color::White		= Color(0xFFFFFFFF);
	inline const Color Color::Red		= Color(0xFF0000FF);
	inline const Color Color::Green		= Color(0x00FF00FF);
	inline const Color Color::Blue		= Color(0x0000FFFF);
	inline const Color Color::Magenta	= Color(0xFF00FFFF);
	inline const Color Color::Yellow	= Color(0xFFFF00FF);
	inline const Color Color::Cyan		= Color(0x00FFFFFF);
	inline const Color Color::LightGray	= Color(White * 0.75f);
	inline const Color Color::Gray		= Color(White * 0.50f);
	inline const Color Color::DarkGray	= Color(White * 0.25f);
	inline const Color Color::Clear		= Color(0x00000000);
}