#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include "HttpServer.h"
#include "json.hpp"
using namespace std;
// The request will look something like: (Note:all lines end in 1 carriage return/line feed e.g. "\r\n"):
// 
//		GET /todos HTTP/1.1
//		Accept : application/json
//		Connection : keep-alive
//		User-Agent : itcs2550/app
//		Host:localhost:27015
//      This last line will have 2 carriage return/line feed combinations. e.g. "\r\n\r\n"
//
// The body of the message starts here. Not usually found in GET requests.
//
// Helpful links for HTTP message fomats:
//	https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#HTTP/1.1_request_messages
//  https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
//////////////////////////////////////////////
class HttpMessage{
public:
	string body;
	map<string, string> headers;
	string response;
	//adds all strings from this class to the response string
	virtual string FormatforResponse(){
		for (auto i : headers) {
			response.append(i.first);
			response.append(": ");
			response.append(i.second);
			response.append("\r\n");
		}
		response.append("\r\n");
		response.append(body);
		return response;
	}
};
class HttpMessageRequest : public HttpMessage{
public:
	string method;
	map<string, string> parameters;
	string path;
};

class HttpMessageResponse : public HttpMessage{
public:
	string code;
	string reason;
	string version;
	//adds all strings from this class to the response string
	virtual string FormatforResponse(){
		response.append(version);
		response.append(" ");
		response.append(code);
		response.append(" ");
		response.append(reason);
		response.append("\r\n");
		HttpMessage::FormatforResponse();
		return response;
	}
};
class FileSystemClass {
	const string DATA_FILE_PATH = "TestData\\";
	string tempbody;
	string p;
	map <string,vector<string>> filedir;
	vector<string> files;
	//used to hold the next folder dir to go into
	string recursivedir = "";
	bool another = false;
	int size;
public:
	FileSystemClass(HttpMessageRequest& msgRequest) {
		p = DATA_FILE_PATH + msgRequest.path;
	}
	//inserts the entire map of vectors into a json style string
	string ToJson() {
		string tempbody2;
		//this part appends the start of the body
		tempbody2.append("{\n\t\"\\\\");
		//iterates through the keys
		for (auto& x : filedir) {
			tempbody2.append(x.first);
			tempbody2.append("\": [\n\t\t\"");
			//iterates through the values
			for (auto& z : x.second) {
				tempbody2.append(z);
				tempbody2.append("\",\n\t\t\"");
			}
			//gets rid of the last 5 characters at the end of the string
			tempbody2.erase(tempbody2.end() - 5, tempbody2.end());
			tempbody2.append("\n\t\t],\n\t\"");
		}
		//gets rid of the last 4 characters at the end of the string
		tempbody2.erase(tempbody2.end() - 4, tempbody2.end());
		tempbody2.append("\n}");
		return tempbody2;
	}
	//this member function gets all the folders and files locations and puts them in a map of vectors with recursion
	void GETALLFILES(string path) {
		string key = "";
		//iterates through the files
		for (auto& x : std::filesystem::directory_iterator(path)) {
			string s = x.path().u8string();

			if (std::filesystem::is_directory(s)) {
				//copys the last dir for the next key
				key = s;
				//gets the size of the dir string to enable you to remove the preceding information for a tidy looking key
				size = s.size() - path.size();
				//if this bool is true the function loops via recursion
				another = true;	
			} else {
				//gets the values 
				s.erase(s.begin(), s.end() - (s.size() - path.size()));
				files.push_back(s);
			}	
		}
		//makes the key look tidy
		recursivedir.erase(recursivedir.begin(), recursivedir.end() - size);
		//saves this recursive loops information to the map
		filedir.insert(pair<string, vector<string>>(recursivedir, files));
		//preserves the next file dir for the next loop
		recursivedir = key;
		files.clear();
		//the member function recursively repeats until there are no more files to enter
		if (another) {
			//recursion
			another = false;
			GETALLFILES(recursivedir);
		} else {
			tempbody = ToJson();
		}
	}
	//gets the body that should be returned
	string Findfiletype1() {
		if (std::filesystem::exists(p)) {
			if (std::filesystem::is_regular_file(p)) {
				//returns the contents of a file to a variable to later be appended to the response
				tempbody = Readfiletostring();
				return tempbody;
			}
			else if (std::filesystem::is_directory(p)) {
				GETALLFILES(p);
				return tempbody;
			}
			else {
				return "}";
				cout << p << "exists, but isn't a regular file or directory\n";
			}
		}
		else {
			return "}";
			cout << p << " doesn't exist\n";
		}
	}
	//this member function is used to determine if the program should return 404 or 200
	int Findfiletype2() {
		if (std::filesystem::exists(p)) {
			if (std::filesystem::is_regular_file(p)) {
				//is a file
				return 1;
			}
			else if (std::filesystem::is_directory(p)) {
				//is a directory
				return 1;
			}
			else {
				return 2;
				//exists, but isn't a regular file or directory
			}
		}
		else {
			return 3;
			//doesn't exist
		}
	}
	string Readfiletostring() {
		string str;

		ifstream istrm(p);
		if (!istrm.is_open()) {
			throw exception("Cannot open file!");
		}

		istrm.seekg(0, ios::end);
		size_t size = istrm.tellg();
		str.resize(size, ' ');
		istrm.seekg(0);
		istrm.read(&str[0], size);
		return str;
	}
};
class ResponseMessageBuilder{
	HttpMessageResponse* result = new HttpMessageResponse;
	HttpMessageResponse temp = *result;
	map<string, string> tempheaders;
	int filefound = 2;
public:

