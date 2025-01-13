#include "Neela/Essentials/InputsManager.h"
#include "Neela/Essentials/WindowsManager.h"
#include "Neela/Essentials/Window.h"
#include "Neela/Inputs/InputAction.h"
#include "Neela/ImGuiRenderer.h"

#include "stdexcept"

#define INPUTSMANAGER_SERIALIZE_VERSION 1

namespace Neela
{
	std::shared_ptr<InputsManager> InputsManager::m_Instance = nullptr;

	//=============================================================================================
	//==== PUBLIC CONSTRUCTORS	
	//=============================================================================================

	InputsManager::InputsManager()
	{
		try
		{
			if (InputsManager::m_Instance != nullptr)
				throw std::runtime_error("InputManager::InputManager() -> An InputManager has already been instanciated.");
		}
		catch (const std::exception& e)
		{
			printf("%s", e.what());
			exit(EXIT_FAILURE);
		}

		// Custom deleter vide pour ne pas appeler le destructeur de InputsManager (qui aurait ete appele deux fois).
		InputsManager::m_Instance.reset(this, [](InputsManager*){});
	}

	InputsManager::~InputsManager()
	{
		DestroyBindings(true);
		m_Instance = nullptr;
	}

	//=============================================================================================
	//==== PUBLIC METHODS	
	//=============================================================================================

	void InputsManager::DestroyBindings(bool alsoDestroyActions)
	{
		m_MouseButtonsMap.clear();
		m_KeyboardKeysMap.clear();

		if (alsoDestroyActions)
			m_InputActionsMap.clear();
	}

	//=============================================================================================
	//==== PUBLIC STATIC METHODS	
	//=============================================================================================

