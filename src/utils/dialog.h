#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <string>

namespace utils
{

	class Dialog
	{
	public:
		static char const* save();
		static char const* load();
		static std::string defaultFilename();
	};

}

#endif //_DIALOG_H_
