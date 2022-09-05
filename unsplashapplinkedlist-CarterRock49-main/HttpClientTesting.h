#pragma once
#include <vector>

#ifdef _TESTING
class HttpClient
{
protected:
	// Override this to get the data from the GET method call.
	virtual void StartOfData()
	{}
	virtual void Data(const char arrData[], const unsigned int iSize)
	{}
	virtual void EndOfData()
	{}

public:

	HttpClient() {}

	void Test(std::vector<char> data)
	{
		StartOfData();
		Data(&data[0], static_cast<unsigned int>(data.size()));
		EndOfData();
	}
};
#endif