	void InputsManager::PollEvents()
	{
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent))
		{
			for (auto& window : WindowsManager::GetWindows())
				window.second->GetImGuiRenderer()->ProcessEvent(sdlEvent);

			switch (sdlEvent.type)
			{
				case SDL_WINDOWEVENT:
					ProcessWindowEvent(sdlEvent);
					break;

				// Keyboard inputs
				case SDL_KEYDOWN:
				{
					if (sdlEvent.key.repeat != 0)
						continue;

					ProcessKeyDownInput(sdlEvent.key.keysym.sym);
					break;
				}

				case SDL_KEYUP:
				{
					ProcessKeyUpInput(sdlEvent.key.keysym.sym);
					break;
				}

				// Mouse inputs
				case SDL_MOUSEBUTTONDOWN:
				{
					ProcessMouseButtonDownInput(static_cast<EMouseButton>(sdlEvent.button.button));
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					ProcessMouseButtonUpInput(static_cast<EMouseButton>(sdlEvent.button.button));
					break;
				}

				default:
					break;
			}
		}

		ExecuteAllActiveInputs();

		return;
	}

	Vector2i InputsManager::GetMousePosition()
	{
		Vector2i pos;
		SDL_GetMouseState(&pos.X, &pos.Y);
		return pos;
	}

	bool InputsManager::IsActive(const std::string& actionName)
	{
		EnsureInstance("IsActive(const std::string& actionName)");

		if (InputsManager::m_Instance->m_InputActionsMap.empty())
			return false;

		auto target = InputsManager::m_Instance->m_InputActionsMap.find(actionName);
		if (target == InputsManager::m_Instance->m_InputActionsMap.end())
			return false;

		return target->second.m_Count > 0;
	}

	void InputsManager::ExecuteAllActiveInputs()
	{
		EnsureInstance("InputManager::ExecuteAllActiveInputs()");

		if (InputsManager::m_Instance->m_InputActionsMap.empty())
			return;

		for (auto it = InputsManager::m_Instance->m_InputActionsMap.begin(); it != InputsManager::m_Instance->m_InputActionsMap.end(); it++)
		{
			if (it->second.m_Count > 0)
				it->second.m_CodeToExecute(true);
		}
	}

	void InputsManager::BindKey(SDL_Keycode keycode, const std::string& actionName)
	{
		EnsureInstance("BindKeyDown(SDL_Keycode keycode, const std::string& actionName)");

		if (InputsManager::m_Instance->m_KeyboardKeysMap.empty())
		{
			InputsManager::m_Instance->m_KeyboardKeysMap.emplace(keycode, actionName);
			return;
		}

		auto target = InputsManager::m_Instance->m_KeyboardKeysMap.find(keycode);
		if (target != InputsManager::m_Instance->m_KeyboardKeysMap.end())
		{
			target->second = actionName;
		}
		else
		{
			InputsManager::m_Instance->m_KeyboardKeysMap.emplace(keycode, actionName);
		}
	}

	void InputsManager::BindMouseButton(EMouseButton button, const std::string& actionName)
	{
		EnsureInstance("BindMouseButtonDown(EMouseButton button, const std::string& actionName)");

		if (InputsManager::m_Instance->m_MouseButtonsMap.empty())
		{
			InputsManager::m_Instance->m_MouseButtonsMap.emplace(button, actionName);
			return;
		}

		auto target = InputsManager::m_Instance->m_MouseButtonsMap.find(button);
		if (target != InputsManager::m_Instance->m_MouseButtonsMap.end())
		{
			target->second = actionName;
		}
		else
		{
			InputsManager::m_Instance->m_MouseButtonsMap.emplace(button, actionName);
		}
	}

	void InputsManager::BindAction(const std::string& actionName, const std::function<void(bool)>& action)
	{
		EnsureInstance("BindAction(const std::string& actionName, const std::function<void(bool)>& action)");

		InputAction newAction
		{
			action,
			0
		};

		if (InputsManager::m_Instance->m_InputActionsMap.empty())
		{
			InputsManager::m_Instance->m_InputActionsMap.emplace(actionName, newAction);
			return;
		}

		auto target = InputsManager::m_Instance->m_InputActionsMap.find(actionName);
		if (target != InputsManager::m_Instance->m_InputActionsMap.end())
		{
			target->second.m_CodeToExecute = action;
		}
		else
		{
			InputsManager::m_Instance->m_InputActionsMap.emplace(actionName, newAction);
		}
	}

	void InputsManager::ProcessKeyDownInput(SDL_Keycode key)
	{
		EnsureInstance("ProcessKeyDownInput(SDL_Keycode key)");

		InputsManager::m_Instance->ProcessKeyInput(key, true);
	}

	void InputsManager::ProcessMouseButtonDownInput(EMouseButton button)
	{
		EnsureInstance("ProcessMouseButtonDownInput(EMouseButton button)");

		InputsManager::m_Instance->ProcessMouseButtonInput(button, true);
	}

	void InputsManager::ProcessKeyUpInput(SDL_Keycode key)
	{
		EnsureInstance("ProcessKeyUpInput(SDL_Keycode key)");

		InputsManager::m_Instance->ProcessKeyInput(key, false);
	}

	void InputsManager::ProcessMouseButtonUpInput(EMouseButton button)
	{
		EnsureInstance("ProcessMouseButtonUpInput(EMouseButton button)");

		InputsManager::m_Instance->ProcessMouseButtonInput(button, false);
	}

	const std::shared_ptr<InputsManager>& InputsManager::GetInstance()
	{
		return m_Instance;
	}

	//=============================================================================================
	//==== IPRINTABLE
	//=============================================================================================

	const std::string& InputsManager::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return fmt::format("{}", json.dump(2));
	}

	//=============================================================================================
	//==== ISERIALIZEABLE
	//=============================================================================================

	void InputsManager::Serialize(std::vector<uint8_t>& byteArray) const
	{
		// Version
		Serializer::Serialize<uint8_t>(byteArray, INPUTSMANAGER_SERIALIZE_VERSION);

		// Mouse inputs size
		Serializer::Serialize<uint8_t>(byteArray, m_MouseButtonsMap.size());

		// Mouse inputs
		for (auto& pair : m_MouseButtonsMap)
		{
			Serializer::Serialize<uint8_t>(byteArray, (uint8_t)pair.first);
			Serializer::Serialize(byteArray, pair.second);
		}

		// Keyboard inputs size
		Serializer::Serialize<uint8_t>(byteArray, m_KeyboardKeysMap.size());

		// Keyboard inputs
		for (auto& pair : m_KeyboardKeysMap)
		{
			Serializer::Serialize<int32_t>(byteArray, (int32_t)pair.first);
			Serializer::Serialize(byteArray, pair.second);
		}
	}

	void InputsManager::Serialize(nlohmann::ordered_json& j) const
	{
		nlohmann::ordered_json mouseButtonsDoc;
		for (auto& mouseInput : m_MouseButtonsMap)
			mouseButtonsDoc.push_back({ static_cast<std::uint8_t>(mouseInput.first), mouseInput.second });

		j["MouseInputs"] = mouseButtonsDoc;

		nlohmann::ordered_json keysDoc;
		for (auto& keysInput : m_KeyboardKeysMap)
			keysDoc.push_back({ static_cast<std::uint32_t>(keysInput.first), keysInput.second });

		j["KeyboardInputs"] = keysDoc;
	}

	bool InputsManager::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		DestroyBindings(false);

		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > INPUTSMANAGER_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "InputsManager::Deserialize() -> InputsManager has unsupported version {} (Current version is {}).\n", version, INPUTSMANAGER_SERIALIZE_VERSION);
			return false;
		}

		const uint8_t mouseInputsSize = Serializer::Deserialize<uint8_t>(byteArray, offset);

		for (size_t i = 0; i < mouseInputsSize; i++)
		{
			const EMouseButton butt = static_cast<EMouseButton>(Serializer::Deserialize<uint8_t>(byteArray, offset));
			const std::string name = Serializer::Deserialize<std::string>(byteArray, offset);
			BindMouseButton(butt, name);
		}

		const uint8_t keyboardInputsSize = Serializer::Deserialize<uint8_t>(byteArray, offset);

		for (size_t i = 0; i < keyboardInputsSize; i++)
		{
			const SDL_Keycode butt = static_cast<SDL_Keycode>(Serializer::Deserialize<int32_t>(byteArray, offset));
			const std::string name = Serializer::Deserialize<std::string>(byteArray, offset);
			BindKey(butt, name);
		}

		return true;
	}

	bool InputsManager::Deserialize(nlohmann::ordered_json& json)
	{
		DestroyBindings(false);

		for (auto& m : json["MouseInputs"])
			BindMouseButton(static_cast<EMouseButton>(m[0]), m[1]);

		for (auto& k : json["KeyboardInputs"])
			BindKey(static_cast<SDL_Keycode>(k[0]), k[1]);

		return true;
	}

	//=============================================================================================
	//==== IFILESERIALIZEABLE
	//=============================================================================================

	bool InputsManager::SaveToFileJson(const std::filesystem::path& filepath) const
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
				return false;
			}
		}

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			fmt::print(fg(fmt::color::red), "InputsManager::SaveToFileJson() -> Failed to open Inputs file {}.\n", filepath.string());
			return false;
		}

		nlohmann::ordered_json json;
		Serialize(json);

		file << json.dump(2);
		file.close();

		fmt::print(fg(fmt::color::lime_green), "InputsManager::SaveToFileJson() -> Saved Inputs to {}.\n", filepath.string());
		return true;
	}

	bool InputsManager::SaveToFileBin(const std::filesystem::path& filepath) const
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
			fmt::print(fg(fmt::color::red), "InputsManager::SaveToFileBin() -> Failed to open Inputs file {}.\n", filepath.string());
			return false;
		}

		std::vector<std::uint8_t> buffer;
		Serialize(buffer);
		file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

		file.close();

		fmt::print(fg(fmt::color::lime_green), "InputsManager::SaveToFileBin() -> Saved Inputs to {}.\n", filepath.string());
		return true;
	}

	//=============================================================================================
	//==== PRIVATE STATIC METHODS	
	//=============================================================================================

	void InputsManager::ProcessWindowEvent(const SDL_Event& windowEvent)
	{
		switch (windowEvent.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
				WindowsManager::CloseWindowByID(windowEvent.window.windowID);
				break;

			default:
				break;
		}
	}

	void InputsManager::EnsureInstance(const std::string& functionName)
	{
		try
		{
			if (!InputsManager::m_Instance)
				throw std::runtime_error("");
		}
		catch (const std::exception&)
		{
			printf("InputManager::%s -> No InputManager has been instanciated.\n", functionName.c_str());
			exit(2);
		}
	}

	//=============================================================================================
	//==== PRIVATE METHODS	
	//=============================================================================================

	void InputsManager::ProcessKeyInput(SDL_Keycode key, bool keyDown)
	{
		if (!InputsManager::m_Instance)
			throw std::runtime_error("InputManager::ProcessKeyInput() -> No InputManager has been instanciated.");

		if (InputsManager::m_Instance->m_KeyboardKeysMap.empty())
			return;

		auto targetName = InputsManager::m_Instance->m_KeyboardKeysMap.find(key);
		if (targetName == InputsManager::m_Instance->m_KeyboardKeysMap.end())
			return;

		auto targetAction = InputsManager::m_Instance->m_InputActionsMap.find(targetName->second);
		if (targetAction == InputsManager::m_Instance->m_InputActionsMap.end())
			return;

		if (targetAction->second.m_CodeToExecute)
			targetAction->second.m_CodeToExecute(keyDown);

		targetAction->second.m_Count += keyDown ? 1 : -1;
	}

	void InputsManager::ProcessMouseButtonInput(EMouseButton button, bool buttonDown)
	{
		if (!InputsManager::m_Instance)
			throw std::runtime_error("InputManager::ProcessMouseButtonInput() -> No InputManager has been instanciated.");

		if (InputsManager::m_Instance->m_MouseButtonsMap.empty())
			return;

		auto targetName = InputsManager::m_Instance->m_MouseButtonsMap.find(button);
		if (targetName == InputsManager::m_Instance->m_MouseButtonsMap.end())
			return;

		auto targetAction = InputsManager::m_Instance->m_InputActionsMap.find(targetName->second);
		if (targetAction == InputsManager::m_Instance->m_InputActionsMap.end())
			return;

		if (targetAction->second.m_CodeToExecute)
			targetAction->second.m_CodeToExecute(buttonDown);

		targetAction->second.m_Count += buttonDown ? 1 : -1;
	}
}
