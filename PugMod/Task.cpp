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
	this->Create(Index, Time, Loop, FunctionCallback, nullptr);
}

void CTask::Create(int Index, float Time, bool Loop, void* FunctionCallback, void* FunctionParameter)
{
	if (FunctionCallback)
	{
		if (!this->Exists(Index))
		{
			P_TASK_INFO Task = { 0 };

			Task.Index = Index;
			Task.Time = Time;
			Task.EndTime = gpGlobals->time + Time;
			Task.Loop = Loop;
			Task.FunctionCallback = FunctionCallback;
			Task.FunctionParameter = FunctionParameter;

			this->m_Data.insert(std::make_pair(Task.Index, Task));
		}
	}
}

bool CTask::Exists(int Index)
{
	auto it = this->m_Data.find(Index);

	return (it != this->m_Data.end());
}

void CTask::Remove(int Index)
{
	this->m_Data.erase(Index);
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

void CTask::Think()
{
	P_TASK_INFO Task = { 0 };

	for (std::map<int, P_TASK_INFO>::iterator it = this->m_Data.begin();it != this->m_Data.end();)
	{
		if (gpGlobals->time >= it->second.EndTime)
		{
			Task = it->second;

			if (Task.FunctionParameter != nullptr)
			{
				((void(*)(void*))Task.FunctionCallback)(Task.FunctionParameter);
			}
			else
			{
				((void(*)())Task.FunctionCallback)();
			}

			if (it->second.Loop)
			{
				it->second.EndTime += it->second.Time;

				it++;
			}
			else
			{
				it = this->m_Data.erase(it);
			}
		}
		else
		{
			it++;
		}
	}
}