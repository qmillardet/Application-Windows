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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409





MainPage::MainPage()
{
	InitializeComponent();
	MainPage::getDataFromServer();
}

int MainPage::getDataFromServer()
{
	StringRest->Text = L"Aucune données";
	return 0;
}


void Projet_STER::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Foundation::Uri^ uri = ref new Uri("http://iotlab.telecomnancy.eu:8080/iotlab/rest/data/1/temperature-light2-light1-battery_indicator-humidity/last");
	Windows::Web::Http::HttpClient^ httpClient = ref new HttpClient();
	//auto httpResponse = httpClient->GetStringAsync(uri);
	//StringRest->Text = httpResponse->ToString();
	create_task(httpClient->GetStringAsync(uri))
		.then([=](Platform::String^ Tet)
	{
		StringRest->Text = Tet;
		return task_from_result();
	}).then([](void) {
		OutputDebugString(L"File deleted.");
	});
	
}
