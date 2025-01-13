#pragma once

#include "Neela/Audio/AudioWave.h"
#include "Neela/Essentials/Export.h"
#include "Neela/Essentials/InputsManager.h"
#include "Neela/Interfaces/IFileSerializeable.h"

#include <entt/entt.hpp>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace Neela
{
	class Window;
	class Texture;

	class NEELA_ENGINE_API ResourcesManager
	{
	private:
		static ResourcesManager* m_Instance;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesMap;
		std::unordered_map<std::string, std::shared_ptr<AudioWave>> m_AudioWavesMap;

	public:
		//==== Constructors ====

		ResourcesManager();
		ResourcesManager(const ResourcesManager& copy) = delete;
		ResourcesManager(ResourcesManager&& victim) = delete;
		~ResourcesManager();

		//==== Static Methods ====

		static std::string SaveScene(const entt::registry& world, const std::filesystem::path& filepath);
		static void LoadScene(const std::filesystem::path& filepath);

		static std::shared_ptr<Texture> GetTexture(const std::weak_ptr<Window>& window, const std::string& path);
		static std::shared_ptr<AudioWave> GetWav(const std::string& path);
		static void Purge();

		//==== LoadFromFile Methods ====

		template<typename T>
		static std::shared_ptr<T> LoadFromFileBin(const std::filesystem::path& filepath);

		template<typename T>
		static std::shared_ptr<T> LoadFromFileJson(const std::filesystem::path& filepath);

		//==== Operators ====

		ResourcesManager& operator=(const ResourcesManager& copy) = delete;
		ResourcesManager& operator=(ResourcesManager&& victim) = delete;

	private:
		//==== Methods ====

		template <class T>
		void PurgeMap(std::unordered_map<std::string, std::shared_ptr<T>>& map);

		//==== Static Methods ====

		static void EnsureInstance(const std::string& functionName);
	};

	template<class T>
	inline std::shared_ptr<T> ResourcesManager::LoadFromFileBin(const std::filesystem::path& filepath)
	{
		std::ifstream inputFile(filepath, std::ios::binary);
		if (!inputFile.is_open())
		{
			fmt::print(stderr, fg(fmt::color::red), "ResourcesManager::LoadFromFileBin() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		std::vector<std::uint8_t> byteArray;
		inputFile.seekg(0, std::ios::end);
		std::size_t length = inputFile.tellg();
		inputFile.seekg(0, std::ios::beg);

		byteArray.resize(length);
		inputFile.read(reinterpret_cast<char*>(byteArray.data()), length);
		inputFile.close();

		T obj;
		T* objPtr = &obj;
		std::size_t offset = 0;
		if (IFileSerializeable* ser = dynamic_cast<IFileSerializeable*>(objPtr))
		{
			ser->Deserialize(byteArray, offset);
		}
		else
		{
			fmt::print(stderr, fg(fmt::color::red), "ResourcesManager::LoadFromFileBin() -> Type does not implement IFileSerializeable.\n");
			return nullptr;
		}

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileBin() -> Loaded file from {}.\n", filepath.string());
		return std::make_shared<T>(std::move(obj));
	}

	template<class T>
	inline std::shared_ptr<T> ResourcesManager::LoadFromFileJson(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "ResourcesManager::LoadFromFileJson() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		nlohmann::ordered_json doc = nlohmann::ordered_json::parse(file);
		file.close();

		T obj;
		T* objPtr = &obj;
		if (IFileSerializeable* ser = dynamic_cast<IFileSerializeable*>(objPtr))
		{
			ser->Deserialize(doc);
		}
		else
		{
			fmt::print(stderr, fg(fmt::color::red), "ResourcesManager::LoadFromFileJson() -> Type does not implement IFileSerializeable.\n");
			return nullptr;
		}

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileJson() -> Loaded file from {}.\n", filepath.string());
		return std::make_shared<T>(std::move(obj));
	}

	template<>
	inline std::shared_ptr<AudioWave> ResourcesManager::LoadFromFileBin<AudioWave>(const std::filesystem::path& filepath)
	{
		std::ifstream inputFile(filepath, std::ios::binary);
		if (!inputFile.is_open())
		{
			fmt::print(stderr, fg(fmt::color::red), "ResourcesManager::LoadFromFileBin<AudioWave>() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		std::vector<std::uint8_t> byteArray;
		inputFile.seekg(0, std::ios::end);
		std::size_t length = inputFile.tellg();
		inputFile.seekg(0, std::ios::beg);

		byteArray.resize(length);
		inputFile.read(reinterpret_cast<char*>(byteArray.data()), length);
		inputFile.close();

		AudioWave wave;
		std::size_t offset = 0;
		wave.Deserialize(byteArray, offset);

		std::shared_ptr<AudioWave> ptr = GetWav(wave.GetPath());

		if (!ptr)
			return nullptr;

		ptr->SetPitch(wave.GetPitch());
		ptr->SetVolume(wave.GetVolume());
		ptr->SetSamples(wave.GetSamples());

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileBin<AudioWave>() -> Loaded file from {}.\n", filepath.string());
		return ptr;
	}

	template<>
	inline std::shared_ptr<AudioWave> ResourcesManager::LoadFromFileJson<AudioWave>(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "ResourcesManager::LoadFromFileJson<AudioWave>() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		nlohmann::ordered_json doc = nlohmann::ordered_json::parse(file);
		file.close();

		AudioWave wave;
		wave.Deserialize(doc);

		std::shared_ptr<AudioWave> ptr = GetWav(wave.GetPath());

		if (!ptr)
			return nullptr;

		ptr->SetPitch(wave.GetPitch());
		ptr->SetVolume(wave.GetVolume());
		ptr->SetSamples(wave.GetSamples());

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileJson<AudioWave>() -> Loaded file from {}.\n", filepath.string());
		return ptr;
	}

	template<>
	inline std::shared_ptr<InputsManager> ResourcesManager::LoadFromFileBin<InputsManager>(const std::filesystem::path& filepath)
	{
		std::ifstream inputFile(filepath, std::ios::binary);
		if (!inputFile.is_open())
		{
			fmt::print(stderr, fg(fmt::color::red), "ResourcesManager::LoadFromFileBin() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		std::vector<std::uint8_t> byteArray;
		inputFile.seekg(0, std::ios::end);
		std::size_t length = inputFile.tellg();
		inputFile.seekg(0, std::ios::beg);

		byteArray.resize(length);
		inputFile.read(reinterpret_cast<char*>(byteArray.data()), length);
		inputFile.close();

		if(!InputsManager::GetInstance())
			return nullptr;

		std::size_t offset = 0;
		InputsManager::GetInstance()->Deserialize(byteArray, offset);

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileBin() -> Loaded file from {}.\n", filepath.string());
		return InputsManager::GetInstance();
	}

	template<>
	inline std::shared_ptr<InputsManager> ResourcesManager::LoadFromFileJson<InputsManager>(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "ResourcesManager::LoadFromFileJson() -> Failed to open file {}.\n", filepath.string());
			return nullptr;
		}

		nlohmann::ordered_json doc = nlohmann::ordered_json::parse(file);
		file.close();

		if (!InputsManager::GetInstance())
			return nullptr;

		InputsManager::GetInstance()->Deserialize(doc);

		fmt::print(fg(fmt::color::lime_green), "ResourcesManager::LoadFromFileJson() -> Loaded file from {}.\n", filepath.string());
		return std::shared_ptr<InputsManager>(InputsManager::GetInstance());
	}

	template<class T>
	inline void ResourcesManager::PurgeMap(std::unordered_map<std::string, std::shared_ptr<T>>& map)
	{
		if (map.empty())
			return;

		for (auto it = map.begin(); it != map.end();)
		{
			if (it->second.use_count() > 1)
			{
				++it;
			}
			else
			{
				it = map.erase(it);
			}
		}
	}
}
