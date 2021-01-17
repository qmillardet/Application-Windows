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
# define M_PI           3.14159265358979323846  /* pi */


using namespace Projet_STER;
using namespace Windows::Data::Xml::Dom;

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
using namespace Windows::UI::Notifications;
using namespace Windows::Data::Json;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

using namespace Windows::UI::Core;

using namespace Windows::Devices::Geolocation;

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
Platform::String^ nomMoteSimple;

Projet_STER::MainPage^ instance;

auto url = String::Concat("http://iotlab.telecomnancy.eu:8080/iotlab/rest/data/1/temperature-light2-light1-battery_indicator-humidity/1/", nomMote);
Windows::Foundation::Uri^ uri = ref new Uri(url);

HANDLE Verrou;
shared_mutex Ver;

bool notTake = false;

Brush^ originalColor;
bool originalColorBool = false;
bool gpsReady = false;

struct MoteInfoCSV {
	Platform::String^ numMote;
	Platform::String^ nomMote;
	double latitude;
	double longitude;
};

std::vector<MoteInfoCSV> listOfMote;

static UINT Inc()
{
	Sleep(1);	//attente pour la création du Mutex
	while (TRUE) {
		Ver.lock();
		bool reussi = false;
		url = String::Concat("http://iotlab.telecomnancy.eu:8080/iotlab/rest/data/1/temperature-light2-light1-battery_indicator-humidity/1/", nomMote);
		uri = ref new Uri(url);
		Windows::Web::Http::HttpClient^ httpClient = ref new HttpClient();
		auto my_task = create_task(httpClient->GetStringAsync(uri))
			.then([](Platform::String^ Tet)
		{

			auto objJson = JsonObject::Parse(Tet);
			Platform::String^ label = "temperature";
			JsonArray^ data = objJson->GetObject()->GetNamedArray("data")->GetArray();
			int jsonArraySize = data->Size;
			bool found = false;
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					temprature = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
					found = true;
				}
			}
			if (!found) {
				temprature = L"Aucune données";

			}
			found = false;
			label = "humidity";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					humidite = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
					found = true;
				}
			}
			if (!found) {
				humidite = L"Aucune données";

			}
			found = false;
			label = "light1";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					lumiere = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
					found = true;
				}
			}
			if (!found) {
				lumiere = L"Aucune données";

			}
			found = false;
			label = "battery_indicator";
			for (int i = 0; i < jsonArraySize; i++) {
				if (data->GetObjectAt(i)->GetNamedString("label")->Equals(label)) {
					batterie = data->GetObjectAt(i)->GetNamedValue("value")->ToString();
					found = true;
				}
			}
			if (!found) {
				batterie  = L"Aucune données";

			}


			return task_from_result();
		});
		try {
			my_task.wait();
		}
		catch (Exception^ e) {
			String^ xml = "<toast activationType='foreground'>"
				"<visual>"
				"<binding template='ToastGeneric'>"
				"<text>Erreur</text>"
				"</binding>"
				"</visual>"
				"</toast>";

			XmlDocument^ doc = ref new XmlDocument();
			doc->LoadXml(xml);

			auto binding = doc->SelectSingleNode("//binding");

			auto el = doc->CreateElement("text");
			el->InnerText = L"Erreur lors du chargement des données";
			binding->AppendChild(el); // Add content to notification

			auto toast = ref new ToastNotification(doc);

			ToastNotificationManager::CreateToastNotifier()->Show(toast); // Show the toast
		}
		notTake = false;
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

	MoteInfoCSV info;
	info.nomMote = L"Amphi Nord";
	info.numMote = L"9.138";
	info.latitude = 48.669422;
	info.longitude = 6.155112;

	MoteInfoCSV info1;
	info1.nomMote = L"Amphi Sud";
	info1.numMote = L"111.130";
	info1.latitude = 48.668837;
	info1.longitude = 6.154990;

	MoteInfoCSV info2;
	info2.nomMote = L"Salle E1.22";
	info2.numMote =L"151.105";
	info2.longitude = 6.155363;
	info2.latitude = 48.668922;

	MoteInfoCSV info3;
	info3.nomMote = L"Salle N0.3";
	info3.numMote = L"32.131";
	info3.latitude = 48.669400;//
	info3.longitude = 6.155340;


	MoteInfoCSV info4;
	info4.nomMote = L"Bureau 2.6";
	info4.numMote = L"97.145";
	info4.latitude = 48.669439;
	info4.longitude = 6.155265;

	MoteInfoCSV info5;
	info5.nomMote = L"Bureau 2.7";
	info5.numMote = L"120.99";
	info5.longitude = 6.155269;
	info5.latitude = 48.669419;

	MoteInfoCSV info6;
	info6.nomMote = L"Bureau 2.8";
	info6.numMote = L"200.124";
	info6.longitude = 6.155287;
	info6.latitude = 48.669394;

	MoteInfoCSV info7;
	info7.nomMote = L"Bureau 2.9";
	info7.numMote = L"53.105";
	info7.longitude = 6.155310;
	info7.latitude = 48.669350;

	listOfMote.push_back(info);
	listOfMote.push_back(info1);
	listOfMote.push_back(info2);
	listOfMote.push_back(info3);
	listOfMote.push_back(info4);
	listOfMote.push_back(info5);
	listOfMote.push_back(info6);
	listOfMote.push_back(info7);

	labLocAuto->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);


}

