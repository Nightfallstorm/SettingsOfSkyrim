#pragma once
#include <filesystem>
#include <iostream>
#include <string>
struct RunINIParseAfterESPINIParse
{
	// Run original function for parsing the INIs attached to plugins, then parse our INIs right afterwards
	static std::uint64_t thunk(std::uint64_t a_unk, std::uint64_t a_unk2)
	{
		auto result = func(a_unk, a_unk2);
		ParseINIs();
		return result;
	};

	static inline std::vector<std::string> GetINIPaths() {
		std::vector<std::string> inis;
		constexpr auto path = L"Data/SKSE/INIs/";
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::wstring wstring(entry.path().native());
			std::string strPath;
			std::transform(wstring.begin(), wstring.end(), std::back_inserter(strPath), [](wchar_t c) {
				return (char)std::tolower((char)c);
			});
			if (strPath.ends_with(".ini")) {
				inis.emplace_back(strPath);
			}
		}
		return inis;
	}
	static inline std::vector<std::string> GetINIsToRead()
	{
		std::vector<std::string> inis;
		for (const auto& ini : GetINIPaths()) {
			if (!ini.ends_with("output.ini")) {
				inis.emplace_back(ini);
			}
			
		}
		std::sort(inis.begin(),	inis.end());
		return inis;
	}

	static inline std::vector<std::string> GetINIsToWrite()
	{
		std::vector<std::string> inis;
		for (const auto& ini : GetINIPaths()) {
			if (ini.ends_with("output.ini")) {
				inis.emplace_back(ini);
			}
		}
		return inis;
	}

	// TODO: CLIB-NG's header doesn't account for the newest RE of INISettingCollection, wait for update
	static inline void ParseINIs()
	{
		auto iniSettingsCollection = RE::INISettingCollection::GetSingleton();
		auto iniPrefSettingsCollection = RE::INIPrefSettingCollection::GetSingleton();
		char originalIniSubKey[0x104];
		strcpy_s(originalIniSubKey, iniSettingsCollection->subKey);
		char originalIniPrefSubKey[0x104];
		strcpy_s(originalIniPrefSubKey, iniPrefSettingsCollection->subKey);

		for (auto& ini : GetINIsToRead()) {
			strcpy_s(iniSettingsCollection->subKey, ini.c_str());
			iniSettingsCollection->OpenHandle(true);
			iniSettingsCollection->ReadAllSettings();
			iniSettingsCollection->CloseHandle();

			strcpy_s(iniPrefSettingsCollection->subKey, ini.c_str());
			iniPrefSettingsCollection->OpenHandle(true);
			iniPrefSettingsCollection->ReadAllSettings();
			iniPrefSettingsCollection->CloseHandle();
		}

		for (auto& ini : GetINIsToWrite()) {
			strcpy_s(iniSettingsCollection->subKey, ini.c_str());
			iniSettingsCollection->OpenHandle(true);
			iniSettingsCollection->WriteAllSettings();
			iniSettingsCollection->CloseHandle();

			strcpy_s(iniPrefSettingsCollection->subKey, ini.c_str());
			iniPrefSettingsCollection->OpenHandle(true);
			iniPrefSettingsCollection->WriteAllSettings();
			iniPrefSettingsCollection->CloseHandle();
		}

		strcpy_s(iniSettingsCollection->subKey, originalIniSubKey);
		strcpy_s(iniPrefSettingsCollection->subKey, originalIniPrefSubKey);
	}

	static inline REL::Relocation<decltype(thunk)> func;

	// Install our hook at the specified address
	static inline void Install()
	{
		// AE hook spot is very, VERY different from SE/VR because AE calls the original hook spot for each single entry instead of just once for all INI entries
		// TODO: Verify AE hook spot is good enough
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(35642, 36544), REL::VariantOffset(0x7C, 0x15, 0x7C) };
		stl::write_thunk_call<RunINIParseAfterESPINIParse>(target.address());

		logger::info("RunINIParseAfterESPINIParse hooked at address {:x}", target.address());
		logger::info("RunINIParseAfterESPINIParse hooked at offset {:x}", target.offset());
	}
};

struct LogAllINIParsing
{
	static void thunk(RE::INISettingCollection* a_self)
	{
		logger::info("Skyrim parsing INI: \"{}\"", a_self->subKey);
		func(a_self);
	};

	static inline REL::Relocation<decltype(thunk)> func;

	static inline std::uint32_t idx = 9;  // ReadAllSettings

	// Install our hook at the specified address
	static inline void Install()
	{
		stl::write_vfunc<RE::INISettingCollection, LogAllINIParsing>();

		logger::info("LogAllINIParsing set");
	}
};

struct LogAllINIWriting
{
	static void thunk(RE::INISettingCollection* a_self)
	{
		logger::info("Skyrim writing to INI file: \"{}\"", a_self->subKey);
		func(a_self);
	};

	static inline REL::Relocation<decltype(thunk)> func;

	static inline std::uint32_t idx = 8;  // WriteAllSettings

	// Install our hook at the specified address
	static inline void Install()
	{
		stl::write_vfunc<RE::INISettingCollection, LogAllINIWriting>();

		logger::info("LogAllINIWriting set");
	}
};

struct LogAllPrefINIParsing
{
	static void thunk(RE::INIPrefSettingCollection* a_self)
	{
		logger::info("Skyrim parsing INI: \"{}\" for preferences", a_self->subKey);
		func(a_self);
	};

	static inline REL::Relocation<decltype(thunk)> func;

	static inline std::uint32_t idx = 9;  // ReadAllSettings

	// Install our hook at the specified address
	static inline void Install()
	{
		stl::write_vfunc<RE::INIPrefSettingCollection, LogAllPrefINIParsing>();

		logger::info("LogAllPrefINIParsing set");
	}
};

struct LogAllPrefINIWriting
{
	static void thunk(RE::INIPrefSettingCollection* a_self)
	{
		logger::info("Skyrim writing to INI file: \"{}\" for preferences", a_self->subKey);
		func(a_self);
	};

	static inline REL::Relocation<decltype(thunk)> func;

	static inline std::uint32_t idx = 8;  // WriteAllSettings

	// Install our hook at the specified address
	static inline void Install()
	{
		stl::write_vfunc<RE::INIPrefSettingCollection, LogAllPrefINIWriting>();

		logger::info("LogAllPrefINIWriting set");
	}
};
