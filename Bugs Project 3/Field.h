#ifndef FIELD_H_
#define FIELD_H_

#include "GameConstants.h"

#include <iostream>
#include <fstream>
#include <string>

class Field
{
public:

	enum FieldItem {
		empty, anthill0, anthill1, anthill2, anthill3, food, grasshopper, water, rock, poison
	};
	enum LoadResult {
		load_success, load_fail_file_not_found, load_fail_bad_format
	};

	  // This overload exists for backward compatibility with the original
	  // skelton code.
	LoadResult loadField(std::string filename)
	{
		std::string error;
		return loadField(filename, error);
	}
	
	LoadResult loadField(std::string, std::string&)
	{
		return load_success;
	}

	  // Field has a grasshopper at x=5,y=10 and an anthill0 at x=58,y=53
	FieldItem getContentsOf(int x, int y)
	{
		if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)
			return empty;
		else if (x == 0  ||  x == VIEW_WIDTH-1  ||  y == 0  ||  y == VIEW_HEIGHT-1)
			return rock;
		else if (x == 10  &&  y == 5)
			return grasshopper;
		else if (x == VIEW_WIDTH-1-10  &&  y == VIEW_HEIGHT-1-5)
			return anthill0;

		return empty;
	}
};

#endif // FIELD_H_
