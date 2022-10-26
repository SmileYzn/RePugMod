#pragma once

#define TRANSLATE_FILE "cstrike/addons/pugmod/lang.txt"

#define _T(String) (gTranslate.Get(String))

class CTranslate
{
public:
	void Load();

	void ReplaceAll(std::string& String, const std::string& From, const std::string& To);

	const char* Get(char* Text);

private:
	std::map<std::string, std::string> m_Data;
};

extern CTranslate gTranslate;