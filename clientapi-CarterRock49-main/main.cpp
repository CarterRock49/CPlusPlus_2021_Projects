#include "HttpClient.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
// Quell the nlohmann::json warnings.
#pragma warning( push )
#pragma warning(disable:28020)
#include "json.hpp"
#pragma warning( pop )

using json = nlohmann::json;
using namespace std;
//////////////////////////////////////
// 
//////////////////////////////////////
template<class T, class S>
class GenericClient : public HttpClient{
	vector<char> data;
	T it;
	json jf;

protected:

	virtual void StartOfData() { cout << "Start of data...." << endl; }

	virtual void Data(const char arrData[], const unsigned int iSize) {
		jf += arrData;
		data.insert(data.end(), arrData, arrData + iSize);
	}

	virtual void EndofData() {

		for(auto c : data){
			cout << c;
		}

		cout << endl << "End of data...." << endl << endl;
	}

public:

	GenericClient(){}

	~GenericClient(){}

	S& operator[](int i){}

	GenericClient<T,S> operator++(int){}
};
class Foodreport{
public:
	struct FoodFact
	{
		string product;
		string  manufacturer;
		int rating;
	};

private:
	vector<FoodFact> Facts;

public:
	/*
	Foodreport(){

	}

	FoodFact& GetAt(const int i){

	}

	FoodFact operator++(int i){

	}

	void Parse(const json& jf){
		
	}
	*/
};
class BoredIdeas{
public:
	struct Idea
	{
		string activity;
		string type;
		int price;
	};
	
private:
	vector<Idea> Ideas;

public:
	/*
	BoredIdeas(){}

	Idea& GetAt(const int i){}

	Idea operator++(int i){}

	void Parse(const json& jf){}
	*/

};
template<class T, class S>
ostream& operator<<(ostream& COUT, const GenericClient<T,S>& cf)
{
	return COUT;
}
int main(int argc, char* argv[])
{
	{
		GenericClient<Foodreport, Foodreport::FoodFact> cf;
		cf.Connect("thereportoftheweek-api.herokuapp.com");
		cf.Get("/reports");
		
		cout << cf << endl;
		/*
		cout << "Orginal first Fact" << endl << cf[0];

		cf++;
		
		cout << "New first Fact" << endl << cf[0];
		*/
	}

	{
		GenericClient<BoredIdeas, BoredIdeas::Idea> gh;
		gh.Connect("boredapi.com/api/");
		gh.Get("/api/activity");
		
		cout << gh << endl;
		/*
		cout << "Orginal first Repo" << endl << gh[0];

		gh++;

		cout << "New first Repo" << endl << gh[0];
		*/
	}

	return 0;
}
