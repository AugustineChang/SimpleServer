#pragma once

#include "ThreadingBase.h"
#include "Networking.h"

class FConnectSocketThread : public FRunnable {

public:
	FConnectSocketThread(FSocket *host, TSharedPtr<FInternetAddr> addr) : m_Host(host), m_addr(addr)
	{}

	~FConnectSocketThread() {
	}

	virtual uint32 Run() override {
		if (!m_Host) {
			return 0;
		}

		if (m_Host->GetConnectionState() == ESocketConnectionState::SCS_Connected) {
			return 1;
		}

		if (m_Host->Connect(*m_addr)) {
			UE_LOG(LogTemp, Warning, TEXT("Connect Succeed!"));
			return 1;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Connect Failed!"));
			return 0;
		}
	}

private:
	FSocket *m_Host;
	TSharedPtr<FInternetAddr> m_addr;	
};