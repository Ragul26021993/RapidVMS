//Class to handle the serial communication with the laser Machine..
//Specific for delphi...

#pragma once
#include "RW_MainInterface.h"
namespace RWrapper 
{

public ref class RW_LaserComm
 {
 private: 
	static RW_LaserComm^ PortComInst;
	System::Windows::Forms::Timer^  MyTimer;
	System::IO::Ports::SerialPort^ CommPort;
	System::String^ Readdata;
	System::String^ SettingsFilePath;

	System::String^ GetCommPortName();
	void ConnectToPort(System::String^ PortName);
	void MyTimer_Tick(System::Object^ sender, System::EventArgs^ e);
	void ReadDataBuffer();
	void WritePortData(System::String^ WrString);

 public:
	RW_LaserComm(System::String^ SettingsFile);
	~RW_LaserComm();
	static RW_LaserComm^ MYINSTANCE();
	void InitialisePortComm();
	void SendDataTOPort(System::String^ WrString);
	void ClosePortCommunication();
 };
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!