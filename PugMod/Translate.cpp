#include "precompiled.h"

CTranslate gTranslate;

void CTranslate::Load()
{
	this->m_Data.clear();

	std::ifstream File(TRANSLATE_FILE, std::ifstream::in|std::ifstream::skipws);

	std::string Line, Header;

	while (std::getline(File, Line))
	{
		if (Line.length() > 0)
		{
			if (Line.at(0) == '"')
			{
				Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());

				Header = Line;
			}
			else if (Line.rfind("bp:", 0) == 0)
			{
				Line.erase(0, 3);

				Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());

				this->m_Data.insert(std::make_pair(Header, Line));
			}
		}
	}

	File.close();
}

const char* CTranslate::Get(const char* Text)
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
