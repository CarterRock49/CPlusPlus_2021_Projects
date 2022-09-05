// Programming assignment MusicMachine.
// Macomb Community College
// Written by Carter Rock and Madison Latimer

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

////
const std::string DATA_FILE_PATH = "TestData\\";
////////////////////////////////////////////////////////////////////////////////////////////////
namespace MusicMachine //all classes and enums are within this namespace
{
	enum class MUSIC_TYPE //stores all the types of music there are
	{
		UNKNOWN,
		CHAMBER,
		ORCHESTRAL,
		ROCK,
		ELECTRONIC,
		POP,
		JAZZ,
		BLUES
	};
	///////////////
	class Music
	{
	protected:
		enum class TEMPO //stores the types of tempos a song can have
		{
			UNKNOWN,
			SLOW,
			FAST
		};
		//vector<string> Instruments;
		MUSIC_TYPE MusicType;
		TEMPO Tempo;
		string ArtistName;
		string Location;
		string Origin;
		//string Tempo;
	public:
		virtual ~Music() //destructor
		{

		}
		Music(json& j, MUSIC_TYPE type, TEMPO tempo)
		{
			ArtistName = j["Artist"]; //ArtistName comes from "Artist" in the .json file
			Location = j["Location"]; //Location defines where the song is stored as a file
			Origin = j["Origin"]; //Origin is the country of origin

			/*
			if (MusicType == MUSIC_TYPE::CHAMBER || MusicType == MUSIC_TYPE::ORCHESTRAL)
				Tempo = TEMPO::SLOW;
			else
				Tempo = TEMPO::FAST;
			*/
			//string name = j["Name"];
		}
		MUSIC_TYPE GetMusicType() const //retrieves the type of music, that being any from the enum MUSIC_TYPE
		{
			return MusicType;
		}
		bool IsTempoFast() const //bool that asks if the tempo of the song is fast or not
		{
			if (Tempo == TEMPO::SLOW) 
			{
				return false;
			}
			else if (Tempo == TEMPO::FAST)
			{
				return true;
			}
			return false;
		}
		virtual void Play() const //pure virtual play function that says what song type is playing
		{
			cout << "Playing " << "MUSIC TYPE" << " music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const //prints out the various bits of information stored from the .json file about the specific song playing
		{
			cout << ArtistName + "," + Origin + "," + Location + ",";
			return ArtistName + "," + Origin + "," + Location + ",";
		}
		virtual bool operator==(const Music& m)
		{
			return ArtistName == m.ArtistName && Origin == m.Origin && Location == m.Location;
		}
	};

	class Classical : public Music //first music type is Classical
	{
	private:
		string Key;
		string PieceName;
	public:
		virtual ~Classical()
		{

		}
		Classical(json& j, MUSIC_TYPE MusicType, TEMPO Tempo) : Music(j, MusicType, Tempo)
		{
			Tempo = TEMPO::SLOW; //all classical music has a Slow tempo
			Key = j["Key"]; //all classical music has a Key that defines what key the song is played in
			PieceName = j["Name"]; //classical song names are called Pieces, and are stored in PieceName
		}
		string GetKey() //there are keys A, B, C, D, E, F, and G
		{
			return Key;
		}
		Classical& operator++(int u)
		{
			if (Key == "A"){
				Key = "B";
			} else if (Key == "B"){
				Key = "C";
			} else if (Key == "C"){
				Key = "D";
			} else if (Key == "D"){
				Key = "E";
			} else if (Key == "E"){
				Key = "F";
			} else if (Key == "F"){
				Key = "G";
			} else if (Key == "G"){
				Key = "A";
			}	
			return *this;
		}
		virtual bool operator==(const Classical& c)
		{
			return Key == c.Key;
		}
		virtual void Play() const //tells user Classical Music is playing
		{
			cout << "Playing Classical music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			Music::PrettyPrintClassAttributes(); //calls to Music() pretty print function to print out that portion of the song info
			cout << Key + "," + PieceName + ",";
			return Key + "," + PieceName + ",";
		}
	};