int MainPage::getDataFromServer()
{
	return 0;
}


void Projet_STER::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Amphis Nord
	if (!notTake) {
		nomMote = "9.138";
		nomMoteSimple = "Amphi Nord";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}

void Projet_STER::MainPage::OnTick(Object ^ sender, Object ^ e) {
	editTemperature->Text = temprature;
	editHumidite->Text = humidite;
	editBatterie->Text = batterie;
	editLumiere->Text = lumiere;
	editNumMote->Text = nomMote;
	editNomSimpleText->Text = nomMoteSimple;
}

void Projet_STER::MainPage::AfficherInfo(Platform::String^ nomMote)
{

	
}

void Projet_STER::MainPage::trouverMoteProche()
{

	try
	{
		task<GeolocationAccessStatus> geolocationAccessRequestTask(Windows::Devices::Geolocation::Geolocator::RequestAccessAsync());
		geolocationAccessRequestTask.then([this](task<GeolocationAccessStatus> accessStatusTask)
		{
			// Get will throw an exception if the task was canceled or failed with an error
			auto accessStatus = accessStatusTask.get();

			switch (accessStatus)
			{
			case GeolocationAccessStatus::Allowed:
				// You should set MovementThreshold for distance-based tracking
				// or ReportInterval for periodic-based tracking before adding event
				// handlers. If none is set, a ReportInterval of 1 second is used
				// as a default and a position will be returned every 1 second.
				//
				// Value of 2000 milliseconds (2 seconds)
				// isn't a requirement, it is just an example.
				if (geolocator == nullptr)
				{
					geolocator = ref new Geolocator();
					geolocator->ReportInterval = 2000;
				}

				// Subscribe to PositionChanged event to get updated tracking positions
				positionToken = geolocator->PositionChanged::add(ref new TypedEventHandler<Geolocator^, PositionChangedEventArgs^>(this, &Projet_STER::MainPage::OnPositionChanged));

				// Subscribe to StatusChanged event to get updates of location status change
				statusToken = geolocator->StatusChanged::add(ref new TypedEventHandler<Geolocator^, StatusChangedEventArgs^>(this, &Projet_STER::MainPage::OnStatusChanged));
				break;

			case GeolocationAccessStatus::Denied:
				OutputDebugString(L"Localisation desactivée ");
				break;

			case GeolocationAccessStatus::Unspecified:
				OutputDebugString(L"Localisation avec erreur inconnu ");
			}
		});
	}
	catch (task_canceled&)
	{
		OutputDebugString(L"Cancelled");
	}
	catch (Exception^ ex)
	{
		OutputDebugString(L"Exception");
	}
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

/**void Projet_STER::MainPage::UpdateLocationData(Windows::Devices::Geolocation::Geoposition^ position)
{
	if (position != nullptr)
	{

		long double distMin = -1;
		auto latitude = position->Coordinate->Point->Position.Latitude;
		auto longitude = position->Coordinate->Point->Position.Longitude;
		vector<MoteInfoCSV>::iterator it;
		for (it = listOfMote.begin(); it != listOfMote.end(); ++it) {
			MoteInfoCSV mote = *it;
			auto res = distance(latitude, longitude, mote.latitude, mote.longitude);
			if (distMin == -1 || distMin > res) {
				distMin = res;
				nomMote = mote.numMote;	
				nomMoteSimple = mote.nomMote;
				OutputDebugString(L"Changement detecté");
				try {
					if (!notTake) {
						notTake = true;
						Ver.unlock();
					}
				}
				catch (Exception^ e) {
					OutputDebugString(L"Mutex Error");
				}
			}
		}
	}
}*/

void Projet_STER::MainPage::OnPositionChanged(Windows::Devices::Geolocation::Geolocator^ sender, PositionChangedEventArgs^ e)
{
	// We need to dispatch to the UI thread to display the output
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler(
			[this, e]()
	{

		auto coordinate = e->Position->Coordinate;
		long double distMin = -1;
		auto latitude = coordinate->Point->Position.Latitude;
		auto longitude = coordinate->Point->Position.Longitude;
		vector<MoteInfoCSV>::iterator it;
		for (it = listOfMote.begin(); it != listOfMote.end(); ++it) {
			MoteInfoCSV mote = *it;
			auto res = distance(latitude, longitude, mote.latitude, mote.longitude);
			if ((distMin == -1 || distMin > res) && gpsReady) {
				distMin = res;
				nomMote = mote.numMote;
				nomMoteSimple = mote.nomMote;
				OutputDebugString(L"Changement detecté");
				try {
					if (!notTake) {
						notTake = true;
						Ver.unlock();
					}
				}
				catch (Exception^ e) {
					OutputDebugString(L"Mutex Error");
				}
			}
		}
	},
			CallbackContext::Any
		)
	);
}

