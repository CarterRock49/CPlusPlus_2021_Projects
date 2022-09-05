#include <shlwapi.h>	// To launch the web browser
#include "HttpClient.h"
#include <vector>
#include <iostream>
#include <string>
#include <conio.h>
#pragma warning( push )
#pragma warning(disable:26819)
#include "json.hpp"
#pragma warning( pop )
using json = nlohmann::json;
using namespace std;
////////////////////////////////////////////////
const int ENTRYS = 10;
class Compare {
public:
	bool CompareArea(int area1 , int area2) {
		return area1 > area2;
	}

	bool CompareDescription(int length1, int length2) {
		return length1 > length2;
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
protected:
	virtual void StartOfData() {
	
	}
	virtual void Data(const char arrData[], const unsigned int iSize) {
		jsondata.insert(jsondata.end(), arrData, arrData + iSize);
	}
	virtual void EndOfData() {
		jf = json::parse(jsondata);
		total = jf["total"];
		json& results = jf["results"];
		for (json& x : results) {
		photos[index].height = x["height"];
		photos[index].width = x["width"];
		if (x["description"].is_null()) {
			//the description is null
		} else {
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
		printorder.clear();
		for (int index = 0; index < ENTRYS; index++) {
			printorder.push_back(index);
		}
	}
public:
	UnsplashClient() {
		
	}
	~UnsplashClient() {

	}
	void GetPhoto(const int i) {
		// An example of launching a browser with a URL to a page. 
		// See:https://docs.microsoft.com/en-us/windows/win32/shell/launch for more details.
		//https://stackoverflow.com/questions/1200188/how-to-convert-stdstring-to-lpcstr
		ShellExecuteA(NULL, "open", photos[index].url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	void BubbleSort(int sorttype) {
		Compare com;
		if (sorttype == 1) {
			vector<int> lengthvector;
			for (int index = 0; index < ENTRYS; index++) {
				lengthvector.push_back(photos[index].description.size());
			}
			for (int index = 0; index < ENTRYS - 1; index++) 
			{
				for (int index2 = index + 1; index2 < ENTRYS; index2++) 
				{
					if (com.CompareDescription(lengthvector[index], lengthvector[index2])) {
						swap(lengthvector[index], lengthvector[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		} else if (sorttype == 2) {
			vector<int> area;
			for (int index = 0; index < ENTRYS; index++) {
				area.push_back(photos[index].height * photos[index].width);
			}
			for (int index = 0; index < ENTRYS - 1; index++) {
				for (int index2 = index + 1; index2 < ENTRYS; index2++) {
					if (com.CompareArea(area[index], area[index2])) {
						swap(area[index], area[index2]);
						swap(printorder[index], printorder[index2]);
					}
				}
			}
		} else {

		}
	}
	void display(int currentpage) {
		for (int index = 0; index < ENTRYS; index++) {
			cout << "\n" << index << ") Height: " << photos[printorder[index]].height << ",\tWidth: " << photos[printorder[index]].width << ",\tArea: " << photos[printorder[index]].height * photos[printorder[index]].width << ",\tDESC:" << photos[printorder[index]].description;
		}
		cout << "\nPage " << currentpage << " of " << total;
	}
};
////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	// Leave this, as it will tell you if you have any memory leaks.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// See https://stackoverflow.com/questions/24708700/c-detect-when-user-presses-arrow-key
	const int ARROW_UP	 = 72;
	const int ARROW_DOWN = 80;
	const int P = 112;

	// An example use invoking an GET on the Unsplash API.
	// Notice the query term and the page number.
	string stringholder;
	string searchterm;
	int valueholder;
	int sorttype;
	int key = 0;
	int key2 = 0;
	int page = 1;
	UnsplashClient uc;
	uc.Connect("api.unsplash.com");
	uc.SetAuthToken("Client-ID", "HyVlYrcn_uRNl0u4lF3ytCJwuUxjkz_GqeYH0a3TsYc");

	cout << "Please enter a search term and hit enter: ";
	cin >> searchterm;
		while (true) {
			cout << "\nPlease choose a sorting type (1 for Description or 2 for Area) and hit enter: ";
			cin >> sorttype;
			if (sorttype == 1 || sorttype == 2) {
				break;
			} else {
				cout << "\nunacceptable input please try again";
			}
		}
		uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
		uc.BubbleSort(sorttype);
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
				uc.BubbleSort(sorttype);
				uc.display(page);
			} else if (key == ARROW_DOWN) {
				if (page - 1 == 0) {
					cout << "\nERRORRRd you cannot go lower than page one";
				} else {
					page--;
					uc.Get("/search/photos", { {"query",searchterm}, {"page",to_string(page)} });
					uc.BubbleSort(sorttype);
					uc.display(page);
				}
			} else if (key == P) {
				cout << "\nEnter the number of the picture you want to see: ";
				cin >> valueholder;
				if (valueholder < 0 || valueholder > 9) {
					cout << "\nunacceptable input";
				}
				else {
					uc.GetPhoto(valueholder);
				}
			} else {
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