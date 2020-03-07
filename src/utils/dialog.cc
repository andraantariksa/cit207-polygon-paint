#include <ctime>
#include <sstream>

#include <tinyfiledialogs.h>


#include "dialog.h"

namespace utils
{

	char const* Dialog::save()
	{
		char const* filter[1] = { "*.svg" };
		return tinyfd_saveFileDialog(
			"Save file",
			Dialog::defaultFilename().c_str(),
			1, filter,
			"SVG"
		);
	}

	std::string Dialog::defaultFilename()
	{
		std::time_t now = std::time(nullptr);
		std::tm* time_s = std::localtime(&now);

		std::stringstream name;
		name << time_s->tm_year + 1900;
		name << "-";
		name << time_s->tm_mon + 1;
		name << "-";
		name << time_s->tm_mday;
		name << "-";
		name << time_s->tm_hour + 1;
		name << "-";
		name << time_s->tm_min + 1;
		name << "-";
		name << time_s->tm_sec + 1;
		name << ".svg";

		return name.str();
	}

}