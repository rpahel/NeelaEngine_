#include "Neela/Graphics/Sprite.h"
#include "Neela/Essentials/Renderer.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Graphics/Transform.h"
#include "Neela/NeelaConstants.h"

#include <SDL2/SDL.h>

#define SPRITE_SERIALIZE_VERSION 1

namespace Neela
{
	//=============================================================================================
	//==== PUBLIC CONSTRUCTORS	
	//=============================================================================================

	Sprite::Sprite(const std::shared_ptr<Texture>& texture, Rect* sourceRect, std::uint8_t renderLayer) :
		m_RenderLayer(renderLayer)
	{
		Texture* tex = texture.get();

		m_Texture = texture;
		Vector2i size = tex ? tex->GetSize() : Vector2i(32, 32);
		m_SourceRect = sourceRect ? *sourceRect : Rect(size.X, size.Y, 0, 0);
		m_SpriteSize = sourceRect ? Vector2i(sourceRect->W, sourceRect->H) : Vector2i(m_SourceRect.W, m_SourceRect.H);
		m_Pivot = Vector2f(.5f);
		m_Color = Color::White;
	}

	//=============================================================================================
	//==== PUBLIC METHODS	
	//=============================================================================================

	void Sprite::SetSize(int newWidth, int newHeight)
	{
		m_SpriteSize = Vector2i(newWidth, newHeight);
	}

	void Sprite::SetSize(const Vector2f& newSize)
	{
		m_SpriteSize = newSize;
	}

	void Sprite::SetSourceRect(const Rect& newRect)
	{
		m_SourceRect = newRect;
	}

	void Sprite::SetPivot(const Vector2f& pivot)
	{
		m_Pivot = pivot;
	}

	void Sprite::SetPivot(float x, float y)
	{
		SetPivot(Vector2f(x, y));
	}

	const Vector2f& Sprite::GetPivot() const
	{
		return m_Pivot;
	}

	void Sprite::SetColor(const Color& color)
	{
		m_Color = color;
	}

	const Color& Sprite::GetColor() const
	{
		return m_Color;
	}

	const Vector2f& Sprite::GetSize() const
	{
		return m_SpriteSize;
	}

	const Rect& Sprite::GetSourceRect() const
	{
		return m_SourceRect;
	}

	void Sprite::SetOffset(const Vector2f& offset)
	{
		m_Offset = offset;
	}

	const Vector2f& Sprite::GetOffset() const
	{
		return m_Offset;
	}

	//=============================================================================================
	//==== IRENDERABLE
	//=============================================================================================

	Rect Sprite::GetBounds() const
	{
		return Rect(m_SpriteSize, m_Pivot * m_SpriteSize * -1);
	}

	void Sprite::Draw(const Vector2i& position) const
	{
		Draw(Transform(position));
	}

	void Sprite::Draw(const Transform& transform) const
	{
		if (!m_Texture)
			return;

		const SDL_Color color = SDL_Color{ m_Color.R, m_Color.G, m_Color.B, m_Color.A };
		const Vector2i	textureSize = m_Texture->GetSize();
		const Vector2f	pivotXsize = (m_Pivot * -1) * m_SpriteSize;

		Vector2f topLeft = transform.RelativeToAbsolutePosition(pivotXsize);
		Vector2f topRight = transform.RelativeToAbsolutePosition(pivotXsize + Vector2f(m_SpriteSize.X, 0));
		Vector2f bottomLeft = transform.RelativeToAbsolutePosition(pivotXsize + Vector2f(0, m_SpriteSize.Y));
		Vector2f bottomRight = transform.RelativeToAbsolutePosition(pivotXsize + m_SpriteSize);

		topLeft += m_Offset;
		topRight += m_Offset;
		bottomLeft += m_Offset;
		bottomRight += m_Offset;

		// Vertex position
		SDL_Vertex vertices[4]{};

		vertices[0].position = SDL_FPoint(topLeft.X, topLeft.Y);
		vertices[0].color = color;
		vertices[1].position = SDL_FPoint(topRight.X, topRight.Y);
		vertices[1].color = color;
		vertices[2].position = SDL_FPoint(bottomLeft.X, bottomLeft.Y);
		vertices[2].color = color;
		vertices[3].position = SDL_FPoint(bottomRight.X, bottomRight.Y);
		vertices[3].color = color;

		// UV
		const Vector2f factor = Vector2f::One() / textureSize;
		const Vector2f sourcePos = m_SourceRect.GetPositionVector();
		const Vector2f sourceSize = m_SourceRect.GetSizeVector();

		topLeft = sourcePos * factor;
		topRight = (sourcePos + Vector2f(sourceSize.X, 0)) * factor;
		bottomLeft = (sourcePos + Vector2f(0, sourceSize.Y)) * factor;
		bottomRight = (sourcePos + sourceSize) * factor;

		vertices[0].tex_coord = SDL_FPoint(topLeft.X, topLeft.Y);
		vertices[1].tex_coord = SDL_FPoint(topRight.X, topRight.Y);
		vertices[2].tex_coord = SDL_FPoint(bottomLeft.X, bottomLeft.Y);
		vertices[3].tex_coord = SDL_FPoint(bottomRight.X, bottomRight.Y);

		int indices[6]{ 0, 1, 2, 2, 1, 3 };

		SDL_RenderGeometry(*(m_Texture->GetRenderer()), m_Texture->GetSdlTexture(), vertices, 4, indices, 6);
	}

