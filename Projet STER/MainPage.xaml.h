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
		int getDataFromServer();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTick(Object ^ sender, Object ^ e);
		void AfficherInfo(Platform::String^ nomMote);
		Platform::String^ recupererDonneesMote(Windows::Data::Json::JsonObject^ objJson, Platform::String^ label);
		Platform::String ^ parseCSV(std::string nomMote, Platform::String ^ elmenet);
		std::vector<std::string> split(std::string str, std::string sep);
		bool isFilterUsed;
		Windows::Web::Http::Filters::HttpBaseProtocolFilter^ filter;
		Windows::Web::Http::HttpClient^ httpClient;
		concurrency::cancellation_token_source cancellationTokenSource;
	};
}
