#include "Neela/Essentials/Renderer.h"
#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Graphics/Model.h"
#include "Neela/Graphics/Transform.h"
#include "Neela/Maths/Rect.h"
#include "Neela/NeelaConstants.h"

#include <cassert>
#include <limits>

#define MODEL_SERIALIZE_VERSION 1

namespace Neela
{
	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	Model::Model(const std::weak_ptr<Texture>& texture, const std::vector<SDL_Vertex>& vertices, const std::vector<int>& indexes, std::uint8_t renderLayer) :
		m_Texture(texture),
		m_Vertices(vertices),
		m_Indices(indexes),
		m_RenderLayer(renderLayer)
	{
		assert(indexes.size());
		assert(Maths::Max(indexes) <= vertices.size());
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	const std::vector<SDL_Vertex>& Model::GetVertices() const
	{
		return m_Vertices;
	}

	void Model::SetVertices(const std::vector<SDL_Vertex>& vertices)
	{
		m_Vertices = vertices;
	}

	const std::vector<int>& Model::GetIndices() const
	{
		return m_Indices;
	}

	void Model::SetIndices(const std::vector<int>& indices)
	{
		m_Indices = indices;
	}

	//====================================================================================
	//==== IFILESERIALIZEABLE
	//====================================================================================

	void Model::Serialize(std::vector<uint8_t>& byteArray) const
	{
		Serializer::Serialize<uint8_t>(byteArray, MODEL_SERIALIZE_VERSION);

		Serializer::Serialize(byteArray, GetTexturePath());

		Serializer::Serialize<uint8_t>(byteArray, m_RenderLayer);

		Serializer::Serialize<uint32_t>(byteArray, m_Indices.size());
		for (size_t i = 0; i < m_Indices.size(); i++)
			Serializer::Serialize<uint32_t>(byteArray, m_Indices[i]);

		Serializer::Serialize<uint32_t>(byteArray, m_Vertices.size());
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			Vector2f(m_Vertices[i].position).Serialize(byteArray);
			Vector2f(m_Vertices[i].tex_coord).Serialize(byteArray);
			Color(m_Vertices[i].color).Serialize(byteArray);
		}
	}

	void Model::Serialize(nlohmann::ordered_json& json) const
	{
		json["Texture"] = GetTexturePath();

		json["Render Layer"] = GetRenderLayer();

		json["Indices Size"] = GetIndices().size();
		json["Indices"] = GetIndices();

		json["Vertices Size"] = GetVertices().size();
		for (auto& i : GetVertices())
		{
			nlohmann::ordered_json v;
			Vector2f(i.position).Serialize(v["Position"]);
			Vector2f(i.tex_coord).Serialize(v["UV"]);
			Color(i.color).Serialize(v["Color"]);
			json["Vertices"].push_back(v);
		}
	}