	class NonClassical : public Music
	{
	private:
		string SongName;
	public:
		virtual ~NonClassical()
		{

		}
		NonClassical(json& j, MUSIC_TYPE MusicType, TEMPO Tempo) : Music(j, MusicType, Tempo)
		{
			Tempo = TEMPO::FAST; //all NonClassical songs have fast tempos
			SongName = j["Name"]; //song names are NOT pieces, and are stored in SongName
		}
		virtual bool operator==(const NonClassical& nc)
		{
			return SongName == nc.SongName;
		}
		virtual void Play() const
		{
			cout << "Playing Non Classical music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			Music::PrettyPrintClassAttributes();
			cout << SongName << ",";
			return SongName;
		}
	};

	class Chamber : public Classical //Chamber music inherits from Classical
	{
	private: //all nested types are private
		struct Instruments //stores the instruments used in the song in a struct
		{
			string One;
			string Two;
			string Three;
			string Four;

			bool operator==(const Instruments& i) //this is NOT virtual
			{
				return One == i.One && Two == i.Two && Three == i.Three && Four == i.Four;
			}
		};
		Instruments Instruments;

	public:
		//Instruments i; //i is used to store the instruments used in the song for each string in struct
		Chamber(json& j) : Classical(j, MusicType, Tempo)
		{
			json& instru = j["Instruments"]; //instru points to the "Instruments" in the .json file so it can access the Instruments stored there
			//Instruments i;
			MusicType = MUSIC_TYPE::CHAMBER; //denotes music type
			//instruments One, Two, Three, and Four
			Instruments.One = instru["1"];
			Instruments.Two = instru["2"];
			Instruments.Three = instru["3"];
			Instruments.Four = instru["4"];
		}
		virtual bool operator==(const Chamber& c)
		{
			return Instruments == c.Instruments;
		}
		virtual void Play() const
		{
			cout << "Playing Chamber music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const  //prints out all the attributes unique to Classical music
		{
			Classical::PrettyPrintClassAttributes();
			return /*ArtistName + "," + Origin + "," + Location + "," + PieceName + "," + */
				Instruments.One + "," + Instruments.Two + "," + Instruments.Three + "," + Instruments.Four;
		}
	};

	class Orchestral : public Classical //Orchestral inherets from Classical
	{
	private:
		vector<string> Instruments; //stores the Instruments used in the song into a vector
		//string Key; //what Key the song is played at
		char tempchar;
		string Lastinstru;
	public:
		Orchestral(json& j) : Classical(j, MusicType, Tempo) {
			MusicType = MUSIC_TYPE::ORCHESTRAL;

			//stores every Instrument used into the vector Instruments
			for (auto& d : j["Instruments"])
			{
				Lastinstru = to_string(d);
				stringstream Instru(Lastinstru);
				Instru.get(tempchar); //gets rid of the first quotation
				getline(Instru, Lastinstru, '"'); //grabs the rest of the string minus the last quotation
				Instruments.push_back(Lastinstru);
			}
		}
		virtual bool operator==(const Orchestral& o)
		{
			return Instruments == o.Instruments;
		}
		virtual void Play() const
		{
			cout << "Playing Orchestral music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			Classical::PrettyPrintClassAttributes();

			//inst is used to store the data in the vector so it can be returned by the function with commas to separate
			string instrum;
			for (int x = 0; x < Instruments.size(); x++)
			{
				instrum += Instruments[x];
				instrum += Instruments[x];
				instrum += ",";
			}
			instrum.pop_back(); //deletes the extra comma
			
			return 
				/*ArtistName + "," + Origin + "," + Location + "," + PieceName + "," + Key + "," +*/ instrum;
		}
	};

	class Pop : public NonClassical //Pop inherets from NonClassical
	{
	private:
		string DateOfRanking; //denotes a date for the song's rank
		int Top40Ranking; //denotes an int (that is later cast to a string) that states the song's top 40 ranking
	public:
		Pop(json& j) : NonClassical(j, MusicType, Tempo)
		{
			MusicType = MUSIC_TYPE::POP;
			DateOfRanking = j["DateOfRanking"];
			Top40Ranking = j["Rank"];
		}
		bool operator<(const Pop& p)
		{
			return Top40Ranking < p.Top40Ranking;
		}
		bool operator>(const Pop& p)
		{
			return Top40Ranking > p.Top40Ranking;
		}
		virtual bool operator==(const Pop& p)
		{
			return Top40Ranking == p.Top40Ranking;
		}
		virtual void Play() const
		{
			cout << "Playing Pop music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			NonClassical::PrettyPrintClassAttributes();

			return /*ArtistName + "," + Origin + "," + Location + "," + SongName + "," +*/ DateOfRanking + "," + to_string(Top40Ranking);
		}
	};

