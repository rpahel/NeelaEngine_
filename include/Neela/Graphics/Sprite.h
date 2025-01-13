#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Graphics/Color.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/IRenderable.h"
#include "Neela/Interfaces/IInspectable.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Maths/Rect.h"
#include "Neela/Maths/Vector.h"

#include <memory>
#include <nlohmann/json.hpp>

namespace Neela
{
	class Transform;

	class NEELA_ENGINE_API Sprite : public IRenderable, public IPrintable, public IFileSerializeable, public IInspectable
	{
	private:
		std::shared_ptr<Texture> m_Texture;
		Rect		m_SourceRect;
		Vector2f	m_SpriteSize;
		Vector2f	m_Pivot;
		Color		m_Color;
		std::uint8_t m_RenderLayer = 0;

		// TODO : Serialize
		Vector2f	m_Offset;

	public:
		//==== Constructors ====

		Sprite() = default;
		explicit Sprite(const std::shared_ptr<Texture>& texture, Rect* sourceRect = nullptr, std::uint8_t renderLayer = 0);

		//==== Methods ====

		void		SetSize(int newWidth, int newHeight);
		void		SetSize(const Vector2f& newSize);
		void		SetSourceRect(const Rect& newRect);
		void		SetPivot(const Vector2f& pivot);
		void		SetPivot(float x, float y);
		void		SetColor(const Color& color);
		const Color&		GetColor() const;
		const Vector2f&	GetPivot() const;
		const Vector2f&	GetSize() const;
		const Rect&		GetSourceRect() const;

		void SetOffset(const Vector2f& offset);
		const Vector2f&	GetOffset() const;

		//==== IRenderable ====

		Rect		GetBounds() const override;
		void		Draw(const Vector2i& position) const override;
		void		Draw(const Transform& transform) const override;

		void		SetTexture(const std::shared_ptr<Texture>& newTexture) override;
		const std::shared_ptr<Texture>& GetTexture() const override;
		const std::string& GetTexturePath() const override;
		std::uint8_t GetRenderLayer() const override;
		void SetRenderLayer(std::uint8_t newRenderLayer);

		//==== IPrintable ====

		const std::string&	ToString() const override;

		//==== IFileSerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& json) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		bool SaveToFileJson(const std::filesystem::path& filepath) const override;
		bool SaveToFileBin(const std::filesystem::path& filepath) const override;

		//==== IInspectable ====

		void PopulateInspector() override;
	};
}