	bool Model::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > MODEL_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "Model::Deserialize() -> Model has unsupported version {} (Current version is {}).\n", version, MODEL_SERIALIZE_VERSION);
			return false;
		}

		SetTexture(ResourcesManager::GetTexture(WindowsManager::GetMainWindow(), Serializer::Deserialize<std::string>(byteArray, offset)));

		SetRenderLayer(Serializer::Deserialize<uint8_t>(byteArray, offset));

		const uint32_t indicesSize = Serializer::Deserialize<uint32_t>(byteArray, offset);
		std::vector<int> indices(indicesSize);
		for (size_t i = 0; i < indicesSize; i++)
			indices[i] = Serializer::Deserialize<uint32_t>(byteArray, offset);

		SetIndices(indices);

		const uint32_t verticesSize = Serializer::Deserialize<uint32_t>(byteArray, offset);
		std::vector<SDL_Vertex> vertices(verticesSize);
		for (size_t i = 0; i < verticesSize; i++)
		{
			SDL_Vertex v;

			Vector2f vec;
			vec.Deserialize(byteArray, offset);
			v.position = vec.ToSdlFPoint();

			vec.Deserialize(byteArray, offset);
			v.tex_coord = vec.ToSdlFPoint();

			Color col;
			col.Deserialize(byteArray, offset);
			v.color = col.ToSdlColor();

			vertices[i] = v;
		}

		SetVertices(vertices);

		return true;
	}

	bool Model::Deserialize(nlohmann::ordered_json& json)
	{
		SetTexture(ResourcesManager::GetTexture(WindowsManager::GetMainWindow(), json.value("Texture", "")));
		SetRenderLayer(json.value("Render Layer", 0));

		SetIndices(json.at("Indices"));

		std::vector<SDL_Vertex> vertices;
		vertices.reserve(json.value("Vertices Size", 0));
		for (auto& vertex : json.at("Vertices"))
		{
			SDL_Vertex v;

			Vector2f vec;
			vec.Deserialize(vertex["Position"]);
			v.position = vec.ToSdlFPoint();

			vec.Deserialize(vertex["UV"]);
			v.tex_coord = vec.ToSdlFPoint();

			Color col;
			col.Deserialize(vertex["Color"]);
			v.color = col.ToSdlColor();

			vertices.push_back(v);
		}

		SetVertices(vertices);

		return true;
	}

	bool Model::SaveToFileJson(const std::filesystem::path& filepath) const
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
			fmt::print(fg(fmt::color::red), "Model::SaveToFileJson() -> Failed to open Model file {}.\n", filepath.string());
			return false;
		}

		nlohmann::ordered_json json;
		Serialize(json);

		file << json.dump(2);
		file.close();

		fmt::print(fg(fmt::color::lime_green), "Model::SaveToFileJson() -> Saved Model to {}.\n", filepath.string());
		return true;
	}

	bool Model::SaveToFileBin(const std::filesystem::path& filepath) const
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
			fmt::print(fg(fmt::color::red), "Model::SaveToFileBin() -> Failed to open Model file {}.\n", filepath.string());
			return false;
		}

		std::vector<std::uint8_t> buffer;
		Serialize(buffer);
		file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

		file.close();

		fmt::print(fg(fmt::color::lime_green), "Model::SaveToFileBin() -> Saved Model to {}.\n", filepath.string());
		return true;
	}

	//====================================================================================
	//==== IPRINTABLE
	//====================================================================================

	const std::string& Model::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return json.dump(2);
	}

	//====================================================================================
	//==== IRENDERABLE
	//====================================================================================

	Rect Model::GetBounds() const
	{
		float smallestX = std::numeric_limits<float>().max();
		float smallestY = std::numeric_limits<float>().max();
		float biggestX = std::numeric_limits<float>().lowest();
		float biggestY = std::numeric_limits<float>().lowest();

		for (auto& v : m_Vertices)
		{
			if(v.position.x > biggestX)
				biggestX = v.position.x;

			if(v.position.x < smallestX)
				smallestX = v.position.x;

			if(v.position.y > biggestY)
				biggestY = v.position.y;

			if(v.position.y < smallestY)
				smallestY = v.position.y;
		}

		return Rect(Maths::Abs(biggestX - smallestX), Maths::Abs(biggestY - smallestY), smallestX, smallestY);
	}

	void Model::Draw(const Transform& transform) const
	{
		if (!m_Texture)
			return;

		if(m_Vertices.size() <= 0 || m_Indices.size() <= 0)
			return;

		std::vector<SDL_Vertex> transformed(m_Vertices);
		for (auto& vert : transformed)
			vert.position = transform.RelativeToAbsolutePosition(Vector2f(vert.position)).ToSdlFPoint();

		SDL_RenderGeometry(*(m_Texture->GetRenderer()), m_Texture->GetSdlTexture(), &transformed[0], m_Vertices.size(), &m_Indices[0], m_Indices.size());
	}

	const std::shared_ptr<Texture>& Model::GetTexture() const
	{
		return m_Texture;
	}

	void Model::SetTexture(const std::shared_ptr<Texture>& newTexture)
	{
		m_Texture = newTexture;
	}

	const std::string& Model::GetTexturePath() const
	{
		if (!m_Texture)
			return "";

		return m_Texture->GetPath();
	}

	std::uint8_t Model::GetRenderLayer() const
	{
		return m_RenderLayer;
	}

	void Model::SetRenderLayer(std::uint8_t newRenderLayer)
	{
		m_RenderLayer = newRenderLayer;
	}

	//====================================================================================
	//==== IINSPECTABLE
	//====================================================================================

	void Model::PopulateInspector()
	{
		if (!ImGui::TreeNode("Model"))
			return;

		Texture* texPtr = m_Texture.get();
		std::string texture = texPtr ? texPtr->GetPath() : "None";
		if (ImGui::InputTextWithHint("Texture file", "NeelaLogo.png", &texture[0], texture.size(), 32))
		{
			SetTexture(ResourcesManager::GetTexture((texPtr ? texPtr->GetWindow() : WindowsManager::GetMainWindow()), TEXTURES_ROOT + texture));
		}

		int layer = GetRenderLayer();
		if (ImGui::InputInt("Render Layer", &layer))
		{
			SetRenderLayer(layer);
		}

		ImGui::Text("Vertex Count : %u", m_Vertices.size());

		size_t i = 0;
		for (auto& v : m_Vertices)
		{
			const std::string s = fmt::format("Vertex {}", i);
			if (ImGui::TreeNode(s.c_str()))
			{
				int color[4] = { v.color.r, v.color.g, v.color.b, v.color.a };
				if (ImGui::SliderInt4("Color", color, 0, 255))
					v.color = { static_cast<unsigned char>(color[0]), static_cast<unsigned char>(color[1]), static_cast<unsigned char>(color[2]), static_cast<unsigned char>(color[3]) };

				float pos[2] = { v.position.x, v.position.y };
				if (ImGui::DragFloat2("Position", pos))
					v.position = { pos[0], pos[1] };

				float uv[2] = { v.tex_coord.x, v.tex_coord.y };
				if (ImGui::DragFloat2("UV", uv, 0.01f, 0, 1))
					v.tex_coord = { uv[0], uv[1] };

				ImGui::TreePop();
			}
			i++;
		}

		ImGui::TreePop();
	}
}