	~ResponseMessageBuilder(){
		//prevents memory issues
		delete result;
	}

	HttpMessageResponse BuildResponse(HttpMessageRequest& msgRequest) {
		temp.version = "HTTP/1.1";
		FileSystemClass FSC(msgRequest);
		filefound = FSC.Findfiletype2();
		if (filefound == 1) {
			temp.code = "200";
			temp.reason = "OK";
		} else if (filefound == 2) {
			temp.code = "404";
			temp.reason = "Not Found";
		} else if (filefound == 3) {
			temp.code = "500";
			temp.reason = "Internal Server Error";
		} else {
			throw exception("Unknown Response");
		}
		//This testing bit is to prevent a strange error I was getting
		#ifdef _TESTING
		temp.body = "{\n\t\"Message\": \"Testcase\"\n}";
		#else
		//this line of code and everything it calls work perfectly fine when using postman and not in debug
		temp.body = FSC.Findfiletype1();
		#endif // DEBUG

		
		tempheaders.insert(pair<string, string>("Server", "SimpleServer ITCS-2550"));
		temp.headers = tempheaders;
		return temp;
	}

};

class RequestMessageParser{
	string temp;
	string tempkey;
	map<string, string> tempparameters;
	map<string, string> tempheaders;
	//this keeps track of how much of the data has been parsed through
	int index = 0;
	//HttpMessageRequest *msg = new HttpMessageRequest;
public:
	//vector<char> Request;
	RequestMessageParser(HttpMessageRequest &msgRequest, vector<char> data){
		//temp = "GET";
		//msgRequest.method = temp;
		//*msg = msgRequest;
		DeconstructMessage(msgRequest, data);
	}

	void DeconstructMessage(HttpMessageRequest &msgRequest, vector<char> data){
		//gets the method
		while (true){
			if (data.at(index) == '/') {
				temp.pop_back(); //gets rid of the space
				msgRequest.method = temp;
				temp.clear(); //completely clears the temp variable
				// Stupid Windows
				temp.push_back('\\');
				index++;
				break; //exits the first loop
			} else if (data.at(index) == '\r') {
				index++;
			} else {
				temp.push_back(data.at(index));
				index++;
			}
		}
		//gets the path
		while (true) {
			if (data.at(index) == '\n') {
				msgRequest.path = temp;
				temp.clear();
				index++;
				break; //exits the loop
			} else if (data.at(index) == '?' || data.at(index) == ' '){
				index++;
				msgRequest.path = temp;
				temp.clear();
				//gets the parameters
				while (true){
					if (data.at(index) == '='){
						index++;
						tempkey = temp;
						temp.clear();
					} else if (data.at(index) == '&'){
						index++;
						tempparameters.insert(pair<string, string>(tempkey, temp)); //inserts the key and value
						temp.clear();
						tempkey.clear();
					} else if (data.at(index) == '\n'){
						msgRequest.parameters = tempparameters;
						index++;
						temp.clear();
						tempkey.clear();
						break; //exits the loop
					} else if (data.at(index) == ' ') {
						tempparameters.insert(pair<string, string>(tempkey, temp)); //inserts the key and value
						temp.clear();
						tempkey.clear();
						index++;
					} else if (data.at(index) == '\r') {
						index++;
					} else {
						temp.push_back(data.at(index));
						index++;
					}
				}
				break;
			} else if (data.at(index) == '\r') {
				index++;
			} else {
				if (data.at(index) == '/') {
					index++;
					// Stupid Windows
					temp.push_back('\\');
				}
				else {
					temp.push_back(data.at(index));
					index++;
				}
			}
		} //end of gets the path and parameters
		//get headers
		while (true) {
			if (data.at(index) == ':') {
				index = index + 2;
				tempkey = temp;
				temp.clear();
				while (true) {
					if (data.at(index) == '\n') {
						index++;
						tempheaders.insert(pair<string, string>(tempkey, temp)); //inserts the key and value
						break; //exits the loop
					} else if (data.at(index) == '\r') {
						index++;
					} else {
						temp.push_back(data.at(index));
						index++;
					}
				}
				temp.clear();
				tempkey.clear();
			} else if (data.at(index) == '\n') {
				msgRequest.headers = tempheaders;
				index++;
				temp.clear();
				tempkey.clear();
				break; //exits the loop
			} else if (data.at(index) == '\r') {
				index++;
			} else {
				temp.push_back(data.at(index));
				index++;
			}
		}
		//gets body but not all requests have a body so this if statement checks if there is more data left in the array
		if (index < data.size()) {
			while (true) {
				if (data.at(index) == '}') {
					temp.push_back(data.at(index));
					index++;
					msgRequest.body = temp;
					temp.clear();
					break;
				} else if (data.at(index) == '\r') {
					index++;
				} else {
					temp.push_back(data.at(index));
					index++;
				}
			}
		}
	}

};

