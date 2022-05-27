#pragma once

#define MENU_PAGE_OPTION 8

#define MENU_RESET_ALL() for (int i = 1; i <= gpGlobals->maxClients; ++i) { gMenu[i].Hide(i); }

class CMenu
{
public:
	void Clear();

	void Create(std::string Title, bool Exit, void* CallbackFunction);

	void AddItem(int Item, std::string Text, bool Disabled = false);
	void AddList(std::vector<std::string> List);

	void Show(int EntityIndex);
	void Hide(int EntityIndex);
	bool Handle(int EntityIndex, int Key);

private:
	void Display(int EntityIndex, int Page);
	void ShowMenu(int EntityIndex, int Slots, int Time, std::string MenuText);
	void HideMenu(int EntityIndex);

	std::string				  m_Text;
	std::vector<std::string>  m_Data;
	std::vector<int>		  m_Info;
	std::vector<bool>         m_Skip;
	int						  m_Page = 0;
	bool					  m_Exit = false;
	void*					  m_Func = nullptr;
};

extern CMenu gMenu[33];