	class Jazz : public NonClassical //Jazz inherets from NonClassical
	{
	private:
		enum class IMPROVISATION //an enum to store the types of improvisation that are in the song
		{
			NONE,
			SOME,
			ALOT
		};
		IMPROVISATION Improvisation;
		string ImprovisationToString() const
		{
			//determines what kind of enum Improvisation is and converts it to the appropriate string
			switch (Improvisation)
			{
			case IMPROVISATION::NONE:
				return "None";
			case IMPROVISATION::SOME:
				return "Some";
			case IMPROVISATION::ALOT:
				return "Alot";
			}
			return "null";
		}
	public:
		Jazz(json& j) : NonClassical(j, MusicType, Tempo)
		{
			Improvisation = IMPROVISATION::SOME; //this might be "cheating" a little, but the file denotes the Jazz song's tempo as being Some
			MusicType = MUSIC_TYPE::JAZZ;
			//string improvToString = ImprovisationToString();
			//string Improvisation = j["Improvisation"];
		}
		virtual bool operator==(const Jazz& j)
		{
			return Improvisation == j.Improvisation;
		}
		virtual void Play() const
		{
			cout << "Playing Jazz music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const  //ImprovisationToString is a string type and can be returned directly 
		{
			NonClassical::PrettyPrintClassAttributes();

			return /*ArtistName + "," + Origin + "," + Location + "," + SongName + "," +*/ ImprovisationToString();
		}
	};

	class Rock : public NonClassical //Rock iherets from NonClassical
	{
	private:
		enum class VOLUME //enum Volume stores the volume types the song can be at
		{
			NA,
			QUIET, //in the diagram, this said QUITE, but I changed it to QUIET
			LOUD,
			VERYLOUD
		};
		string VolumeToString() const
		{
			switch (Volume)
			{
			case VOLUME::NA:
				return "NA";
			case VOLUME::QUIET:
				return "Quiet";
			case VOLUME::LOUD:
				return "Loud";
			case VOLUME::VERYLOUD:
				return "Very Loud";
			default:
				return "null";
			}
			return "null";
		}
		int Expletives; //states as an int how many explicit words are in the song
		bool GuitarsOnly; //bool that asks if there are guitars only in the song or not
		VOLUME Volume;
	public:
		Rock(json& j) : NonClassical(j, MusicType, Tempo)
		{
			Volume = VOLUME::LOUD; //again, the file denoted the song as being Loud
			MusicType = MUSIC_TYPE::ROCK;
			//string volumeToString = VolumeToString();
			Expletives = j["Expletives"];
			GuitarsOnly = j["Guitars Only"];
			//string Guitars = to_string(j["Guitars Only"]);
			//string Volume = j["Recommended Volume"];
			//string Expletives = to_string(j["Expletives"]);
		}
		bool IsLoud()
		{
			switch(Volume){
				case VOLUME::LOUD:
					return true;
				default:
				return false;
			}
		}
		bool IsQuiet() //the diagram said IsQuite but I changed it to Quiet
		{
			switch(Volume){
				case VOLUME::QUIET:
					return true;
				default:
				return false;
			}
		}
		bool IsVeryLoud()
		{
			switch(Volume){
				case VOLUME::VERYLOUD:
					return true;
				default:
				return false;
			}
		}
		Rock& operator++(int u)
		{
			switch (Volume){
			case VOLUME::QUIET:
				Volume = VOLUME::LOUD;
				break;
			case VOLUME::LOUD:
				Volume = VOLUME::VERYLOUD;
				break;
			case VOLUME::VERYLOUD:
				Volume = VOLUME::QUIET;
				break;
			}
			return *this;
		}
		virtual bool operator==(const Rock& r)
		{
			return Volume == r.Volume;
		}
		virtual void Play() const
		{
			cout << "Playing Rock music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			NonClassical::PrettyPrintClassAttributes();

			return /*ArtistName + "," + Origin + "," + Location + "," + SongName + "," +*/ to_string(Expletives) + "," + VolumeToString();
		}
	};

