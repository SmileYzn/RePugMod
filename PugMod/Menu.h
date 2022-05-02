#pragma once

#define MENU_PAGE_OPTION 8

#define MENU_RESET_ALL() for (int i = 1; i <= gpGlobals->maxClients; ++i) { gMenu[i].Hide(i); }

class CMenu
{
public:
	void Create(std::string Title, bool Exit, void* CallbackFunction);

	void AddItem(int Item, std::string Text);
	void AddList(std::vector<std::string> List);

	void Show(int EntityIndex);
	void Hide(int EntityIndex);
	bool Handle(int EntityIndex, int Key);

private:
	void Display(int EntityIndex, int Page);
	void ShowMenu(int EntityIndex, int Slots, int Time, char *Text, int Length);
	void HideMenu(int EntityIndex);

	std::string				  m_Text;
	std::vector<std::string>  m_Data;
	std::vector<int>		  m_Info;
	int						  m_Page;
	bool					  m_Exit;
	void*					  m_Func;
	int						  m_iMsg;
};

extern CMenu gMenu[33];