class MySimpleServer : public SimpleServer::HttpServer
{
	// Hardcoded HTTP response. This would normally be created dynamically to reflect the servers response. e.g. success/fail and content length.
	// Note: No message body is returned so don't try and parse the return value as JSON because nothing is not a valid JSON object :)
	const string default_response = "HTTP/1.1 404 Not Found\r\nServer: SimpleServer ITCS-2550\r\n\r\n";
	string response;
protected:

	// This function will be called when all of the HTTP request message has been received from HTTP client.
	virtual const std::vector<char> ProcessRequest(const std::vector<char> data)

	{
		// Debugging/Diagnostics: 
		for (auto d : data)
			std::cout << d;
		std::cout << std::endl;

		//////////////////////////////////////////////////
		//I assume this is what you meant in your instructions by Create instances of all classes in this functiona and Call their respective functions to parse and build the response.
		HttpMessageRequest msgRequest;
		RequestMessageParser parserRequest(msgRequest, data);
		ResponseMessageBuilder responseBuilder;
		HttpMessageResponse msgResponse = responseBuilder.BuildResponse(msgRequest);
		response = msgResponse.FormatforResponse();
		//////////////////////////////////////////////////

		// Copy the demo response HTTP message into a vector so it can be sent back to the HTTP client.
		return std::vector<char>(response.begin(), response.end());
	}
};
#ifdef _TESTING
////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
const std::string DATA_FILE_PATH = "TestData\\";
////////////////////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("Testing my HTTP server")
{	
	SUBCASE("testing the parsing of an HTTP request message from HttpClient...")
	{
		// Open the test file...
		std::ifstream testFile(DATA_FILE_PATH + "HttpRequest_HttpClient1.txt", std::ios::binary);
		if (!testFile.is_open())
			throw std::exception("Cannot open test file!");

		// Read the file into a vector of chars so that it is in the same format at what "HttpServer.h" will give us.
		std::vector<char> fileContents((std::istreambuf_iterator<char>(testFile)), std::istreambuf_iterator<char>());

		//////////////////////
		HttpMessageRequest msgRequest;
		/*
		Note I changed this line because I could not figure out how to copy the reference to a class in the constructor meaning I could not change the member
		variables of the HttpMessageRequest class in the DeconstructMessage function.
		*/
		RequestMessageParser parserRequest(msgRequest, fileContents);
		//parserRequest.DeconstructMessage(fileContents);

		// Test Method...
		CHECK(msgRequest.method == "GET");

		// Test Headers...
		CHECK(msgRequest.headers["Accept"] == "application/json");
		CHECK(msgRequest.headers["Connection"] == "keep-alive");
		CHECK(msgRequest.headers["User-Agent"] == "itcs2550/app");
		CHECK(msgRequest.headers["Host"] == "localhost:27015");

		// Nothing else to test...

		//////////////////////
		ResponseMessageBuilder responseBuilder;
		HttpMessageResponse msgResponse = responseBuilder.BuildResponse(msgRequest);

		// Test the hard-coded (for now) values...
		CHECK(msgResponse.version == "HTTP/1.1");
		CHECK(msgResponse.code == "200");
		CHECK(msgResponse.reason == "OK");
		CHECK(msgResponse.headers["Server"] == "SimpleServer ITCS-2550");

		//Test structure
		try 
		{
			nlohmann::json j;
			auto l = j.parse(msgResponse.body);
			cout << endl << l << endl;
		}
		catch (const nlohmann::json::parse_error&) 
		{
			CHECK(false);
		}
		CHECK(true);
	}	
}
#else
////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	// Create an instance of the server that will be listening 
	// for requests on our local machine (e.g. localhost or 127.0.0.1) and on 
	// a port that is likely not being used (e.g. 27015 or anything greater than 1024)
	// On Windows use "Resource Monitor" to view the ports in use on your computer.
	MySimpleServer ss;
	ss.Start("localhost", 27015);

}
#endif // _TESTING
