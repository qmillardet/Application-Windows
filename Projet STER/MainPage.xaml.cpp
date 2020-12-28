//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <iostream>


using namespace Projet_STER;

using namespace concurrency;
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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
	MainPage::getDataFromServer();
}

int MainPage::getDataFromServer()
{
	return 0;
}


void Projet_STER::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Amphis Nord
	AfficherInfo("9.138");
}

void Projet_STER::MainPage::AfficherInfo(Platform::String^ nomMote)
{

	auto url = String::Concat("http://iotlab.telecomnancy.eu:8080/iotlab/rest/data/1/temperature-light2-light1-battery_indicator-humidity/1/" , nomMote);
	Windows::Foundation::Uri^ uri = ref new Uri(url);
	Windows::Web::Http::HttpClient^ httpClient = ref new HttpClient();
	create_task(httpClient->GetStringAsync(uri))
		.then([=](Platform::String^ Tet)
	{
		auto jsonParse = JsonObject::Parse(Tet);
		Platform::String^ label = "temperature";
		Platform::String^ temprature = recupererDonneesMote(jsonParse, label);
		label = "humidity";
		Platform::String^ humidite = recupererDonneesMote(jsonParse, label);
		label = "light1";
		Platform::String^ lumiere = recupererDonneesMote(jsonParse, label);
		label = "battery_indicator";
		Platform::String^ batterie = recupererDonneesMote(jsonParse, label);
		editTemperature->Text = temprature + "°C";
		editHumidite->Text = humidite;
		editBatterie->Text = batterie;
		editLumiere->Text = lumiere;
		
		return task_from_result();
	}).then([](void) {
		OutputDebugString(L"Données récupérées.");
	});
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
