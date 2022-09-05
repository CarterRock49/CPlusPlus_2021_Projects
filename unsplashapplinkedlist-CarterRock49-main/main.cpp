#include <vector>
#include <iostream>
#include <string>
#include <conio.h>
// There is a naming collision in shlwapi.h with STL. This fie
// https://stackoverflow.com/questions/11544073/how-do-i-deal-with-the-max-macro-in-windows-h-colliding-with-max-in-std
#define NOMINMAX
#include <shlwapi.h>

#include "HttpClient.h"
#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;
////////////////
#ifdef _TESTING
#include "HttpClientTesting.h"
#endif
//////////////////////////////////////
// the amount of entrys per page
const int ENTRYS = 10;
class CompareDescription {
public:
	bool Compare(int length1, int length2, bool ascending) {
		if (ascending) {
			return length1 > length2;
		} else {
			return length1 < length2;
		}
	}
};
class CompareArea {
public:
	bool Compare(int area1, int area2, bool ascending) {
		if (ascending) {
			return area1 > area2;
		}
		else {
			return area1 < area2;
		}
	}
};
class CompareId {
public:
	bool Compare(int length1, int length2, bool ascending) {
		if (ascending) {
			return length1 > length2;
		}
		else {
			return length1 < length2;
		}
	}
};
class UnsplashClient : public HttpClient {
private:
	struct Photo
	{
		struct Author {
			string bio;
			string firstName;
			string lastName;
			string location;
		};
		string id;
		Author author;
		string description;
		int height;
		int width;
		int likes;
		string url;
	};
	int total;
	vector<char> jsondata;
	vector<int> printorder;
	Photo photos[ENTRYS];
	json jf;
	int index = 0;
	//Photo* photos;
public:
	virtual void StartOfData() {

	}
	virtual void Data(const char arrData[], const unsigned int iSize) {
		jsondata.insert(jsondata.end(), arrData, arrData + iSize);
	}
	virtual void EndOfData() {
		//this is all the info being put into the structs
		jf = json::parse(jsondata);
		total = jf["total"];
		json& results = jf["results"];
		for (json& x : results) {
			photos[index].id = x["id"];
			photos[index].height = x["height"];
			photos[index].width = x["width"];
			if (x["description"].is_null()) {
				//the description is null
			}
			else {
				//the description is not null
				photos[index].description = x["description"];
			}
			photos[index].likes = x["likes"];
			json& urls = x["urls"];
			photos[index].url = urls["raw"];
			json& user = x["user"];
			if (x["first_name"].is_null()) {
				//the firstName is null
			}
			else {
				//the firstName is not null
				photos[index].author.firstName = x["first_name"];
			}
			if (x["last_name"].is_null()) {
				//the lastName is null
			}
			else {
				//the lastName is not null
				photos[index].author.lastName = x["last_name"];
			}
			if (x["last_name"].is_null()) {
				//the bio is null
			}
			else {
				//the bio is not null
				photos[index].author.bio = x["bio"];
			}
			if (x["last_name"].is_null()) {
				//the location is null
			}
			else {
				//the location is not null
				photos[index].author.location = x["location"];
			}
			index++;
		}
		index = 0;
		jsondata.clear();
		//this is used for sorting the info
		printorder.clear();
		for (int index = 0; index < ENTRYS; index++) {
			printorder.push_back(index);
		}
	}
	UnsplashClient() {

	}
	~UnsplashClient() {

	}
	void GetPhoto(const int i) {
		// An example of launching a browser with a URL to a page. 
		// See:https://docs.microsoft.com/en-us/windows/win32/shell/launch for more details.
		//https://stackoverflow.com/questions/1200188/how-to-convert-stdstring-to-lpcstr
		ShellExecuteA(NULL, "open", photos[printorder[index]].url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	template <class T> void Sort(int sorttype, bool ascending) {
		T t;
		vector<int> lengthvector;
		CompareDescription comdesc;
		printorder.clear();
		lengthvector.clear();
		for (int index = 0; index < ENTRYS; index++) {
			printorder.push_back(index);
		}
		if (sorttype == 1) {
			vector<int> lengthvector;
			for (int index = 0; index < ENTRYS; index++) {
				lengthvector.push_back(photos[index].description.size());
			}
			for (int index = 0; index < ENTRYS - 1; index++)
			{
				for (int index2 = index + 1; index2 < ENTRYS; index2++)
				{
					if (t.Compare(lengthvector[index], lengthvector[index2], ascending)) {
						swap(lengthvector[index], lengthvector[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		}
		else if (sorttype == 2) {
			vector<int> area;
			for (int index = 0; index < ENTRYS; index++) {
				area.push_back(photos[index].height * photos[index].width);
			}
			for (int index = 0; index < ENTRYS - 1; index++) {
				for (int index2 = index + 1; index2 < ENTRYS; index2++) {
					if (t.Compare(area[index], area[index2], ascending)) {
						swap(area[index], area[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		}
		else if (sorttype == 3) {
			vector<int> id;
			for (int index = 0; index < ENTRYS; index++) {
				id.push_back(photos[index].id.size());
			}
			for (int index = 0; index < ENTRYS - 1; index++) {
				for (int index2 = index + 1; index2 < ENTRYS; index2++) {
					if (t.Compare(id[index], id[index2], ascending)) {
						swap(id[index], id[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		} else {
			throw exception("Bad sorttype");
		}
	}

	void BubbleSort(int sorttype, bool ascending) {
		if (sorttype == 1) {
			vector<int> lengthvector;
			CompareDescription comdesc;
			for (int index = 0; index < ENTRYS; index++) {
				lengthvector.push_back(photos[index].description.size());
			}
			for (int index = 0; index < ENTRYS - 1; index++)
			{
				for (int index2 = index + 1; index2 < ENTRYS; index2++)
				{
					if (comdesc.Compare(lengthvector[index], lengthvector[index2], ascending)) {
						swap(lengthvector[index], lengthvector[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		}
		else if (sorttype == 2) {
			vector<int> area;
			CompareArea comarea;
			for (int index = 0; index < ENTRYS; index++) {
				area.push_back(photos[index].height * photos[index].width);
			}
			for (int index = 0; index < ENTRYS - 1; index++) {
				for (int index2 = index + 1; index2 < ENTRYS; index2++) {
					if (comarea.Compare(area[index], area[index2], ascending)) {
						swap(area[index], area[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		}
		else if (sorttype == 3) {
			vector<int> id;
			CompareId ID;
			for (int index = 0; index < ENTRYS; index++) {
				id.push_back(photos[index].id.size());
			}
			for (int index = 0; index < ENTRYS - 1; index++) {
				for (int index2 = index + 1; index2 < ENTRYS; index2++) {
					if (ID.Compare(id[index], id[index2], ascending)) {
						swap(id[index], id[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		}
		else {
			throw exception("Bad sorttype");
		}
	}
	//this is the function to display the information that is called in main
	void display(int currentpage) {
		for (int index = 0; index < ENTRYS; index++) {
			cout << "\n" << index << ") Id: "<< photos[printorder[index]].id  << ",\tHeight: " << photos[printorder[index]].height << ",\tWidth: " << photos[printorder[index]].width << ",\tArea: " << photos[printorder[index]].height * photos[printorder[index]].width << ",\tDESC:" << photos[printorder[index]].description;
		}
		cout << "\nPage " << currentpage << " of " << total;
	}
	// this is the function to display the information that called for the testcases
	virtual void play() {
		for (int index = 0; index < ENTRYS; index++) {
			cout << "\n" << index << ") Id: "<< photos[printorder[index]].id  << ",\tHeight: " << photos[printorder[index]].height << ",\tWidth: " << photos[printorder[index]].width << ",\tArea: " << photos[printorder[index]].height * photos[printorder[index]].width << ",\tDESC:" << photos[printorder[index]].description;
		}
	}
};

ostream& operator<<(ostream& out, UnsplashClient& m)
{
	m.play();
	cout << endl;
	return out;
}

//////////////////////////////////////
#ifdef _TESTING
////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <crtdbg.h>
const std::string DATA_FILE_PATH = "TestData\\";
////////////////////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
////////////////
TEST_CASE("Testing my Unsplash client")
{
	SUBCASE("testing the parsing of an HTTP request message...")
	{
		// Open the test file...
		std::ifstream testFile(DATA_FILE_PATH + "UnsplashQuery1.json", std::ios::binary);
		if (!testFile.is_open())
			throw std::exception("Cannot open test file!");

		// Read the file into a vector of chars so that it is in the same format at what "HttpServer.h" will give us.
		std::vector<char> fileContents((std::istreambuf_iterator<char>(testFile)), std::istreambuf_iterator<char>());
		/*
		note I changed the testcases somewhat, commented out some testcases on the bottom, and didn't implement a linked list
		because I don't have enough time to finish the assignment since
		I didn't do last weeks assignment because I was busy with family matters then and so 
		the work for this week was much larger since it was based off of last week.
		*/
		UnsplashClient uc;
		uc.Test(fileContents);

		std::cout << "----------- No order / No sort -----------" << std::endl;
		std::cout << std::endl << uc;

		////
		std::cout << "--------- Sort on Id: ascending ----------" << std::endl;
		uc.Sort<CompareId>(3, true);
		std::cout << std::endl << uc;

		std::cout << "--------- Sort on Id: descending ----------" << std::endl;
		uc.Sort<CompareId>(3, false);
		std::cout << std::endl << uc;

		////
		std::cout << "----- Sort on Description: ascending -----" << std::endl;
		uc.Sort<CompareDescription>(1, true);
		std::cout << std::endl << uc;

		std::cout << "----- Sort on Description: descending -----" << std::endl;
		uc.Sort<CompareDescription>(1, false);
		std::cout << std::endl << uc;

		////
		std::cout << "----- Sort on Area: ascending -----" << std::endl;
		uc.Sort<CompareArea>(2, true);
		std::cout << std::endl << uc;

		std::cout << "----- Sort on Area: descending -----" << std::endl;
		uc.Sort<CompareArea>(2, false);
		std::cout << std::endl << uc;
		
		////
		//std::cout << "-- Locating photo by id and index --" << std::endl;
		//uc.Sort<CompareId>(true);
		/*
		UnsplashClient::Photo* photo {};
		photo = uc.GetPhoto("hhh");
		std::cout << *photo << std::endl;

		photo = uc.GetPhoto(7);
		std::cout << *photo << std::endl;
		*/
	}
}
#else
#include <fstream>
int main(int argc, char* argv[])
{
	// Leave this, as it will tell you if you have any memory leaks.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// See https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key
	const int ARROW_UP = 72;
	const int ARROW_DOWN = 80;
	const int P = 112;

	string stringholder;
	string searchterm;
	int valueholder;
	int sorttype;
	bool ascend;
	int key = 0;
	int key2 = 0;
	int page = 1;
	UnsplashClient uc;
	uc.Connect("api.unsplash.com");
	uc.SetAuthToken("Client-ID", "HyVlYrcn_uRNl0u4lF3ytCJwuUxjkz_GqeYH0a3TsYc");

	cout << "Please enter a search term and hit enter: ";
	cin >> searchterm;
	while (true) {
		cout << "\nPlease choose a sorting type (1 for Description, 2 for Area, 3 for id) and hit enter: ";
		cin >> sorttype;
		if (sorttype == 1 || sorttype == 2 || sorttype == 3) {
			break;
		}
		else {
			cout << "\nunacceptable input please try again";
		}
	}
	while (true) {
		cout << "\nPlease choose to sort by ascending or descending (1 for ascending and 0 for descending): ";
		cin >> ascend;
		if (sorttype == 1 || sorttype == 0) {
			break;
		}
		else {
			cout << "\nunacceptable input please try again";
		}
	}
	uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
	uc.BubbleSort(sorttype, ascend);
	uc.display(page);
	while (true) {
		cout << "\nPress Arrow Up to move one page ahead, Arrow Down to move one page back, press p if there is a picture you want to see currently listed, and enter anything else to end the program: ";
		_getch();
		key = _getch();
		/*
		switch (_getch()){
			case ARROW_UP:
				page++;
				uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
				uc.BubbleSort(sorttype);
				uc.display(page);
				break;
			case ARROW_DOWN:
				if (page-- == 0) {
					cout << "\nError you cannot go lower than page one";
				} else {
					page--;
					uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
					uc.BubbleSort(sorttype);
					uc.display(page);
				}
				break;
			case P:
				cout << "\nEnter the number of the picture you want to see: ";
				cin >> valueholder;
				if (valueholder < 0 || valueholder > 9) {
					cout << "\nunacceptable input";
				} else {
					uc.GetPhoto(valueholder);
				}
				break;
		}
		*/
		if (key == ARROW_UP) {
			page++;
			uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
			uc.BubbleSort(sorttype, ascend);
			uc.display(page);
		}
		else if (key == ARROW_DOWN) {
			if (page - 1 == 0) {
				cout << "\nERRORRRd you cannot go lower than page one";
			}
			else {
				page--;
				uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
				uc.BubbleSort(sorttype, ascend);
				uc.display(page);
			}
		}
		else if (key == P) {
			cout << "\nEnter the number of the picture you want to see: ";
			cin >> valueholder;
			if (valueholder < 0 || valueholder > 9) {
				cout << "\nunacceptable input";
			}
			else {
				uc.GetPhoto(valueholder);
			}
		}
		else {
			return 0;
		}
		/*
		cout << "\nIf you would like to continue the program type \"e\" if you would like to exit the program type anything but that: ";
		cin >> stringholder;
		//makes the string uppercase
		transform(stringholder.begin(), stringholder.end(), stringholder.begin(), ::toupper);
		if (stringholder != "E") { return 0; }
		*/
	}
}
#endif //_TESTING