	void Sprite::SetTexture(const std::shared_ptr<Texture>& newTexture)
	{
		m_Texture = newTexture;
	}

	const std::shared_ptr<Texture>& Sprite::GetTexture() const
	{
		return m_Texture;
	}

	const std::string& Sprite::GetTexturePath() const
	{
		if (m_Texture)
			return m_Texture->GetPath();

		return "";
	}

	std::uint8_t Sprite::GetRenderLayer() const
	{
		return m_RenderLayer;
	}

	void Sprite::SetRenderLayer(std::uint8_t newRenderLayer)
	{
		m_RenderLayer = newRenderLayer;
	}

	//=============================================================================================
	//==== IPRINTABLE
	//=============================================================================================

	const std::string& Sprite::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return json.dump(2);
	}

	//=============================================================================================
	//==== ISERIALIZEABLE
	//=============================================================================================

	void Sprite::Serialize(std::vector<uint8_t>& byteArray) const
	{
		Serializer::Serialize<uint8_t>(byteArray, SPRITE_SERIALIZE_VERSION);
		Serializer::Serialize(byteArray, GetTexturePath());
		Serializer::Serialize<uint8_t>(byteArray, GetRenderLayer());
		GetSourceRect().Serialize(byteArray);
		GetSize().Serialize(byteArray);
		GetPivot().Serialize(byteArray);
		GetColor().Serialize(byteArray);
	}

	void Sprite::Serialize(nlohmann::ordered_json& json) const
	{
		json["Texture"] = GetTexturePath();
		json["Render Layer"] = GetRenderLayer();
		GetSourceRect().Serialize(json["Source Rect"]);
		GetSize().Serialize(json["Sprite Size"]);
		GetPivot().Serialize(json["Pivot"]);
		GetColor().Serialize(json["Color"]);
	}

	bool Sprite::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > SPRITE_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "Sprite::Deserialize() -> Sprite has unsupported version {} (Current version is {}).\n", version, SPRITE_SERIALIZE_VERSION);
			return false;
		}

		SetTexture(ResourcesManager::GetTexture(WindowsManager::GetMainWindow(), Serializer::Deserialize<std::string>(byteArray, offset)));
		SetRenderLayer(Serializer::Deserialize<uint8_t>(byteArray, offset));

		Rect rect;
		rect.Deserialize(byteArray, offset);
		SetSourceRect(rect);

		Vector2f size;
		size.Deserialize(byteArray, offset);
		SetSize(size);

		Vector2f pivot;
		pivot.Deserialize(byteArray, offset);
		SetPivot(pivot);

		Color color;
		color.Deserialize(byteArray, offset);
		SetColor(color);

		return true;
	}

	bool Sprite::Deserialize(nlohmann::ordered_json& json)
	{
		SetTexture(ResourcesManager::GetTexture(WindowsManager::GetMainWindow(), json.value("Texture", "")));
		SetRenderLayer(json.value("Render Layer", 0));

		Rect rect;
		rect.Deserialize(json.at("Source Rect"));
		SetSourceRect(rect);

		Vector2f size;
		size.Deserialize(json.at("Sprite Size"));
		SetSize(size);

		Vector2f pivot;
		pivot.Deserialize(json.at("Pivot"));
		SetPivot(pivot);

		Color color;
		color.Deserialize(json.at("Color"));
		SetColor(color);

		return true;
	}

	bool Sprite::SaveToFileJson(const std::filesystem::path& filepath) const
	{
		if (!std::filesystem::exists(filepath))
		{
			std::filesystem::path folders(filepath);
			folders.remove_filename();
			try
			{
				std::filesystem::create_directories(folders);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				fmt::print(fg(fmt::color::red), "{}\n", e.what());
				return false;
			}
		}

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "Sprite::SaveToFileJson() -> Failed to open Sprite file {}.\n", filepath.string());
			return false;
		}

		nlohmann::ordered_json json;
		Serialize(json);

		file << json.dump(2);
		file.close();

		fmt::print(fg(fmt::color::lime_green), "Sprite::SaveToFileJson() -> Saved Sprite to {}.\n", filepath.string());
		return true;
	}

	bool Sprite::SaveToFileBin(const std::filesystem::path& filepath) const
	{
		if (!std::filesystem::exists(filepath))
		{
			std::filesystem::path folders(filepath);
			folders.remove_filename();
			try
			{
				std::filesystem::create_directories(folders);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				fmt::print(fg(fmt::color::red), "{}\n", e.what());
				return false;
			}
		}

		std::ofstream file(filepath, std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "Sprite::SaveToFileBin() -> Failed to open Sprite file {}.\n", filepath.string());
			return false;
		}

		std::vector<std::uint8_t> buffer;
		Serialize(buffer);
		file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

		file.close();

		fmt::print(fg(fmt::color::lime_green), "Sprite::SaveToFileBin() -> Saved Sprite to {}.\n", filepath.string());
		return true;
	}

	//=============================================================================================
	//==== IINSPECTABLE
	//=============================================================================================

	void Sprite::PopulateInspector()
	{
		if (!ImGui::TreeNode("Sprite"))
			return;

		Texture* texPtr = m_Texture.get();
		std::string texture = texPtr ? texPtr->GetPath() : "None";
		if (ImGui::InputTextWithHint("Texture file", "NeelaLogo.png", &texture[0], texture.size(), 32))
		{
			SetTexture(ResourcesManager::GetTexture((texPtr ? texPtr->GetWindow() : WindowsManager::GetMainWindow()), TEXTURES_ROOT + texture));
		}

		texPtr = m_Texture.get();

		int layer = GetRenderLayer();
		if (ImGui::InputInt("Render Layer", &layer))
		{
			SetRenderLayer(layer);
		}

		int source_rect[4] = { m_SourceRect.X, m_SourceRect.Y, m_SourceRect.W, m_SourceRect.H };
		if (ImGui::SliderInt4("Source Rect", source_rect, 0, texPtr ? texPtr->GetSize().GetMax() : 0))
			SetSourceRect(Rect(source_rect[2], source_rect[3], source_rect[0], source_rect[1]));

		float sizeArray[2] = { m_SpriteSize.X, m_SpriteSize.Y };
		if (ImGui::DragFloat2("Sprite Size", sizeArray, 0.1f, 0, 0, "%.2f"))
			SetSize(Vector2f(Maths::Clamp(sizeArray[0], 0, std::numeric_limits<float>().max()), Maths::Clamp(sizeArray[1], 0, std::numeric_limits<float>().max())));

		float pivotArray[2] = { m_Pivot.X, m_Pivot.Y };
		if (ImGui::SliderFloat2("Sprite Pivot", pivotArray, 0, 1, "%.2f"))
			SetPivot(Vector2f(pivotArray[0], pivotArray[1]));

		int color[4] = { m_Color.R, m_Color.G, m_Color.B, m_Color.A };
		if (ImGui::SliderInt4("Color", color, 0, 255))
			SetColor(Color(color[0], color[1], color[2], color[3]));

		ImGui::TreePop();
	}
}