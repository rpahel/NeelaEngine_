#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Graphics/Texture.h"
#include "Neela/Graphics/Color.h"
#include "Neela/Interfaces/IRenderable.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Interfaces/IInspectable.h"

#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>

namespace Neela
{
	class Transform;
	struct Rect;

	class NEELA_ENGINE_API Model : public IRenderable, public IFileSerializeable, public IPrintable, public IInspectable
	{
		std::shared_ptr<Texture> m_Texture;
		std::vector<SDL_Vertex> m_Vertices;
		std::vector<int> m_Indices;
		std::uint8_t m_RenderLayer = 0;

	public:
		//==== Constructors ====

		Model() = default;
		explicit Model(const std::weak_ptr<Texture>& texture, const std::vector<SDL_Vertex>& vertices, const std::vector<int>& indexes, std::uint8_t renderLayer = 0);

		//==== Methods ====

		const std::vector<SDL_Vertex>& GetVertices() const;
		void SetVertices(const std::vector<SDL_Vertex>& vertices);

		const std::vector<int>& GetIndices() const;
		void SetIndices(const std::vector<int>& indices);

		//==== IFileSerializeable ====

		void Serialize(std::vector<uint8_t>& byteArray) const;
		void Serialize(nlohmann::ordered_json& json) const;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		bool SaveToFileJson(const std::filesystem::path& filepath) const;
		bool SaveToFileBin(const std::filesystem::path& filepath) const;

		//==== IPrintable ====
		
		const std::string& ToString() const;
		
		//==== IRenderable ====

		Rect GetBounds() const override;
		void Draw(const Transform& transform) const override;

		const std::shared_ptr<Texture>& GetTexture() const override;
		void SetTexture(const std::shared_ptr<Texture>& newTexture) override;
		const std::string& GetTexturePath() const override;
		std::uint8_t GetRenderLayer() const override;
		void SetRenderLayer(std::uint8_t newRenderLayer) override;

		//==== IInspectable ====

		void PopulateInspector() override;
	};
}