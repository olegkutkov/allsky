/*
 QHY camera tool

 Copyright 2018  Oleg Kutkov <kutkov.o@yandex.ru>

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
#include <stdint.h>
#include <fitsio.h>
#include <errno.h>
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
		, data_size(size)
	{
		buf = new uint8_t[data_size];

		if (!buf) {
			throw FitsException(errno);
		}
	}

	ImageBuf(size_t size, uint8_t *src)
		: buf(NULL)
		, data_size(size)
	{
		buf = new uint8_t[data_size];

		if (!buf) {
			throw FitsException(errno);
		}

		memcpy(buf, src, data_size);
	}

	~ImageBuf()
	{
		if (buf) {
			delete[] buf;
			buf = NULL;
		}
	}

	uint8_t *Raw() { return buf; };

private:
	uint8_t* buf;
	size_t data_size;
};

class FitsHandler
{
public:
	FitsHandler(const std::string &filename, bool creat = true);
	~FitsHandler();

	void SetImageWH(const int width, const int height);

	bool LoadImageData();
	bool CreateNewImage(int bitpixel);
	bool SetImegeData(size_t data_size, uint8_t *data);
	bool SaveImageData();
	bool ReleaseImageData();

	bool SetHeader();

	void Substract(const FitsHandler& rhs);

private:
	fitsfile *fhandle;
	ImageBuf *imagebuf;
	int imgwidth;
	int imgheight;
	int bitpix;
	std::string fname;
};

#endif 

