#include "Neela/Essentials/ResourcesManager.h"
#include "Neela/NeelaCore.h"

#include <stdexcept>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

namespace Neela
{
	ResourcesManager* ResourcesManager::m_Instance;

	//=============================================================================================
	//==== PUBLIC CONSTRUCTORS	
	//=============================================================================================

	ResourcesManager::ResourcesManager()
	{
		try
		{
			if (ResourcesManager::m_Instance != nullptr)
				throw std::runtime_error("ResourcesManager::ResourcesManager() -> A ResourcesManager has already been instanciated.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(2);
		}

		ResourcesManager::m_Instance = this;
	}

	ResourcesManager::~ResourcesManager()
	{
		Purge();
		ResourcesManager::m_Instance = nullptr;
	}

	//=============================================================================================
	//==== PUBLIC STATIC METHODS	
	//=============================================================================================

	std::string ResourcesManager::SaveScene(const entt::registry& world, const std::filesystem::path& filepath)
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
				fmt::print(fg(fmt::color::red), "{}", e.what());
			}
		}

		nlohmann::ordered_json doc;

		for (auto&& [entity] : world.storage<entt::entity>()->each())
		{
			nlohmann::json ent;

			if (const CameraComponent* ptr = world.try_get<CameraComponent>(entity))
				ent["CameraComponent"] = *ptr;

			//if (const TransformComponent* ptr = world.try_get<TransformComponent>(entity))
			//	ent["TransformComponent"] = *ptr;

			//if (const PhysicsComponent* ptr = world.try_get<PhysicsComponent>(entity))
			//	ent["PhysicsComponent"] = *ptr;

			//if (const SpriteSheetComponent* ptr = world.try_get<SpriteSheetComponent>(entity))
			//	ent["SpriteSheetComponent"] = *ptr;

			//if (const ModelComponent* ptr = world.try_get<ModelComponent>(entity))
			//	ent["ModelComponent"] = *ptr;

			if (ent.empty())
				continue;

			doc["Entities"].push_back(ent);
		}

		std::ofstream file(filepath);
		file << doc.dump(2);
		file.close();

		std::string canon = std::filesystem::canonical(filepath).string();
		fmt::print(fg(fmt::color::lime_green), "Saved Scene to {}.\n", canon);
		return canon;
	}

	void ResourcesManager::LoadScene(const std::filesystem::path& filepath)
	{
		std::filesystem::path canonPath;

		// Recuperation du fichier
		try
		{
			canonPath = std::filesystem::canonical(filepath);
		}
		catch (std::exception& e)
		{
			fmt::print(fg(fmt::color::red), "{}\n", e.what());
			exit(EXIT_FAILURE);
		}

		std::ifstream inputFile(canonPath);

		if (!inputFile.is_open())
		{
			fmt::print(fg(fmt::color::red), "Could not open {}\n", filepath.string());
			inputFile.close();
			exit(EXIT_FAILURE);
		}

		// Parsing
		nlohmann::ordered_json doc = nlohmann::ordered_json::parse(inputFile);
		inputFile.close();

		// Creation des entites
		for (auto& i : doc["Entities"])
		{
			auto it = i.find("NameComponent");
			if (it == i.end())
			{
				fmt::print(fg(fmt::color::red), "ResourcesManager::LoadScene() -> Could not find a NameComponent in the Json for an entity.");
				continue;
			}

			entt::handle newEntt = EntityCreator::CreateEntity(Core::World(), it->value("Name", "NULL"));

			it = i.find("CameraComponent");
			if (it != i.end())
				newEntt.emplace<CameraComponent>(it->template get<CameraComponent>());

			//it = i.find("TransformComponent");
			//if (it != i.end())
			//	newEntt.emplace<TransformComponent>(it->template get<TransformComponent>());

			//it = i.find("PhysicsComponent");
			//if (it != i.end())
			//	newEntt.emplace<PhysicsComponent>(it->template get<PhysicsComponent>());

			//it = i.find("SpriteSheetComponent");
			//if (it != i.end())
			//{
			//	newEntt.emplace<SpriteSheetComponent>(it->template get<SpriteSheetComponent>());
			//	std::shared_ptr<Sprite>& s = newEntt.get<SpriteSheetComponent>().m_Sprite;
			//	s->SetTexture(GetTexture(WindowsManager::GetWindowByTitle(it.value().at("Sprite").value("Window", "")), it.value().at("Sprite").value("Texture", "")));
			//	s->SetRenderLayer(it.value().at("Sprite").value("RenderLayer", 0));
			//	newEntt.emplace<GraphicsComponent>(s);
			//}

			//it = i.find("ModelComponent");
			//if (it != i.end())
			//{
			//	newEntt.emplace<ModelComponent>(it->template get<ModelComponent>());
			//	std::shared_ptr<Model>& m = newEntt.get<ModelComponent>().m_Model;
			//	m->SetTexture(GetTexture(WindowsManager::GetWindowByTitle(it.value().at("Model").value("Window", "")), it.value().at("Model").value("Texture", "")));
			//	m->SetRenderLayer(it.value().at("Model").value("RenderLayer", 0));
			//	newEntt.emplace<GraphicsComponent>(m);
			//}
		}

		//for (int i = doc["Entities"].size() - 1; i >= 0; i--)
		//{
		//	entt::handle newEntt = EntityCreator::CreateEntity(Core::World());

		//	auto it = doc["Entities"][i].find("CameraComponent");
		//	if (it != doc["Entities"][i].end())
		//		newEntt.emplace<CameraComponent>(it->template get<CameraComponent>());

		//	it = doc["Entities"][i].find("GravityComponent");
		//	if (it != doc["Entities"][i].end())
		//		newEntt.emplace<GravityComponent>(it->template get<GravityComponent>());

		//	it = doc["Entities"][i].find("TransformComponent");
		//	if (it != doc["Entities"][i].end())
		//		newEntt.emplace<TransformComponent>(it->template get<TransformComponent>());

		//	it = doc["Entities"][i].find("PhysicsComponent");
		//	if (it != doc["Entities"][i].end())
		//		newEntt.emplace<PhysicsComponent>(it->template get<PhysicsComponent>());

		//	it = doc["Entities"][i].find("SpriteSheetComponent");
		//	if (it != doc["Entities"][i].end())
		//	{
		//		newEntt.emplace<SpriteSheetComponent>(it->template get<SpriteSheetComponent>());
		//		std::shared_ptr<Sprite>& s = newEntt.get<SpriteSheetComponent>().m_Sprite;
		//		s->SetTexture(GetTexture(WindowsManager::GetWindowByTitle(it.value().at("Sprite").value("Window", "")), it.value().at("Sprite").value("Texture", "")));
		//		s->SetRenderLayer(it.value().at("Sprite").value("RenderLayer", 0));
		//		newEntt.emplace<GraphicsComponent>(s);
		//	}

		//	it = doc["Entities"][i].find("ModelComponent");
		//	if (it != doc["Entities"][i].end())
		//	{
		//		newEntt.emplace<ModelComponent>(it->template get<ModelComponent>());
		//		std::shared_ptr<Model>& m = newEntt.get<ModelComponent>().m_Model;
		//		m->SetTexture(GetTexture(WindowsManager::GetWindowByTitle(it.value().at("Model").value("Window", "")), it.value().at("Model").value("Texture", "")));
		//		m->SetRenderLayer(it.value().at("Model").value("RenderLayer", 0));
		//		newEntt.emplace<GraphicsComponent>(m);
		//	}
		//}

		// Parentage des transforms
		for (auto&& [entity] : Core::World().storage<entt::entity>().each())
		{
			const TransformComponent* ptr0 = Core::World().try_get<TransformComponent>(entity);
			if (!ptr0)
				continue;

			if (ptr0->m_Transform->GetParentId() == 0)
				continue;

			for (auto&& [entity] : Core::World().storage<entt::entity>().each())
			{
				const TransformComponent* ptr1 = Core::World().try_get<TransformComponent>(entity);

				if (!ptr1)
					continue;

				if (ptr1->m_Transform->GetId() == ptr0->m_Transform->GetParentId())
					ptr0->m_Transform->SetParent(ptr1->m_Transform.get());
			}
		}

		fmt::print(fg(fmt::color::lime_green), "Successfully loaded scene from {}\n", canonPath.string());
	}

	std::shared_ptr<Texture> ResourcesManager::GetTexture(const std::weak_ptr<Window>& window, const std::string& path)
	{
		assert(path != "");

		EnsureInstance("GetTexture(const Renderer& renderer, const std::string& path)");

		if (!ResourcesManager::m_Instance->m_TexturesMap.empty())
		{
			auto target = ResourcesManager::m_Instance->m_TexturesMap.find(path);

			if (target != ResourcesManager::m_Instance->m_TexturesMap.end())
				return target->second;
		}

		std::shared_ptr<Texture> ptr = std::make_shared<Texture>(Texture::LoadFromFile(window, path.c_str()));
		ResourcesManager::m_Instance->m_TexturesMap.emplace(path, ptr);
		ptr->m_Path = path;
		return ptr;
	}

	std::shared_ptr<AudioWave> ResourcesManager::GetWav(const std::string& path)
	{
		assert(path != "");

		EnsureInstance("GetWav(const std::string& path)");

		if (!ResourcesManager::m_Instance->m_AudioWavesMap.empty())
		{
			auto target = ResourcesManager::m_Instance->m_AudioWavesMap.find(path);

			if (target != ResourcesManager::m_Instance->m_AudioWavesMap.end())
				return target->second;
		}

		try
		{
			std::filesystem::canonical(path);
		}
		catch (std::exception& e)
		{
			fmt::print(fg(fmt::color::red), "{} could not be found.\n", path);
			return nullptr;
		}

		drwav wavFile;
		drwav_init_file(&wavFile, path.c_str(), nullptr);

		std::vector<std::int16_t> samples(wavFile.channels * wavFile.totalPCMFrameCount);
		drwav_read_pcm_frames_s16(&wavFile, wavFile.totalPCMFrameCount, &samples[0]);

		std::shared_ptr<AudioWave> ptr = std::make_shared<AudioWave>(path, samples);

		ResourcesManager::m_Instance->m_AudioWavesMap.emplace(path, ptr);
		return ptr;
	}

	void ResourcesManager::Purge()
	{
		EnsureInstance("Purge()");
		ResourcesManager::m_Instance->PurgeMap(ResourcesManager::m_Instance->m_TexturesMap);
		ResourcesManager::m_Instance->PurgeMap(ResourcesManager::m_Instance->m_AudioWavesMap);
	}

	//=============================================================================================
	//==== PRIVATE STATIC METHODS
	//=============================================================================================

	void ResourcesManager::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (!ResourcesManager::m_Instance)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("ResourcesManager::%s -> No ResourcesManager has been instanciated.\n", functionName.c_str());
			exit(EXIT_FAILURE);
		}
	}
}