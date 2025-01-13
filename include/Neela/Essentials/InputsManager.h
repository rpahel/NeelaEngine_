#pragma once

#include "Neela/Enums/MouseButtonEnum.h"
#include "Neela/Essentials/Export.h"
#include "Neela/Inputs/InputAction.h"
#include "Neela/Interfaces/IFileSerializeable.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Maths/Vector.h"

#include <functional>
#include <SDL2/SDL.h>
#include <string>

namespace Neela
{
	class ImGuiRenderer;

	class NEELA_ENGINE_API InputsManager : public IPrintable, public IFileSerializeable
	{
	private:
		//==== Fields ====

		static std::shared_ptr<InputsManager> m_Instance;

		std::unordered_map<std::string, InputAction>	m_InputActionsMap;
		std::unordered_map<EMouseButton, std::string>	m_MouseButtonsMap;
		std::unordered_map<SDL_Keycode, std::string>	m_KeyboardKeysMap;

	public:
		//==== Constructors ====

		InputsManager();
		InputsManager(const InputsManager&) = delete;
		InputsManager(InputsManager&&) = delete;
		~InputsManager();

		//==== Methods ====

		void DestroyBindings(bool alsoDestroyActions);

		//==== Static Methods ====

		static Vector2i GetMousePosition();
		static void PollEvents();

		static bool IsActive(const std::string& actionName);
		static void ExecuteAllActiveInputs();

		static void BindKey(SDL_Keycode keycode, const std::string& actionName);
		static void BindMouseButton(EMouseButton button, const std::string& actionName);
		static void BindAction(const std::string& actionName, const std::function<void(bool /*active*/)>& action);

		static void ProcessKeyDownInput(SDL_Keycode key);
		static void ProcessMouseButtonDownInput(EMouseButton button);

		static void ProcessKeyUpInput(SDL_Keycode key);
		static void ProcessMouseButtonUpInput(EMouseButton button);

		static const std::shared_ptr<InputsManager>& GetInstance();

		//==== Operators ====

		InputsManager& operator=(const InputsManager&) = delete;
		InputsManager& operator=(InputsManager&&) = delete;

		//==== IPrintable ====

		const std::string& ToString() const override;

		//==== ISerializable ====

		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& j) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset) override;
		bool Deserialize(nlohmann::ordered_json& json) override;

		//==== IFileSerializable ====

		bool SaveToFileJson(const std::filesystem::path& filepath) const override;
		bool SaveToFileBin(const std::filesystem::path& filepath) const override;

	private:
		//==== Static Methods ====

		static void ProcessWindowEvent(const SDL_Event& windowEvent);
		static void EnsureInstance(const std::string& functionName);

		//==== Methods ====

		void ProcessKeyInput(SDL_Keycode key, bool keyDown);
		void ProcessMouseButtonInput(EMouseButton button, bool buttonDown);
	};
}
