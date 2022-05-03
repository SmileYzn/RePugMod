#include "precompiled.h"

CMenu gMenu[33];

void CMenu::Clear()
{
	this->m_Text.clear();

	this->m_Data.clear();

	this->m_Info.clear();

	this->m_Skip.clear();

	this->m_Page = -1;

	this->m_Exit = false;

	this->m_Func = nullptr;
}

void CMenu::Create(std::string Title, bool Exit, void* CallbackFunction)
{
	this->Clear();

	this->m_Text = Title;

	this->m_Page = -1;

	this->m_Exit = Exit;

	this->m_Func = CallbackFunction; 
}

void CMenu::AddItem(int Item, std::string Text, bool Disabled)
{
	this->m_Data.push_back(Text);

	this->m_Info.push_back(Item);

	this->m_Skip.push_back(Disabled);
}

void CMenu::AddList(std::vector<std::string> List)
{
	this->m_Data.clear();

	this->m_Info.clear();

	this->m_Skip.clear();

	for (size_t i = 0; i < List.size(); i++)
	{
		this->AddItem(i, List[i].c_str(), false);
	}
}

void CMenu::Show(int EntityIndex)
{
	if (this->m_Data.size() && this->m_Info.size())
	{
		this->Display(EntityIndex, 0);
	}
}

void CMenu::Hide(int EntityIndex)
{
	this->m_Page = -1;

	this->HideMenu(EntityIndex);
}

bool CMenu::Handle(int EntityIndex, int Key)
{
	if (this->m_Page != -1)
	{
		auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

		if (Player)
		{
			if (Player->m_iMenu == Menu_OFF)
			{
				if (Key == 9)
				{
					this->Display(EntityIndex, ++this->m_Page);
				}
				else if (Key == 10)
				{
					this->Display(EntityIndex, --this->m_Page);
				}
				else
				{
					unsigned int ItemIndex = (Key + (this->m_Page * MENU_PAGE_OPTION)) - 1;

					if (ItemIndex < this->m_Info.size())
					{
						this->Hide(EntityIndex);

						if (this->m_Func)
						{
							((void(*)(int, int, bool, const char*))this->m_Func)(EntityIndex, this->m_Info[ItemIndex], this->m_Skip[ItemIndex], this->m_Data[ItemIndex].c_str());
						}
					}
				}

				return true;
			}
			else
			{
				this->m_Page = -1;
			}
		}
	}

	return false;
}

void CMenu::Display(int EntityIndex,int Page)
{
	if (Page < 0)
	{
		Page = 0;

		this->m_Page = 0;

		if (this->m_Exit)
		{
			this->m_Page = -1;
			return;
		}
	}
	else
	{
		this->m_Page = Page;
	}

	unsigned int Start = (Page * MENU_PAGE_OPTION);

	if (Start >= this->m_Data.size())
	{
		Start = Page = this->m_Page = 0;
	}

	char MenuText[512] = { 0 };

	int PageCount = this->m_Data.size() > MENU_PAGE_OPTION ? (this->m_Data.size() / MENU_PAGE_OPTION + ((this->m_Data.size() % MENU_PAGE_OPTION) ? 1 : 0)) : 1;

	int Len = sprintf(MenuText, "\\y%s\\R%i/%i\n\\w\n", this->m_Text.c_str(), (Page + 1), PageCount);

	unsigned int End = (Start + MENU_PAGE_OPTION);

	if (End > this->m_Data.size())
	{
		End = this->m_Data.size();
	}

	int Slots = (1 << 9); // MENU_KEY_0
	int BitSum = 0;

	for (unsigned int b = Start; b < End; b++)
	{
		Slots |= (1 << BitSum);

		if (this->m_Skip[b])
		{
			Len += sprintf(MenuText, "%s\\r%i.\\d %s\n", MenuText, ++BitSum, this->m_Data[b].c_str());
		}
		else
		{
			Len += sprintf(MenuText, "%s\\r%i.\\w %s\n", MenuText, ++BitSum, this->m_Data[b].c_str());
		}
	}

	if (End != this->m_Data.size())
	{
		Slots |= (1 << 8); // MENU_KEY_9; 

		if (Page)
		{
			Len += sprintf(MenuText, "%s%s", MenuText, "\n\\r9.\\w Next\n\\r0.\\w Back");
		}
		else
		{
			Len += sprintf(MenuText, "%s%s", MenuText, "\n\\r9.\\w Next");

			if (this->m_Exit)
			{
				Len += sprintf(MenuText, "%s\n\\r0.\\w Exit", MenuText);
			}
		}
	}
	else
	{
		if (Page)
		{
			Len += sprintf(MenuText, "%s\n\\r0.\\w Back", MenuText);
		}
		else
		{
			if (this->m_Exit)
			{
				Len += sprintf(MenuText, "%s\n\\r0.\\w Exit", MenuText);
			}
		}
	}

	this->ShowMenu(EntityIndex, Slots, -1, MenuText, Len);
}

void CMenu::ShowMenu(int EntityIndex, int Slots, int Time, char *Text, int Length)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Player->IsDormant())
		{
			static int iMsgShowMenu;

			if (iMsgShowMenu || (iMsgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", NULL)))
			{
				Player->m_iMenu = Menu_OFF;

				char *n = Text;
				char c = 0;
				int a;

				do
				{
					a = Length;

					if (a > MAX_BUFFER_MENU)
					{
						a = MAX_BUFFER_MENU;
					}

					Length -= a;
					c = *(n += a);
					*n = 0;

					MESSAGE_BEGIN(MSG_ONE, iMsgShowMenu, NULL, Player->edict());
					WRITE_SHORT(Slots);
					WRITE_CHAR(Time);
					WRITE_BYTE(c ? 1 : 0);
					WRITE_STRING(Text);
					MESSAGE_END();

					*n = c;
					Text = n;
				} while (*n);
			}
		}
	}
}

void CMenu::HideMenu(int EntityIndex)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Player->IsDormant())
		{
			static int iMsgShowMenu;

			if (iMsgShowMenu || (iMsgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", NULL)))
			{
				Player->m_iMenu = Menu_OFF;

				MESSAGE_BEGIN(MSG_ONE, iMsgShowMenu, nullptr, Player->edict());
				WRITE_SHORT(0);
				WRITE_CHAR(0);
				WRITE_BYTE(0);
				WRITE_STRING("");
				MESSAGE_END();
			}
		}
	}
}
