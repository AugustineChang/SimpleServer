#pragma once

#include "ThreadingBase.h"
#include "Networking.h"
#include "VRGameInstance.h"

class FReceiveThread : public FRunnable
{
public:
	FReceiveThread(FSocket *client, UVRGameInstance *inst) : m_Client(client), m_inst(inst)
	{}
	~FReceiveThread()
	{
		stopping = true;
	}

	virtual bool Init() override
	{
		stopping = false;
		return true;
	}

	virtual uint32 Run() override
	{
		if (!m_Client)
		{
			return 0;
		}

		TArray<uint8> ReceiveData;
		uint8 element = 0;
		//接收数据包
		while (!stopping)   //线程计数器控制
		{
			ReceiveData.Init(element, 1024u);
			int32 read = 0;
			if (m_Client->Recv(ReceiveData.GetData(), ReceiveData.Num(), read)) {
				const FString ReceivedUE4String = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData())));
				FString log = "Server: " + ReceivedUE4String;

				UE_LOG(LogTemp, Warning, TEXT("*** %s"), *log);

				m_inst->OnTimeLimitCallback(true, ReceivedUE4String);
				
				this->Stop();
			}			
			else {
				this->Stop();				
				UE_LOG(LogTemp, Warning, TEXT("*** disconnected"));				
			}

			FPlatformProcess::Sleep(0.01f);
		}

		m_Client->Close();
		return 1;
	}

	virtual void Stop() override
	{
		stopping = true;    //计数器-1
	}

private:
	FSocket *m_Client;  //客户端套接字
	UVRGameInstance *m_inst;
	bool stopping;      //循环控制
	FThreadSafeCounter m_StopTaskCounter;   //线程引用计数器
};