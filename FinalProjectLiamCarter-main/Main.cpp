// Final Project...
/*
Currently will store information on directory objects
can show the directory objects in the ui and can process them withe their stored information
in ui will have to treat folder and files differently for sorting, my idea was to print folders first then files
how to go to next directory will be just pass in  the path of the folder , and new structs will be created with the new current directory, will have to save \. directory path though for going up
*/
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <stdio.h>//for rename, copy and delete
#include <iomanip>
using json = nlohmann::json;
const std::string DATA_FILE_PATH = "C:\\"; //the program has to start at the base of your harddrive since stacks are FIFO structure (your harddrives letter may be diffrent but mine is C) and starting at the base of your harddrive allows you to access every file on your computer but if you start elsewhere then the program can only acesss subfolders of whereever else you inputed

using namespace std;

struct files { //stack
	files* nextfolder;
	int fileamount;
	string folderpath;
	vector <string> foldernames; //these two vectors are parallel 
	vector <string> folderpaths;
	vector <string> filenames; //these three vectors are parallel 
	vector <string> filepaths;
	vector <int> filesizes;
};

files* top;

class altername {//used for template function, this class inparticular renames files
public:
	void change(const vector<string>& filedir, const string targetdir) {
		string name;
		string currentdir;
		int stringend = 0;
		int stringbegin = 0;
		string finalstring;
		string temp;
		cout << "\n\nPlease input a name scheme for all the file(s): ";
		cin >> name;
		for (int index = 0; index < filedir.size(); index++) {
			currentdir = filedir[index];
			for (int index2 = filedir[index].size(); index2 > 0; index2--) {

				if (currentdir[index2] == '.') {
					stringend = index2;
				}

				if (currentdir[index2] == '\\') {
					stringbegin = index2 + 1;
					break;
				}
			}
			finalstring = filedir[index];
			temp = name;
			name.append(to_string(index));
			finalstring.replace(stringbegin, stringend - stringbegin, name);
			filesystem::rename(filedir[index], finalstring);
			name = temp;
		}
	}
	void change(const string filedir, const string targetdir) {
		string name;
		int stringend = 0;
		int stringbegin = 0;
		string finalstring;
		cout << "\n\nPlease input a name the file: ";
		cin >> name;
		for (int index = filedir.size(); index > 0; index--) {

			if (filedir[index] == '.') {
				stringend = index;
			}

			if (filedir[index] == '\\') {
				stringbegin = index + 1;
				break;
			}
		}
		finalstring = filedir;
		finalstring.replace(stringbegin, stringend - stringbegin, name);
		filesystem::rename(filedir, finalstring);
	}
};

class alterlocation {//used for template function, this class inparticular pastes files
public:
	void change(const vector<string>& filedir, const string targetdir) {
		string currentdir;
		for (int index = 0; index < filedir.size(); index++) {
			currentdir = filedir[index];
			filesystem::copy(currentdir, targetdir);
		}
	}
	void change(const string filedir, const string targetdir) {
		filesystem::copy(filedir, targetdir);
	}
};

