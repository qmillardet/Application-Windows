//
// MainPage.xaml.h
// Déclaration de la classe MainPage.
//

#pragma once

#include "MainPage.g.h"

namespace Projet_STER
{
	/// <summary>
	/// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTick(Object ^ sender, Object ^ e);
		void trouverMoteProche();
		Platform::String^ recupererDonneesMote(Windows::Data::Json::JsonObject^ objJson, Platform::String^ label);
		Platform::String ^ parseCSV(std::string nomMote, Platform::String ^ elmenet);
		std::vector<std::string> split(std::string str, std::string sep);
		void UpdateLocationData(Windows::Devices::Geolocation::Geoposition ^ position);
		long double toRadians(const long double degree);
		long double distance(long double lat1, long double long1, long double lat2, long double long2);
		void WinLog(const wchar_t * text);
		bool isFilterUsed;
		Windows::Web::Http::Filters::HttpBaseProtocolFilter^ filter;
		Windows::Web::Http::HttpClient^ httpClient;
		concurrency::cancellation_token_source cancellationTokenSource;
		concurrency::cancellation_token_source geopositionTaskTokenSource;
		void Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void enable_localisation();
		void disable_localisation();
		void OnPositionChanged(Windows::Devices::Geolocation::Geolocator^ sender, Windows::Devices::Geolocation::PositionChangedEventArgs^ e);
		void OnStatusChanged(Windows::Devices::Geolocation::Geolocator^ sender, Windows::Devices::Geolocation::StatusChangedEventArgs^ e);
		Windows::Devices::Geolocation::Geolocator^ geolocator = nullptr;
		Windows::Foundation::EventRegistrationToken positionToken;
		Windows::Foundation::EventRegistrationToken statusToken;
		void Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_5(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_6(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_7(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_8(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}

static UINT Inc();