/*
Source : https://www.geeksforgeeks.org/program-distance-two-points-earth/
*/

long double Projet_STER::MainPage::toRadians(const long double degree)
{
	// cmath library in C++  
	// defines the constant 
	// M_PI as the value of 
	// pi accurate to 1e-30 
	long double one_deg = (M_PI) / 180;
	return (one_deg * degree);
}
/*
Source :https://www.geeksforgeeks.org/program-distance-two-points-earth/
*/
long double Projet_STER::MainPage::distance(long double lat1, long double long1,
	long double lat2, long double long2)
{
	// Convert the latitudes  
	// and longitudes 
	// from degree to radians. 
	lat1 = toRadians(lat1);
	long1 = toRadians(long1);
	lat2 = toRadians(lat2);
	long2 = toRadians(long2);

	// Haversine Formula 
	long double dlong = long2 - long1;
	long double dlat = lat2 - lat1;

	long double ans = pow(sin(dlat / 2), 2) +
		cos(lat1) * cos(lat2) *
		pow(sin(dlong / 2), 2);

	ans = 2 * asin(sqrt(ans));

	// Radius of Earth in  
	// Kilometers, R = 6371 
	// Use R = 3956 for miles 
	long double R = 6371;

	// Calculate the result 
	ans = ans * R;


	return ans;
}





void Projet_STER::MainPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	trouverMoteProche();
	if (originalColorBool) {
		disable_localisation();
	}
	else {
		enable_localisation();
	}
	
}

void Projet_STER::MainPage::enable_localisation() {
	labLocAuto->Background = ref new SolidColorBrush(Windows::UI::Colors::Green);
	originalColorBool = true;

}

void Projet_STER::MainPage::disable_localisation() {
	labLocAuto->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
	if (originalColorBool) {
		geolocator->PositionChanged::remove(positionToken);
		geolocator->StatusChanged::remove(statusToken);
	}
	originalColorBool = false;
}

void Projet_STER::MainPage::OnStatusChanged(Windows::Devices::Geolocation::Geolocator^ sender, StatusChangedEventArgs^ e)
{
	// We need to dispatch to the UI thread to display the output
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler(
			[this, e]()
	{
		switch (e->Status)
		{
		case Windows::Devices::Geolocation::PositionStatus::Ready:
			// Location platform is providing valid data.
			gpsReady = true;
			break;

		case Windows::Devices::Geolocation::PositionStatus::Initializing:
			// Location platform is attempting to acquire a fix.
			gpsReady = false;
			break;

		case Windows::Devices::Geolocation::PositionStatus::NoData:
			// Location platform could not obtain location data.
			gpsReady = false;
			break;

		case Windows::Devices::Geolocation::PositionStatus::Disabled:
			// The permission to access location data is denied by the user or other policies.
			gpsReady = false;
			break;

		case Windows::Devices::Geolocation::PositionStatus::NotInitialized:
			// The location platform is not initialized. This indicates that the application
			// has not made a request for location data.
			gpsReady = false;
			break;

		case Windows::Devices::Geolocation::PositionStatus::NotAvailable:
			// The location platform is not available on this version of the OS.
			gpsReady = false;
			break;

		default:
			gpsReady = false;
			break;
		}
	},
			CallbackContext::Any
		)
	);
}

void Projet_STER::MainPage::Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Amphis Nord
	if (!notTake) {
		nomMote = "111.130";
		nomMoteSimple = "Amphi Sud";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();

}


void Projet_STER::MainPage::Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Salle N0.3 -> 32.131
	if (!notTake) {
		nomMote = "32.131";
		nomMoteSimple = "Salle N0.3";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}


void Projet_STER::MainPage::Button_Click_4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Salle E1.22 -> 32.131
	if (!notTake) {
		nomMote = "151.105";
		nomMoteSimple = "Salle E1.22";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}


void Projet_STER::MainPage::Button_Click_5(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Bureau 2.6 -> 97.145
	if (!notTake) {
		nomMote = "97.145";
		nomMoteSimple = "Bureau 2.6";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}


void Projet_STER::MainPage::Button_Click_6(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Bureau 2.7 -> 120.99
	if (!notTake) {
		nomMote = "120.99";
		nomMoteSimple = "Bureau 2.7";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}


void Projet_STER::MainPage::Button_Click_7(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Bureau 2.8 -> 200.124
	if (!notTake) {
		nomMote = "200.124";
		nomMoteSimple = "Bureau 2.8";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}


void Projet_STER::MainPage::Button_Click_8(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Bureau 2.9 -> 200.124
	if (!notTake) {
		nomMote = "53.105";
		nomMoteSimple = "Bureau 2.9";
		notTake = true;
		Ver.unlock();
	}
	disable_localisation();
}
