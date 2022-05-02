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
	CTask::Create(Index, Time, Loop, FunctionCallback, NULL);
}

void CTask::Create(int Index, float Time, bool Loop, void* FunctionCallback, void* FunctionParameter)
{
	if (FunctionCallback)
	{
		if (!this->Exists(Index))
		{
			P_TASK_INFO Task = { 0 };

			Task.Index				= Index;
			Task.Time				= Time;
			Task.EndTime			= gpGlobals->time + Time;
			Task.Loop				= Loop;
			Task.FunctionCallback	= FunctionCallback;
			Task.FunctionParameter	= FunctionParameter;

			this->m_Data.insert(std::make_pair(Task.Index, Task));
		}
	}
}

bool CTask::Exists(int Index)
{
	std::map<int, P_TASK_INFO>::iterator it = this->m_Data.find(Index);

	return (it != this->m_Data.end());
}

void CTask::Remove(int Index)
{
	this->m_Data.erase(Index);
}

float CTask::Timeleft(int Index)
{
	std::map<int, P_TASK_INFO>::iterator it = this->m_Data.find(Index);

	if (it != this->m_Data.end())
	{
		return (it->second.EndTime - gpGlobals->time);
	}

	return 0.0f;
}

void CTask::Think()
{
	for (std::map<int, P_TASK_INFO>::iterator it = this->m_Data.begin(); it != this->m_Data.end(); it++)
	{
		if (gpGlobals->time >= it->second.EndTime)
		{
			P_TASK_INFO Task = it->second;
			
			if (it->second.Loop)
			{
				it->second.EndTime += it->second.Time;
			}
			else
			{
				this->Remove(it->first);
			}

			if (Task.FunctionParameter)
			{
				((void(*)(void*))Task.FunctionCallback)(Task.FunctionParameter);
			}
			else
			{
				((void(*)())Task.FunctionCallback)();
			}
		}
	}
}
