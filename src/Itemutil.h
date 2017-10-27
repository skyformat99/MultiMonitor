
#pragma once
#include <string>
#include <vector>


#define MDSROOT						 "MDSROOT"
#define CONNECTIONOK				 "CONNECTION_OK"
#define CONNECTIONTIMEOUT			 "CONNECTION_TIME_OUT"

enum ItemColour
{
	WHITE	= 0,
	BLACK	= 1,
	YELLOW	= 2,
	RED		= 3,
	BLUE	= 4,
	GREEN	= 5,
	NO_USE 
};

typedef struct XYCoordinate
{
	int x;
	int y;
	ItemColour colour;
	XYCoordinate(int ix, int iy)
	{
		x = ix;
		y = iy;
	}

	XYCoordinate(int ix, int iy,ItemColour c)
	{
		x = ix;
		y = iy;
		colour = c;
	}
	XYCoordinate()
	{
		x = 0;
		y = 0;
	}

	bool operator == (const XYCoordinate& c)
	{
		return c.x == x && c.y == y && c.colour == colour;

	}

	const  XYCoordinate& operator =(const XYCoordinate& c)
	{
		x = c.x;
		y = c.y;
		colour = c.colour;
		return *this;
	}


	bool operator < (const XYCoordinate &c) const
	{
        if(x<c.x)
			return true;
		else if(x == c.x && y < c.y)
			return true;
		else
			return false;
	}


}XYCoordinate;



inline int GetSeconds(int iTime)
{
    return iTime / 10000 % 100 * 3600 + iTime / 100 % 100 * 60 + iTime % 100;
}
