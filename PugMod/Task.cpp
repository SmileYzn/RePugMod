#include "precompiled.h"

CTask gTask;

CTask::CTask()
{
	this->Clear();
}

void CTask::Clear()
{
	this->m_Data.clear();
}

void CTask::Create(int Index, float Time, bool Loop, void* FunctionCallback)
{
	this->Create(Index, Time, Loop, FunctionCallback, NULL);
}

void CTask::Create(int Index, float Time, bool Loop, void* FunctionCallback, const char* FunctionParameter)
{
	if (FunctionCallback)
	{
		P_TASK_INFO Task = { 0 };

		Task.Index = Index;
		Task.Time = Time;
		Task.EndTime = gpGlobals->time + Time;
		Task.Loop = Loop;
		Task.Remove = false;
		Task.FunctionCallback = FunctionCallback;

		if (FunctionParameter)
		{
			memcpy(Task.FunctionParameter, FunctionParameter, sizeof(Task.FunctionParameter));
		}

		this->m_Data.insert(std::make_pair(Task.Index, Task));
	}
}

bool CTask::Exists(int Index)
{
	auto it = this->m_Data.find(Index);

	return (it != this->m_Data.end());
}

void CTask::Remove(int Index)
{
	if(this->m_Data.find(Index) != this->m_Data.end())
	{
		this->m_Data[Index].Remove = true;
	}
}

float CTask::Timeleft(int Index)
{
	auto it = this->m_Data.find(Index);

	if (it != this->m_Data.end())
	{
		return (it->second.EndTime - gpGlobals->time);
	}

	return 0.0f;
}

P_TASK_INFO CTask::GetInfo(int Index)
{
	auto it = this->m_Data.find(Index);

	if (it != this->m_Data.end())
	{
		return it->second;
	}

	return P_TASK_INFO();
}

void CTask::Think()
{
	for (auto it = this->m_Data.cbegin(); it != this->m_Data.cend(); ++it)
	{
		if (gpGlobals->time >= it->second.EndTime)
		{
			if (!it->second.Remove)
			{
				if (it->second.Loop)
				{
					this->m_Data[it->first].EndTime = (gpGlobals->time + it->second.Time);
				}
				else
				{
					this->m_Data[it->first].Remove = true;
				}
				
				(this->m_Data[it->first].FunctionCallback)
				{
					((void(*)(const char*))this->m_Data[it->first].FunctionCallback)(this->m_Data[it->first].FunctionParameter);
				}
			}
			else
			{
				this->m_Data.erase(it->first);
			}
		}
	}
}
