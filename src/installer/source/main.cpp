#include "installer.h"
#include "locations.h"
#include <plugin_defs.h>
#include <filesystem>
#include <fstream>

namespace Uberton {
namespace Installer {

enum class ExceptionType {
	PathNotFound,
	DirectoryCouldNotBeCreated,
	PathNotAccessible
};

class InstallerException : public std::exception
{
public:
	InstallerException(ExceptionType type, const std::string& msg) : std::exception(msg.c_str()), type(type) {
	}

	ExceptionType type;
};



void writeDirToDriveRecursively(const cmrc::embedded_filesystem& fs, const std::string& dir, const std::string& destination) {
	for (const auto& f : fs.iterate_directory(dir)) {
		auto filename = f.filename();
		if (f.is_file()) {
			auto embeddedPath = dir + filename;
			auto destinationDir = destination + dir;
			auto destinationPath = destinationDir + filename;

			auto file = fs.open(embeddedPath);

			std::ofstream outfile{ destinationPath, std::ios::binary };
			if (outfile) {
				std::copy(file.begin(), file.end(), std::ostreambuf_iterator<char>(outfile));
			}
			else {
				throw InstallerException(ExceptionType::PathNotAccessible, "File " + destinationPath + "could not be opened");
			}
		}
		else if (f.is_directory()) {
			auto subDir = dir + filename + "/";
			auto subPath = destination + subDir;
			if (!std::filesystem::exists(subPath)) {
				std::filesystem::create_directories(subPath);
			}
			writeDirToDriveRecursively(fs, subDir, destination);
		}
	}
}

void writeFilesystemToDrive(const cmrc::embedded_filesystem& fs, const std::string& destination) {
	writeDirToDriveRecursively(fs, "", destination);
}

void installVST(std::string& result) {
	result += "\nCopying VST3 Plugin ...";
	auto fs = vst_plugin::get_filesystem();
	writeFilesystemToDrive(fs, getVSTLocation() + "Uberton/");
}

void installPresets(std::string& result) {
#if (UBERTON_PLUGIN_HAS_FACTORY_PRESETS)
	result += "\nCopying Factory Presets ...";
	auto fs = factory_presets::get_filesystem();
	writeFilesystemToDrive(fs, getFactoryPresetLocation() + UBERTON_PLUGIN_NAME + "/");
#endif
}

void installUserGuide(std::string& result) {
#if (UBERTON_PLUGIN_HAS_USERGUIDE_PDF)
	result += "\nCopying User Guide ...";
	auto fs = userguide_pdf::get_filesystem();
	const std::string location = getUserguideLocation() + UBERTON_PLUGIN_NAME + "/";
	writeFilesystemToDrive(fs, location);
	std::ofstream file(location + "version.txt");
	file << UBERTON_PLUGIN_VERSION;
#endif
}

//struct InstallationState
//{
//	bool pdf;
//	bool vst;
//	std::string version;
//
//};
//void checkInstallation() {
//	const std::string vstLoc = getVSTLocation() + "Uberton/" + UBERTON_PLUGIN_NAME + ".vst3";
//	const std::string programLoc = getUserguideLocation() + UBERTON_PLUGIN_NAME + "/";
//	InstallationState state;
//	
//	if (std::filesystem::exists(programLoc)) {
//		for (const auto& entry : std::filesystem::directory_iterator(programLoc)) {
//
//		}
//		std::ifstream file(programLoc + "version.txt");
//		if (file) {
//			std::string version;
//			file >> version;
//			if (!version.empty()) {
//				state.version = version;
//			}
//		}
//	}
//}

void doInstall(std::string& result) {
	result += "\nInstallation started";

	std::string errorString;

	try {
		installVST(result);
		installPresets(result);
		//installUserGuide(result); // user guide is now in plugin bundle
		result += "\n\nInstallation finished sucessfully.";
	}
	catch (const InstallerException& e) {
		errorString += "\nError " + std::to_string(static_cast<int>(e.type)) + std::string(": ") + e.what();
	}
	catch (const std::exception& e) {
		errorString += std::string("\nError:") + e.what();
	}
	catch (...) {
		errorString += "\nUnknown Error";
	}

	if (!errorString.empty()) {
		result += "\nEncountered Errors during Installation:";
		result += errorString;
		result += "\n\nThe Installation could not be finished properly. Please write to mail@uberton.org for support. ";
	}
}



static Init gAppDelegate(std::make_unique<Installer>(UBERTON_PLUGIN_NAME " Installer (" UBERTON_PLUGIN_VERSION ")", UBERTON_PLUGIN_VERSION, [&](std::string& s) { doInstall(s); }));


}
}
