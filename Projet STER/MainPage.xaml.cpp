//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <thread>
//#include <mutex>
#include <shared_mutex>


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
