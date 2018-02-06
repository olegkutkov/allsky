/*
 QHY camera tool

 Copyright 2016  Oleg Kutkov <kutkov.o@yandex.ru>
 Based on original QHY code from https://github.com/qhyccd-lzr

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#ifndef FITS_HANDLER_HPP
#define FITS_HANDLER_HPP

#include <string>
#include <fitsio.h>
#include <exception>

class FitsException : public std::exception
{
public:
	FitsException(int error);

	virtual const char* what() const throw();

private:
	int errcode;

};

class ImageBuf
{
public:
	ImageBuf(size_t size)
		: buf(NULL)
	{
		buf = new long[size];
	}

	~ImageBuf()
	{
		delete[] buf;
	}

	long* Raw() { return buf; };

private:
	long* buf;
};

class FitsHandler
{
public:
	FitsHandler(const std::string &filename, bool creat = true);
	~FitsHandler();

	void SetImageWH(const int width, const int height);

	bool LoadImageData();
	bool SetImegeData();
	bool SaveImageData();
	bool ReleaseImageData();

	void operator-(const FitsHandler& rhs);

private:
	fitsfile *fhandle;
	long *imagebuf;
	int imgwidth;
	int imgheight;
	std::string fname;
};

#endif 

