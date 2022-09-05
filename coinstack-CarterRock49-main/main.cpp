#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
////////////////////////////////////////////////////////////////////////////////////////////////
// Work here.
////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <crtdbg.h>
const std::string DATA_FILE_PATH = "TestData\\";
////////////////////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <string>

using namespace std;

struct Coin {
	double marketCap;
	string name;
	Coin* next;
	int rank;
	string symbol;
};

Coin* top;

template <typename T> class TListStack{
	//this function frees all nodes in the stack
	void Delete(T* pt) {
		for (int index = 0; index > counter; index++) {
			Coin* temp = new Coin();
			temp = top;
			free(top);
			top = temp->next;
			
		}
	}
	T* _container;
	
public:
	int counter = 0;
	//this function removes the top of the stack and returns it
	T Pop() {
		//throws an out of range exception is there is nothing in the stack.
		if (top == NULL)
			throw out_of_range("Error:Out of range!");
		Coin* temp;
		temp = top;
		top = top->next;
		Coin t = *temp;
		free(temp);
		return t;
	}
	//this function returns the top of the stack
	T Top() {
		if (top == NULL)
			throw out_of_range("Error:Out of range!");
		Coin t = *top;
		return t;
	}
	//this function returns the bottom of the stack
	T Bottom() {
		if (top == NULL)
			throw out_of_range("Error:Out of range!");
		Coin* temp = top;
		while (temp->rank != 1) {
			temp = temp->next;
		}
		Coin t = *temp;
		return t;
	}
	//this function add another struct on the top of the stack
	void Push(Coin t) {
		Coin* temp = new Coin();
		top = &t;
		temp->next = top;
		//top = temp;

		delete temp;
	}
	~TListStack() {
		
	}

};
class Cryptos : public TListStack<Coin> {
	json jf;
public:
	//constuctor that parses a file
	Cryptos(ifstream& fp) {
		jf = json::parse(fp);
		for (auto& d : jf) {
			Coin* temp = new Coin();
			temp->rank = d["RANK"];
			temp->name = d["NAME"];
			temp->symbol = d["SYMBOL"];
			temp->marketCap = d["MARKET_CAP"];
			temp->next = top;
			counter++;//used for the delete function
			top = temp; //saves the current stack for the next pass through the loop
		}
	}
	~Cryptos() {
		
	}
};
////////////////
TEST_CASE("Testing the read Coin stack")
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SUBCASE("testing the parsing of an empty file...")
	{
		// Open the test file...
		std::ifstream testFile(DATA_FILE_PATH + "empty.json", std::ifstream::binary);
		if (!testFile.is_open())
			throw std::exception("Cannot open test file!");

		Cryptos crypto(testFile);

		try
		{
			crypto.Pop().rank;
		}
		catch (const std::out_of_range& e)
		{
			CHECK(std::string(e.what()) == "Error:Out of range!");
		}


		Coin coin;
		coin.rank = 0;
		coin.name = "No Name";
		coin.symbol = "No Symbol";
		coin.marketCap = -1;
		crypto.Push(coin);

		CHECK(crypto.Top().rank == 0);
	}
	SUBCASE("testing the parsing of a file with Coins (in JSON)...")
	{
		// Open the test file...
		std::ifstream testFile(DATA_FILE_PATH + "JsonObject14a.json", std::ifstream::binary);
		if (!testFile.is_open())
			throw std::exception("Cannot open test file!");

		Cryptos crypto(testFile);

		CHECK(crypto.Bottom().rank == 1);
		CHECK(crypto.Top().rank == 50);

		CHECK(crypto.Pop().rank == 50);
		CHECK(crypto.Pop().rank == 49);
		CHECK(crypto.Pop().rank == 48);

		CHECK(crypto.Pop().name == "Gala");
		CHECK(crypto.Pop().name == "Aave");
		CHECK(crypto.Pop().name == "Monero");

		CHECK(crypto.Pop().symbol == "MIOTA");
		CHECK(crypto.Pop().symbol == "HNT");
		CHECK(crypto.Pop().symbol == "XTZ");

		CHECK(crypto.Pop().marketCap == 4919251508);
		CHECK(crypto.Pop().marketCap == 4977622135);
		CHECK(crypto.Pop().marketCap == 4994016853);
	}
}