//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "pch.h"
#include "MainPage.xaml.h"
#include <thread>
#include <shared_mutex>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <locale> 
#include <codecvt>


using namespace Projet_STER;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Data::Json;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

using namespace std;
// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

DWORD MyThreadIDDec;
HANDLE MyThreadDec;

std::mutex My_mutex_dec;

Platform::String^ nomMote;
Platform::String^ temprature;
Platform::String^ humidite;
Platform::String^ batterie;
Platform::String^ lumiere;

Projet_STER::MainPage^ instance;

auto url = String::Concat("http://iotlab.telecomnancy.eu:8080/iotlab/rest/data/1/temperature-light2-light1-battery_indicator-humidity/1/", nomMote);
Windows::Foundation::Uri^ uri = ref new Uri(url);


HANDLE Verrou;
shared_mutex Ver;

struct MoteInfoCSV {
	Platform::String^ numMote;
	Platform::String^ nomMote;
	float latitude;
	float longitude;
};

std::vector<MoteInfoCSV> listOfMote;

static UINT Inc()
{
	Sleep(1);	//attente pour la création du Mutex
	while (TRUE) {
		Ver.lock();
		Windows::Web::Http::HttpClient^ httpClient = ref new HttpClient();
		create_task(httpClient->GetStringAsync(uri))
			.then([=](Platform::String^ Tet)
		{

			OutputDebugString(L"Je passe là\n");
			auto objJson = JsonObject::Parse(Tet);
			Platform::String^ label = "temperature";
			JsonArray^ data = objJson->GetObject()->GetNamedArray("data")->GetArray();
			int jsonArraySize = data->Size;
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					temprature = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
				}
			}
			label = "humidity";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					humidite = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
				}
			}
			label = "light1";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					lumiere = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
				}
			}
			label = "battery_indicator";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					batterie = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
				}
			}
			

			return task_from_result();
		});
	}
	return 0;
}

MainPage::MainPage()
{
	InitializeComponent();
	MainPage::getDataFromServer();
	auto timer = ref new Windows::UI::Xaml::DispatcherTimer();
	TimeSpan ts;
	ts.Duration = 2000;
	timer->Interval = ts;
	timer->Start();
	auto registrationtoken = timer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnTick);

	// Création des Threads
	thread TH_Inc(Inc);
	TH_Inc.detach();


	Ver.lock();

	//Sauvegarde
	instance = this;
}

int MainPage::getDataFromServer()
{
	return 0;
}


void Projet_STER::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Amphis Nord
	nomMote = "9.138"; 
	Ver.unlock();
	string s = "9.138";
	//parseCSV(s, L"Google");
}

void Projet_STER::MainPage::OnTick(Object ^ sender, Object ^ e) {
	editTemperature->Text = temprature;
	editHumidite->Text = humidite;
	editBatterie->Text = batterie;
	editLumiere->Text = lumiere;
}

void Projet_STER::MainPage::AfficherInfo(Platform::String^ nomMote)
{

	
}

/*
	Permet d'extraire les données de lumière des données reçu d'une mote
*/
Platform::String^ Projet_STER::MainPage::recupererDonneesMote(JsonObject^ objJson, Platform::String^ label)
{
	Platform::String^ res = "";
	JsonArray^ data = objJson->GetObject()->GetNamedArray("data")->GetArray();
	int jsonArraySize = data->Size;
	for (int i = 0; i < jsonArraySize; i++) {
		if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
			return data->GetObjectAt(i)->GetNamedValue("value")->ToString();
		}
	}
	return res;
}


/*
	Permet d'extraire les données de lumière des données reçu d'une mote
*/
Platform::String^ Projet_STER::MainPage::parseCSV(std::string nomMote, Platform::String^ elmenet)
{
	

	/*FILE *fp;
	fp = fopen("C:\\motes_information.csv", "r");

	StorageFolder^ storageFolder = ApplicationData::Current->LocalFolder;
	create_task(storageFolder->GetFileAsync("sample.txt")).then([](StorageFile^ sampleFile)
	{
		float latitude, logitude;
	string numMote, nomMoteGet;
		while (fscanf(fp, "%g,%g,%s,%s\n", &latitude, &logitude, &numMote, &nomMoteGet) == 2)
		{
			if (numMote == "9.138") {
				OutputDebugString(L"Trouvé");
			}
		}
	});*/
	StorageFolder^ storageFolder = ApplicationData::Current->LocalFolder;
	create_task(storageFolder->GetFileAsync("motes_information.csv")).then([](StorageFile^ sampleFile)
	{
		return FileIO::ReadBufferAsync(sampleFile);

	}).then([this](Streams::IBuffer^ buffer)
	{
		auto dataReader = DataReader::FromBuffer(buffer);
		String^ bufferText = dataReader->ReadString(buffer->Length);
		wstring csvFile = bufferText->Data();
		float latitude, logitude;
		string numMote, nomMoteGet;
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		std::string s = converter.to_bytes(csvFile);
		std::string delimiter = "\n";

		size_t pos = 0;
		std::string token;
		std::string csvDelimiter = ",";
		while ((pos = s.find(delimiter)) != std::string::npos) {

			token = s.substr(0, pos);
			std::vector<std::string> res = split(token, csvDelimiter);
			MoteInfoCSV info;
			info.nomMote = ref new String(converter.from_bytes(res[3].c_str()).c_str());
			info.numMote = ref new String(converter.from_bytes(res[2].c_str()).c_str());
			info.latitude = std::stod(res[0]);
			info.longitude = std::stod(res[1]);
			listOfMote.push_back(info);
			s.erase(0, pos + delimiter.length());
		}
	});
	
	return L"9.138";
}

std::vector<std::string> Projet_STER::MainPage::split(std::string str, std::string sep) {
	char* cstr = const_cast<char*>(str.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok_s(cstr, sep.c_str(), NULL);
	while (current != NULL) {
		arr.push_back(current);
		current = strtok_s(NULL, sep.c_str(), NULL);
	}
	return arr;
}