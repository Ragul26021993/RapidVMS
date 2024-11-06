#include "Stdafx.h"
#include "RW_LaserComm.h"

//Default constructor.. Initialise timer for communication.
RWrapper::RW_LaserComm::RW_LaserComm(System::String^ SettingsFile)
{
	try
	{
		this->PortComInst = this;
		this->SettingsFilePath = SettingsFile;
		this->MyTimer = gcnew System::Windows::Forms::Timer();
		this->MyTimer->Interval = 20;
		this->MyTimer->Tick += gcnew System::EventHandler(this, &RWrapper::RW_LaserComm::MyTimer_Tick);
		this->MyTimer->Enabled = false;
		this->Readdata = "";
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0001", ex); }
}
	
//Destructor.. Release memory...
RWrapper::RW_LaserComm::~RW_LaserComm()
{
	try
	{
		this->MyTimer->Enabled = false;
		if(CommPort->IsOpen)
			CommPort->Close();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0002", ex); }
}

//Returns this class Instance..
RWrapper::RW_LaserComm^ RWrapper::RW_LaserComm::MYINSTANCE()
{
	return PortComInst;
}

//Initialise Prot..
void RWrapper::RW_LaserComm::InitialisePortComm()
{
	try
	{
		this->CommPort = gcnew System::IO::Ports::SerialPort;
		System::IO::StreamReader^ Fileread = gcnew System::IO::StreamReader(SettingsFilePath);
		Fileread->ReadLine();
		System::String^ PortName = Fileread->ReadLine();
		Fileread->Close();
		//System::String^ PName = GetCommPortName();
		if(PortName == "NULL")
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_LaserCommInitialisePortComm01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		ConnectToPort(PortName);
		//this->MyTimer->Enabled = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0003", ex); }
}

//Get the Laser Machine Port Name..
System::String^ RWrapper::RW_LaserComm::GetCommPortName()
{
	try
	{
		Microsoft::VisualBasic::Devices::Computer^ MyComputer = gcnew Microsoft::VisualBasic::Devices::Computer();
		Microsoft::Win32::RegistryKey^ i = MyComputer->Registry->LocalMachine->OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\");
		for each(System::String^ s in i->GetSubKeyNames())
		{
			int r = s->IndexOf("&Mi_");
			int tem = s->IndexOf("_ea");
			if(r != -1 && tem != -1)
			{
                i = MyComputer->Registry->LocalMachine->OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\" + s + "\\");
                i = i->OpenSubKey((i->GetSubKeyNames())[0] + "\\Device Parameters");
                return (i->GetValue("PortName")->ToString());
			}
		}
		return "";
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0004", ex); }
}

//Connect to Laser machine..
void RWrapper::RW_LaserComm::ConnectToPort(System::String^ PortName)
{
	try
	{
		this->CommPort->PortName = PortName;
		this->CommPort->Open();
		this->CommPort->BaudRate = 9600;
		this->CommPort->DataBits = 8;
		this->CommPort->Parity = System::IO::Ports::Parity::None;
		this->CommPort->StopBits = System::IO::Ports::StopBits::One;
		this->CommPort->Handshake = System::IO::Ports::Handshake::None;
		//this->CommPort->Close();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0005", ex); }
}

//Timer rick handling..
void RWrapper::RW_LaserComm::MyTimer_Tick(System::Object^ sender, System::EventArgs^ e)
{
	try{ ReadDataBuffer(); }
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0006", ex); }
}

//Read the data from the port..
void RWrapper::RW_LaserComm::ReadDataBuffer()
{
	try
	{
		if(!CommPort->IsOpen)
		{
			this->MyTimer->Enabled = false;
			return;
		}
		System::String^ temp = CommPort->ReadExisting();
		Readdata = Readdata + temp;
		if(Readdata->Contains("36"))
		{
			this->Readdata = "";
			this->MyTimer->Enabled = false;
			WritePortData("36");
		}
		if(Readdata->Contains("3231"))
		{
			this->Readdata = "";
			this->MyTimer->Enabled = false;
			WritePortData("3231");
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0007", ex); }
}

//Write data to the port..
void RWrapper::RW_LaserComm::WritePortData(System::String^ WrString)
{
	try
	{
		this->MyTimer->Enabled = false;
		for each(char cch in WrString)
		{
			 System::String^ Sdata = gcnew System::String(&cch);
			 CommPort->Write(Sdata);
             Sleep(10);
		}
		this->MyTimer->Enabled = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0008", ex); }
}

//Send data to port.. Exposed function
void RWrapper::RW_LaserComm::SendDataTOPort(System::String^ WrString)
{
	WritePortData(WrString);
}

//Close the port Communiction..
void RWrapper::RW_LaserComm::ClosePortCommunication()
{
	try
	{
		this->MyTimer->Enabled = false;
		if(CommPort->IsOpen)
			CommPort->Close();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("PORTCOMM0009", ex); }
}