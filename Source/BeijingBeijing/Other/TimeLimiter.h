#pragma once

class UTimeLimiter
{
private:
	UTimeLimiter();

public:
	
	static UTimeLimiter* getInstance();

	void checkNetworkAndFile( bool network , const FString & serverData );
	bool createDataDirectory();
	bool loadDataFromFile();
	void saveDataToFile();
	
	bool isTimeValid();
	bool getIsForceNetwork();
	const FString& getAuthorityName();
	FString getEncryptString( const FString &rawStr );
	FString getDecryptString( const FString &encryptStr );
	FString getDecryptString( TArray<uint8> & encryData , int32 dataLen );

private:

	void analysisRawData( const FString &rawData );
	void analysisDatetime( const FString &dateStr , FDateTime &datetime );

	void encryptData( uint8* data , int32 byteLen );
	void decryptData( uint8* data , int32 byteLen );
	int32 xxtea( int32* v , int32 n , int32 *k );

	FString filePath;
	FString fileName;
	FString fullPath;
	
	bool hasNetwork;
	bool hasEncryptFile;
	bool isTimeValid_Server;

	FDateTime lastDate;
	FDateTime deadline;
	FString authorityName;
	bool isForceNetwork;
	static UTimeLimiter *instance;
	
	int32 *password;
};