#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>
#include <stdlib.h>
//
// g++ main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -I .
//

// void PixelGameEngine::UpdateTextEntry()
// {
// 	// Check for typed characters
// for (const auto& key : vKeyboardMap)
// 		if (GetKey(std::get<0>(key)).bPressed)
// 		{
// 			sTextEntryString.insert(nTextEntryCursor, GetKey(olc::Key::SHIFT).bHeld ? std::get<2>(key) : std::get<1>(key));
// 			nTextEntryCursor++;
// 		}

// 	// Check for command characters
// 	if (GetKey(olc::Key::LEFT).bPressed)
// 		nTextEntryCursor = std::max(0, nTextEntryCursor - 1);
// 	if (GetKey(olc::Key::RIGHT).bPressed)
// 		nTextEntryCursor = std::min(int32_t(sTextEntryString.size()), nTextEntryCursor + 1);
// 	if (GetKey(olc::Key::BACK).bPressed && nTextEntryCursor > 0)
// 	{
// 		sTextEntryString.erase(nTextEntryCursor-1, 1);
// 		nTextEntryCursor = std::max(0, nTextEntryCursor - 1);
// 	}
// 	if (GetKey(olc::Key::DEL).bPressed && size_t(nTextEntryCursor) < sTextEntryString.size())
// 		sTextEntryString.erase(nTextEntryCursor, 1);

// 	if (GetKey(olc::Key::UP).bPressed)
// 	{
// 		if (!sCommandHistory.empty())
// 		{
// 			if (sCommandHistoryIt != sCommandHistory.begin())
// 				sCommandHistoryIt--;

// 			nTextEntryCursor = int32_t(sCommandHistoryIt->size());
// 			sTextEntryString = *sCommandHistoryIt;
// 		}
// 	}

// 	if (GetKey(olc::Key::DOWN).bPressed)
// 	{
// 		if (!sCommandHistory.empty())
// 		{
// 			if (sCommandHistoryIt != sCommandHistory.end())
// 			{
// 				sCommandHistoryIt++;
// 				if (sCommandHistoryIt != sCommandHistory.end())
// 				{
// 					nTextEntryCursor = int32_t(sCommandHistoryIt->size());
// 					sTextEntryString = *sCommandHistoryIt;
// 				}
// 				else
// 				{
// 					nTextEntryCursor = 0;
// 					sTextEntryString = "";
// 				}
// 			}
// 		}
// 	}

// 	if (GetKey(olc::Key::ENTER).bPressed)
// 	{
// 		if (bConsoleShow)
// 		{
// 			std::cout << ">" + sTextEntryString + "\n";
// 			if (OnConsoleCommand(sTextEntryString))
// 			{
// 				sCommandHistory.push_back(sTextEntryString);
// 				sCommandHistoryIt = sCommandHistory.end();
// 			}
// 			sTextEntryString.clear();
// 			nTextEntryCursor = 0;
// 		}
// 		else
// 		{
// 			OnTextEntryComplete(sTextEntryString);
// 			TextEntryEnable(false);
// 		}
// 	}
// }

std::string mystring;

void funcccc(const std::vector<std::tuple<olc::Key, std::string, std::string>> *vector,
			 const int32_t key,
			 const std::chrono::_V2::system_clock::time_point time,
			 olc::PixelGameEngine *pge)
{
	for (auto x : *vector)
	{
		if (std::get<0>(x) == key)
		{
			std::cout << "key equals : " << key << std::endl;
			if (pge->GetKey(olc::BACK).bPressed)
			{
				std::cout << "erasing char \n";
				mystring.erase(mystring.size() - 1, 1);
			}
			else
				mystring.append(std::get<1>(x));
		}
		else
			continue;
	}
}

// void vecTimeChecker(std::vector<std::pair<std::chrono::_V2::system_clock::time_point, int32_t>> *vec)
// {
// 	static int32_t x = vec->back().second;
// 	switch (x)
// 	{
// 	// case olc::Key::A:
// 	}
// }

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

	std::string oldTextEntryString;
	std::string mytextString;
	char *delayy;

	bool needToEnableTentry = false;
	std::vector<std::pair<std::chrono::_V2::system_clock::time_point, int32_t>> timeCharInpQueue;

public:
	// void OnTextEntryComplete(const std::string &sText)
	// {
	// std::cout << sText << std::endl;
	// TextEntryEnable(true, "");
	// }
	bool OnUserCreate() override
	{
		TextEntryEnable(true, "");
		oldTextEntryString = TextEntryGetString();
		// Called once at the start, so create things here
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{

		Clear(olc::WHITE);

		const auto &c = GetKeyPressCache();

		if (c.size() > 0)
		{
			for (auto x : c)
			{
				funcccc(GetKeyboardMap(), x, std::chrono::high_resolution_clock::now(), this);
			}
		}
		
		DrawString({10, 10}, TextEntryGetString(), olc::BLACK, 3);

		DrawString({10, 37}, mystring, olc::BLACK, 3);

		for (const auto a : c)
			std::cout << (a) << "\n";

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(2400, 1920, 4, 4))
		demo.Start();
	return 0;
}
