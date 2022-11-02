#include "precompiled.h"

CTranslate gTranslate;

void CTranslate::Load()
{
	this->m_Data.clear();

	std::ifstream File(TRANSLATE_FILE, std::ifstream::in);

	std::string Line, Key;

	std::string Language = "en";

	if (gCvars.GetLanguage()->string)
	{
		Language = gCvars.GetLanguage()->string;
	}

	while (std::getline(File, Line))
	{
		if (Line.length() > 0)
		{
			this->ReplaceAll(Line, "\\1", "\1");
			this->ReplaceAll(Line, "\\3", "\3");
			this->ReplaceAll(Line, "\\4", "\4");

			if (Line.at(0) == '"')
			{
				Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());

				Key = Line;
			}
			else if (Line.rfind(Language.c_str(), 0) == 0)
			{
				Line.erase(0, 3);

				Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());

				this->m_Data.insert(std::make_pair(Key, Line));
			}
		}
	}

	File.close();
}

void CTranslate::ReplaceAll(std::string& String, const std::string& From, const std::string& To)
{
	size_t StartPos = 0;

	while ((StartPos = String.find(From, StartPos)) != std::string::npos)
	{
		String.replace(StartPos, From.length(), To);

		StartPos += To.length();
	}
}

const char* CTranslate::Get(char* Text)
{
	if (Text)
	{
		auto it = this->m_Data.find(Text);

		if (it != this->m_Data.end())
		{
			if (it->second.c_str())
			{
				return it->second.c_str();
			}
		}
	}

	return Text;
}
