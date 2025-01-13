#pragma once

#include "Neela/Interfaces/ISerializeable.h"

#include <fstream>

namespace Neela
{
	class NEELA_ENGINE_API IFileSerializeable : public ISerializeable
	{
	public:
		virtual bool SaveToFileJson(const std::filesystem::path& filepath) const = 0;
		virtual bool SaveToFileBin(const std::filesystem::path& filepath) const = 0;
	};
}