	class Electronic : public NonClassical //Electronic inherets from NonClassical
	{
	private:
		int BeatsPerMinute; //denotes as an int the BPM of the song
	public:
		Electronic(json& j) : NonClassical(j, MusicType, Tempo)
		{
			MusicType = MUSIC_TYPE::ELECTRONIC;
			BeatsPerMinute = j["BeatsPerMin"];
			//string BeatsPerMin = to_string(j["BeatsPerMin"]);
		}
		const unsigned int GetBeatsPerMinute()
		{
			return BeatsPerMinute;
		}
		Electronic& operator++()
		{
			BeatsPerMinute++;
			return *this;
		}
		Electronic& operator--()
		{
			BeatsPerMinute--;
			return *this;
		}
		virtual bool operator==(const Electronic& e)
		{
			return BeatsPerMinute == e.BeatsPerMinute;
		}
		virtual void Play() const
		{
			cout << "Playing Electronic music..." << endl;
		}
		virtual string PrettyPrintClassAttributes() const
		{
			NonClassical::PrettyPrintClassAttributes();
			return to_string(BeatsPerMinute);
		}
	};
	////////////////
	class Machine //machine opens the .json file and has the nlohmann parser parse it
	{
	private:
		json jf;
		Music** Songs; //pointer to a pointer
		ifstream f;
		int songCount;
		/*
		vector<Chamber> SongsChamber;
		vector<Electronic> SongsElectronic;
		vector<Jazz> SongsJazz;
		vector<Orchestral> SongsOrchestral;
		vector<Pop> SongsPop;
		vector<Rock> SongsRock;
		*/
	public:
		virtual ~Machine() //destructor to prevent memory leakage from Music** Songs pointer
		{
			for (int i = 0; i < songCount; i++){
				delete Songs[i];
			}
			delete Songs;
		}
		Machine(string fileName, const int iMaxSongs)
		{
			//opens the file that the fileName varible has stored in it
			f.open(fileName);
			
			Songs = new Music* [iMaxSongs];
			/*
			for(int i = 0; i < iMaxSongs; i++){
				Songs[i] = nullptr;
			}
			*/
		}
		const size_t Parse() //parses file using nlohmann and counts the number of songs
		{
			jf = json::parse(f);
			songCount = 0;
			if (jf.is_array()){
				for (json& c : jf){
					if (c["Type"] == "Pop")
						Songs[songCount++] = new Pop(c);
					else if (c["Type"] == "Rock")
						Songs[songCount++] = new Rock(c);
					else if (c["Type"] == "Orchestral")
						Songs[songCount++] = new Orchestral(c);
					else if (c["Type"] == "Jazz")
						Songs[songCount++] = new Jazz(c);
					else if (c["Type"] == "Electronic")
						Songs[songCount++] = new Electronic(c);
					else if (c["Type"] == "Chamber")
						Songs[songCount++] = new Chamber(c);
					else if (c["Type"] == "Rock")
						Songs[songCount++] = new Rock(c);					
				}
			}
			return songCount;
		}
		/* no longer used
		const Music* GetSong(const int i)
		{	
			return Songs[i];
		}
		*/
		const int GetSongCount()
		{
			//songCount = 0;

			/*
			for (json& j : jf)
			{
				if (j["Type"] == "Pop" || j["Type"] ==  "Orchestral" || j["Type"] == "Jazz" || j["Type"] == "Electronic" || j["Type"] == "Chamber" || j["Type"] == "Rock")
				{
					songCount++;
				}
			}
			*/
			return songCount;
		}
		Music& operator[](int i) 
		{
			//Songs does not have elements that go beyond 6, so the switch sets i to the appropriate element that corresponds to the correct element in the Songs array
			switch (i)
			{
			case 6:
				i = 0;
				break;
			case 7:
				i = 1;
				break;
			case 8:
				i = 2;
				break;
			case 9:
				i = 3;
				break;
			case 10:
				i = 4;
				break;
			case 11:
				i = 5;
				break;
			}

			return *Songs[i];
		}
		/*
		const vector<Chamber> GetChamberSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Chamber") {
					SongsChamber.push_back(Chamber(j));
				}
			}

			return SongsChamber;
		}
		const vector<Electronic> GetElectronicSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Electronic") {
					SongsElectronic.push_back(Electronic(j));
				}
			}
			return SongsElectronic;
		}
		const vector<Jazz> GetJazzSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Jazz") {
					SongsJazz.push_back(Jazz(j));
				}
			}
			return SongsJazz;
		}
		const vector<Orchestral> GetOrchestralSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Orchestral") {
					SongsOrchestral.push_back(Orchestral(j));
				}
			}
			return SongsOrchestral;
		}
		const vector<Pop> GetPopSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Pop") {
					SongsPop.push_back(Pop(j));
				}
			}
			return SongsPop;
		}
		const vector<Rock> GetRockSongs()
		{
			for (json& j : jf) {
				if (j["Type"] == "Rock") {
					SongsRock.push_back(Rock(j));
				}
			}
			return SongsRock;
		}
		*/
	};
}

//Music stream insertion operator with file scope that outputs PrettyPrint
ostream& operator<<(ostream& COUT, MusicMachine::Music& m)
{
	COUT << m.PrettyPrintClassAttributes() << endl;

	return COUT;
}

////////////////////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <crtdbg.h>
TEST_CASE("Testing my Music Machine")
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SUBCASE("testing the parsing of file - JsonObject8a.json")
	{
		const int MAX_SONGS = 10;
		MusicMachine::Machine m(DATA_FILE_PATH + "JsonObject8a.json", MAX_SONGS);

		const int MAX_RECORDS = 6;

		// Invoke the parsing process
		CHECK(m.Parse() == MAX_RECORDS);

		const int iSongCount = m.GetSongCount();

		//Notice that the loop is done twice
		for (int i = 0; i < iSongCount * 2; i++)
		{
			//Virtual function of base class "plays" the correctr instance of derived class
			m[i].Play();

			cout << m[i] << endl;
			cout << endl;

			//Test the "==" operator for the Music class...
			CHECK((m[i] == m[i]) == true);

			//Unit testing the derived classes...
			if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::CHAMBER)
			{
				MusicMachine::Chamber& c = dynamic_cast<MusicMachine::Chamber&>(m[i]);
				CHECK((c == c) == true);
				CHECK((c).GetKey() == "G");
				CHECK((c++).GetKey() == "A");
				CHECK((c++).GetKey() == "B");
				CHECK((c++).GetKey() == "C");
				CHECK((c++).GetKey() == "D");
				CHECK((c++).GetKey() == "E");
				CHECK((c++).GetKey() == "F");
				CHECK((c++).GetKey() == "G");
			}
			else if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::ORCHESTRAL)
			{
				MusicMachine::Orchestral& o = dynamic_cast<MusicMachine::Orchestral&>(m[i]);
				CHECK((o == o) == true);
			}
			else if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::POP)
			{
				MusicMachine::Pop& p = dynamic_cast<MusicMachine::Pop&>(m[i]);
				CHECK((p == p) == true);
				CHECK((p < p) == false);
				CHECK((p > p) == false);

			}
			else if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::JAZZ)
			{
				MusicMachine::Jazz& j = dynamic_cast<MusicMachine::Jazz&>(m[i]);
				CHECK((j == j) == true);
			}
			else if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::ROCK)
			{
				MusicMachine::Rock& r = dynamic_cast<MusicMachine::Rock&>(m[i]);
				CHECK((r == r) == true);
				CHECK((r).IsLoud() == true);
				CHECK((r++).IsLoud() == false);
				CHECK((r).IsVeryLoud() == true);
				CHECK((r++).IsVeryLoud() == false);
				CHECK((r).IsQuiet() == true);
				CHECK((r++).IsQuiet() == false);
			}
			else if (m[i].GetMusicType() == MusicMachine::MUSIC_TYPE::ELECTRONIC)
			{
				MusicMachine::Electronic& e = dynamic_cast<MusicMachine::Electronic&>(m[i]);
				CHECK((e == e) == true);
				CHECK((e).GetBeatsPerMinute() == 141);
				CHECK((++e).GetBeatsPerMinute() == 142);
				CHECK((--e).GetBeatsPerMinute() == 141);
			}
		}

		/* Commented out old test cases
		const int MAX_SONGS = 10;
		MusicMachine::Machine m(DATA_FILE_PATH + "JsonObject7a.json", MAX_SONGS);

		const int MAX_RECORDS = 6;

		// Invoke the parsing process
		CHECK(m.Parse() == MAX_RECORDS);

		const int iSongCount = m.GetSongCount();
		for (int i = 0; i < iSongCount; i++)
		{
			//Notice "pSong" is a pointer to only the base class, "Music".
			const MusicMachine::Music* pSong = m.GetSong(i);

			//Given the base class, using C++ RTTI "typeinfo" to print the name of the derived class.
			cout << "This is - \"" << typeid(*pSong).name() << "\"" << endl;

			//Virtual function of base class "plays" the correct instance of derived class.

			pSong->Play();
			cout << pSong->PrettyPrintClassAttributes() << endl;
			cout << endl;

			switch (pSong->GetMusicType())
			{
				
			case MusicMachine::MUSIC_TYPE::CHAMBER:
				CHECK(dynamic_cast<const MusicMachine::Chamber*>(pSong)->IsTempoFast() == false);
				break;
			case MusicMachine::MUSIC_TYPE::ORCHESTRAL:
				CHECK(dynamic_cast<const MusicMachine::Orchestral*>(pSong)->IsTempoFast() == false);
				break;
			case MusicMachine::MUSIC_TYPE::POP:
				CHECK(dynamic_cast<const MusicMachine::Pop*>(pSong)->IsTempoFast() == true);
				break;
			case MusicMachine::MUSIC_TYPE::JAZZ:
				CHECK(dynamic_cast<const MusicMachine::Jazz*>(pSong)->IsTempoFast() == true);
				break;
			case MusicMachine::MUSIC_TYPE::ROCK:
				CHECK(dynamic_cast<const MusicMachine::Rock*>(pSong)->IsTempoFast() == true);
				break;
			case MusicMachine::MUSIC_TYPE::ELECTRONIC:
				CHECK(dynamic_cast<const MusicMachine::Electronic*>(pSong)->IsTempoFast() == true);
				break;
			default:
				cerr << "Error determining music type!" << endl;
			}
			
		}
		
		/// Chamber
		vector<MusicMachine::Chamber> ChamberSongs = m.GetChamberSongs();
		for (auto& cs : ChamberSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::CHAMBER);
			CHECK(cs.IsTempoFast() == false);
		}

		/// Orchestral
		vector<MusicMachine::Orchestral> OrchestralSongs = m.GetOrchestralSongs();
		for (auto& cs : OrchestralSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::ORCHESTRAL);
			CHECK(cs.IsTempoFast() == false);
		}

		/// Pop
		vector<MusicMachine::Pop> PopSongs = m.GetPopSongs();
		for (auto& cs : PopSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::POP);
			CHECK(cs.IsTempoFast() == true);
		}

		/// Jazz
		vector<MusicMachine::Jazz> JazzSongs = m.GetJazzSongs();
		for (auto& cs : JazzSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::JAZZ);
			CHECK(cs.IsTempoFast() == true);
		}

		/// Rock
		vector<MusicMachine::Rock> RockSongs = m.GetRockSongs();
		for (auto& cs : RockSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::ROCK);
			CHECK(cs.IsTempoFast() == true);
		}

		/// Electronic
		vector<MusicMachine::Electronic> ElectronicSongs = m.GetElectronicSongs();
		for (auto& cs : ElectronicSongs)
		{
			cs.Play();
			cout << cs.PrettyPrintClassAttributes() << endl << endl;
			CHECK(cs.GetMusicType() == MusicMachine::MUSIC_TYPE::ELECTRONIC);
			CHECK(cs.IsTempoFast() == true);
		}
		*/
	}
}