template <typename T> class Storage {
	vector<string> tempForSearch;//used to store folders that have already been searched
public:
	Storage(){

	}
	~Storage(){

	}
	void deleteStorage()
	{
		/*
		files.clear();
		folders.clear();
		*/
	}
	T GetLastDir() { //pops of the top of the stack also returns the stack
		files* temp;
		temp = top;
		top = top->nextfolder;
		files F = *top;
		return F;
	}
	T GetCurrentDir(string Path) //will get data on the inputed directory, and store details on files/folders in the stack for future use also returns the stack
	{
		filesystem::path P{ Path };
		bool isFile;
		files* temp = new files();
		temp->folderpath = Path;
		for (auto const& DirectoryEntry : filesystem::directory_iterator{ P })//will iterate through all directory objects in the directory
		{
			if (DirectoryEntry.is_regular_file())//checks if the entry is a file
			{
				temp->filepaths.push_back(DirectoryEntry.path().string());//stores the file path
				std::string s = DirectoryEntry.path().string();
				s.erase(s.begin(), s.end() - (s.size() - Path.size()));
				temp->filenames.push_back(s); //stores the file name
				try { //the code to get the file size has to be in a try catch since some files are system files which needs perms
					temp->filesizes.push_back(filesystem::file_size(DirectoryEntry));
				}
				catch (...) {
					temp->filesizes.push_back(0);
				}
			}
			else if (DirectoryEntry.is_directory())//checks if the entry is directory
			{
				temp->folderpaths.push_back(DirectoryEntry.path().string()); //stores the folder path
				std::string s = DirectoryEntry.path().string();
				s.erase(s.begin(), s.end() - (s.size() - Path.size()));
				temp->foldernames.push_back(s); //stores the folder name
			}
		}
		temp->nextfolder = top;
		top = temp; //saves the current stack
		files F = *temp;
		return F;
	}
	T Select() { //returns the top of the stack 
		files F = *top;
		return F;
	}
	T sort(int ascending) { //sorts files on the top of the stack with bubble sort by file size (ascending or descending)
		files F = *top;
		for (int index = 0; index < F.filesizes.size() - 1; index++) {
			for (int index2 = index + 1; index2 < F.filesizes.size(); index2++) {
				if (F.filesizes[index] > F.filesizes[index2] && ascending == 1) {//ascending

					swap(F.filesizes[index], F.filesizes[index2]);
					swap(F.filenames[index], F.filenames[index2]);
					swap(F.filepaths[index], F.filepaths[index2]);

				}
				else if (F.filesizes[index] < F.filesizes[index2] && ascending == 0) {//descending

					swap(F.filesizes[index], F.filesizes[index2]);
					swap(F.filenames[index], F.filenames[index2]);
					swap(F.filepaths[index], F.filepaths[index2]);

				}
			}
		}
		*top = F;
		return F;
	}	
};
class FileSytem : public Storage<files>
{
public:
	FileSytem()
	{

	}
	~FileSytem()
	{

	}
	template <class A, typename B, typename C> void Alter(B& x, C y) {//function that allows you to rename and paste files
		A a;

		a.change(x, y);
	}
	//where the pieces of the find process will be
	/*
	These function use recursion until there are no new folders to check then they store this current directory as checked and reset the recursion search
	*/
	/*
	bool findFound = false;//will be true when found
	bool findDone = false;//will be true when all folders are checked
	*/
	vector <string> checked;//to store checked folders
	string findFoundAt = "\n\nFile not found";//to store where its found, and if not found will say not found
	void FindBase(string fileName)
	{
		findFoundAt = "\n\nFile not found";
		files findF = *top;
		string resetSearch = "";
		findF = *top;

		while (true)//loops until its found or all folders are checked
		{
			int unsearched = 0;
			resetSearch = findF.folderpath;
			FindRecursion(resetSearch, fileName);
			break;
		}
		cout << findFoundAt;
	}
	bool FindRecursion(string pathToSearch, string fileName)
	{
		checked.clear();//clears vector for new search
		int unsearched = 0;
		filesystem::path P{ pathToSearch };
		for (auto const& DirectoryEntry : filesystem::directory_iterator{ P })//will iterate through all directory objects in the directory
		{
			if (DirectoryEntry.is_regular_file())//checks if the entry is a file
			{
				std::string test = DirectoryEntry.path().string();
				test.erase(test.begin(), test.end() - (test.size() - pathToSearch.size()));
				if (test == '\\' + fileName)
				{
					findFoundAt = "";
					string foundString = DirectoryEntry.path().string();
					findFoundAt = "The file was located at: " + foundString;
					return true;
				}
			}
			else if (DirectoryEntry.is_directory())//checks if the entry is directory
			{
				bool Checked = false;
				std::string dirToTest = DirectoryEntry.path().string();
				for (int i = 0; i < checked.size(); i++)
				{
					if (checked[i] == dirToTest)
					{
						Checked = true;
					}
				}
				if (!Checked)
				{
					unsearched++;
					FindRecursion(dirToTest, fileName);
				}
				else
				{
					checked.push_back(dirToTest);
				}
				if (unsearched == 0)
				{
					checked.push_back(pathToSearch);
				}
			}
		}
		return false;
	}
	string ReadfileFromDir(string FileNameToRead)//send in the directory, and the file name you want to read
	{
		files F = *top;
		string DirPath = F.folderpath;
		string fileContent = "";
		filesystem::path DirToSearch{ DirPath };
		bool found = false;
		string test = "";
		for (auto const& DirectoryEntry : filesystem::directory_iterator{ DirToSearch })
		{
			test = DirectoryEntry.path().string();
			test.erase(test.begin(), test.end() - (test.size() - DirPath.size()));
			if (test == '\\' + FileNameToRead)//looks for the file in the given path
			{
				found = true;
			}
			if (found)//appends the files content to a string
			{
				fileContent = "";
				char x = ' ';
				ifstream read;
				string toOpen = DirectoryEntry.path().string();
				read.open(toOpen);
				while (read.get(x))
				{
					fileContent += x;
				}
				read.close();
			}
			else if (!found)//returns string letting user know the file was not found if wasnt found
			{
				fileContent = "Error, File was not found";
			}
		}
		return fileContent;//returns the files content as a string
	}
	void printtop() { //prints the top of the stack in a nice way
		files P = *top;
		cout << "\n\nCurrent folder path:" << P.folderpath << "\n";
		cout << "Folders:\n";
		for (int index = 0; index < P.folderpaths.size(); index++) {
			cout << index << ") " << "Sub folder name: " << P.foldernames[index] << ",\tSub folder path: " << P.folderpaths[index] << "\n";
		}
		for (int index = 0; index < P.filepaths.size(); index++) {
			cout << index << ") " << "file name: " << P.filenames[index] << ",\tfile path: " << P.filepaths[index] << "\n";
		}
	}
	//writes the top of the stack to a file in json (outputs to the place where the exe for the program is located)
	void WriteToFile() {
		files F = *top;
		string Outfilename;
		cout << "\n\nPlease enter a name for your outputed file(do not include file extension): ";
		cin >> Outfilename;
		Outfilename.append(".json");
		ofstream foutstream;
		foutstream.open(Outfilename);
		foutstream << "{\n\t\"Folders\": [";
		for (int index = 0; index < F.foldernames.size(); index++) {
			foutstream << "\n\t\t\"" << F.foldernames[index] << "\"";
			if (index != F.foldernames.size()-1) {
				foutstream << ",";
			}
		}
		foutstream << "\n\t],\n\t\"Files\": [";
		for (int index = 0; index < F.filenames.size(); index++) {
			foutstream << "\n\t\t\"" << F.filenames[index] << "\"";
			if (index != F.filenames.size()-1) {
				foutstream << ",";
			}
		}
		foutstream << "\n\t]\n}";
	}
	void UIboardersTop()
	{
		cout << setfill('-') << "-" << setw(70) << "-" << endl << "CURRENT DIRECTORY:";
	}
	void UIboardersBot()
	{
		cout << setfill('-') << "-" << setw(70) << "-" << endl;
	}
};

