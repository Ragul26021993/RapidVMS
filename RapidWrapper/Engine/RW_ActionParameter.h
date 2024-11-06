#pragma once
namespace RWrapper 
{
public ref class RW_ActionParameter
{
private:
	static RW_ActionParameter^ AcnParamInstance;

public:
	delegate void ActionParameterEventHandler(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE actType);
	event ActionParameterEventHandler^ ActionParameterUpdate;
	int Action_EntityID;
	static System::Collections::ArrayList^ ActionParameter_ArrayList;

public:

	RW_ActionParameter();
	~RW_ActionParameter();
	static RW_ActionParameter^ MYINSTANCE();

#pragma region Shape Events, Parameter Updates

	 void RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE actionType);
	 bool SetActionParamForFrontend(RAction* currentAction);
	 void DeleteAction(int id);
	 void SetCriticalAction(int id, bool state);
	 void RerunAction(int id);
#pragma endregion

};
}

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!