ostream& operator<<(ostream& out, FileSytem& m)
{
	m.printtop();
	cout << endl;
	return out;
}

int main()
{
	FileSytem files;
	vector <string> folders;
	vector <string> filedirs;
	string currentdir;
	int choose = 0;
	folders = files.GetCurrentDir(DATA_FILE_PATH).folderpaths;
	int select;
	while (true)
	{
		system("cls");//clears console each loop
		files.UIboardersTop();//boarder
		//top
		cout << files;
		files.UIboardersBot();//boarder
		//options
		string menu00 = "MENU: \n";
		string menu01 = "1.)go into a folder and see the folders and files inside.\n";
		string menu02 = "2.)go back to the last folder you were in.\n";
		string menu03 = "3.)sort files by size then redisplay them.\n";
		string menu04 = "4.)print the contents of a json or txt file.\n";
		string menu05 = "5.)select all files in the current folder for pasting.\n";
		string menu06 = "6.)paste all selected files.\n";
		string menu07 = "7.)paste a specfic file.\n";
		string menu08 = "8.)rename all files.\n";
		string menu09 = "9.)rename a specfic file.\n";
		string menu010 = "10.)write contents of current directory to a json file.\n";
		string menu011 = "11.)Find a file in this directory or under this directory in the heirarchy(works well in a small to medium sized open privilege environment).\n";
		string menuEND= "or enter any other number to end the program.\n\n";

		cout << menu00 << menu01 << menu02 << menu03 << menu04 << menu05 << menu06 << menu07 << menu08 << menu09 << menu010 << menu011 << menuEND;
		cout << "Please enter a number:";
		cin >> select;
		//go into a folder and see the folders and files inside
		if (select == 1) {
			while (true) { //while loop that continues until you input an acceptable number that corresponds with a folder
				cout << "\n\nYou have choosen to step into a folder, what folder would you like to enter please input a number:";
				cin >> select;
				if (folders.size() == 0) {
					cout << "\n\nThere are no folders to step into";
					break;
				}
				else if (select <= folders.size() && select >= 0) {
					folders = files.GetCurrentDir(folders[select]).folderpaths;
					cout << files;
					break;
				}
				else {
					cout << "\n\nYou have choosen a number that is out of range please try again";
					break;
				}
			}
		}
		//brings you back to the last folder you were in
		else if (select == 2) {

			//this needs some way to prevent you from trying to go lower in the stack when there is only one layer
			folders = files.GetLastDir().folderpaths;
			cout << files;
		}
		else if (select == 3) {
			//sort files by size then redisplay them
			int ascending;
			cout << "\n\nWould you like to sort by ascending(1) or descending(0): ";
			cin >> ascending;
			if (ascending > 1 || ascending < 0) {
				try {
					throw exception("Must be a 1 or 0");
				}
				catch (std::exception& except){
					cout << "\nGot exception: " << except.what() << "\n";
					system("pause");
				}
			} else {
				filedirs = files.sort(ascending).filepaths;
			}	
		}
		else if (select == 4) {
			//print the contents of a json or txt file
			string fileName = "";
			string fileRead = "";
			string close = "";
			cout << "\n\nplease enter the name of the file you want to read: ";
			cin >> fileName;
			fileRead = files.ReadfileFromDir(fileName);
			system("cls");//clears and shows the files contents
			cout << "\n\nThe files contents is:\n\n" + fileRead;
			cout << "\n\n enter any key to close read.";
			cin >> close;
		}
		//select all files in the current folder for pasting
		else if (select == 5) {
			filedirs = files.Select().filepaths;
			cout << files;
			cout << "\n\nAll files successfully selected";
		}
		//paste all selected files
		else if (select == 6) {
			files.Alter<alterlocation, vector<string>, string>(filedirs, currentdir);
			cout << files;
			cout << "\n\nAll files successfully pasted";
		}
		else if (select == 7) {//paste a specfic file
			currentdir = files.Select().folderpath;
			for (int index = 0; index < filedirs.size(); index++) {
				cout << index << ") " << "file path: " << filedirs[index] << "\n";
			}
			cout << "\nPlease enter a number of the file you want to paste into the current location: ";
			cin >> choose;
			currentdir = files.Select().folderpath;
			files.Alter<alterlocation, string, string>(filedirs[choose], currentdir);
			cout << files;
			cout << "\n\nFile successfully pasted";
		}
		//rename all file
		else if (select == 8) {
			currentdir = files.Select().folderpath;
			filedirs = files.Select().filepaths;
			files.Alter<altername, vector<string>, string>(filedirs, currentdir);
			cout << "\n\nAll files successfully renamed";
		}
		//rename a specfic file
		else if (select == 9) {
			for (int index = 0; index < filedirs.size(); index++) {
				cout << index << ") " << "file path: " << filedirs[index] << "\n";
			}
			cout << "\nPlease enter a number of the file you want to rename: ";
			cin >> choose;
			currentdir = files.Select().folderpath;
			filedirs = files.Select().filepaths;
			files.Alter<altername, string, string>(filedirs[choose], currentdir);
			cout << "\n\nFile successfully renamed";
		}
		//write current folder to a json file
		else if (select == 10) {
			files.WriteToFile();
			cout << "\n\nFile successfully created";
		}
		//Find a file in this directory or under this directory in the heirarchy(works well in a small to medium sized open privilege environment)
		else if (select == 11)
		{
			string fileName = "";
			cout << "\n\nplease enter the file you want to find: ";
			cin >> fileName;
			try
			{
				files.FindBase(fileName);
			}
			catch (...)
			{
				cout << "\n\nError: The search was to broad, \n\n or This crashes if you try to search the whole system,\n\nor there are search restrictions based on the programs prevleges, \n\nor it has issues with white space in the search term,\n\nor the directory iterator errored out";
				//to fix the unknown error look at these
				/*
				* http://cplusplus.com/forum/beginner/193954/
				* https://stackoverflow.com/questions/64091128/fsrecursive-directory-iterator-exception-unhandled
				* https://stackoverflow.com/questions/62988629/c-stdfilesystemfilesystem-error-exception-trying-to-read-system-volume-inf
				* https://stackoverflow.com/questions/55092205/stdfilesystemrecursive-directory-iterator-crashes-when-iterating-over-the-ex
				* https://stackoverflow.com/questions/47781049/unhandled-exception-at-microsoft-c-exception-stdbad-alloc-at-memory-loca
				* https://stackoverflow.com/questions/63228069/filesystemcreate-directories-error-when-using-time
				*/
			}
		}
		else {
			break;
		}
	}
	return 0;
}