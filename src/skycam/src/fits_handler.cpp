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

#include <string.h>
#include <time.h>
#include "fits_handler.hpp"

FitsException::FitsException(int error)
	: errcode (error)
{	
}

const char* FitsException::what() const throw()
{
	char buf_str[32];

	fits_get_errstatus(errcode, buf_str);

	const char* ret = strdup(buf_str);

	return ret;
}

FitsHandler::FitsHandler(const std::string &filename, bool creat)
	: fhandle (NULL)
	, imagebuf (NULL)
	, bitpix (0)
	, fname (filename)
{
	int status = 0;

	if (creat) {
		fits_create_file(&fhandle, fname.c_str(), &status);
	} else {
		fits_open_file(&fhandle, fname.c_str(), READONLY, &status);
	}

	if (status != 0) {
		throw FitsException(status);
	}
}

FitsHandler::~FitsHandler()
{
	int status = 0;

	if (fhandle) {
		fits_close_file(fhandle, &status);
	}

	ReleaseImageData();
}

void FitsHandler::SetImageWH(const int width, const int height)
{
	imgwidth = width;
	imgheight = height;
}

bool FitsHandler::LoadImageData()
{
	ReleaseImageData();

	int status = 0;
	long anaxes[2] = { 1, 1 };

	fits_get_img_size(fhandle, 2, anaxes, &status);

	imgwidth = anaxes[0];
	imgheight = anaxes[1];

	fits_get_img_type(fhandle, &bitpix, &status);

	if (status != 0) {
		return false;
	}

	long firstpix[2] = { 1, 1 };
	size_t numpix = imgwidth * imgheight;

	imagebuf = new ImageBuf(numpix);

	if (!imagebuf) {
		return false;
	}

	fits_read_pix(fhandle, TBYTE, firstpix,
		numpix, NULL, imagebuf->Raw(), NULL, &status);

	return true;
}

bool FitsHandler::CreateNewImage(int bitpixel)
{
	unsigned int naxis = 2;
	long naxes[2] = { imgwidth, imgheight };
	int status = 0;

	bitpix = bitpixel;

	fits_create_img(fhandle, bitpixel, naxis, naxes, &status);

	return status == 0;
}

bool FitsHandler::SetImegeData(size_t data_size, uint8_t *data)
{
	ReleaseImageData();

	imagebuf = new ImageBuf(data_size, data);

	if (!imagebuf) {
		return false;
	}

	return true;
}

bool FitsHandler::SaveImageData()
{
	if (!imagebuf) {
		return false;
	}

	int status = 0;

	long fpx[2] = { 1L, 1L };

	fits_write_pix(fhandle, TBYTE, fpx, imgwidth * imgheight, imagebuf->Raw(), &status);	

	return status == 0;
}

void get_current_datetime(char *dst)
{
	time_t lt = time(NULL);
	struct tm *utc_tm = gmtime(&lt);

	strftime(dst, 25, "%Y-%m-%dT%H:%M:%S", utc_tm);
}

bool FitsHandler::SetHeader(std::vector<fits_header_data_t>& data)
{
	int status = 0;

	char time_now[25];
	get_current_datetime(time_now);

	fits_write_key(fhandle, TSTRING, "DATE", time_now, "Fits creation date, UTC", &status);

	std::vector<fits_header_data_t>::iterator dat_ir = data.begin();

	for ( ; dat_ir != data.end(); dat_ir++) {
		fits_write_key(fhandle, TSTRING, dat_ir->key, dat_ir->val, "", &status);
	}

	return status == 0;
}

bool FitsHandler::ReleaseImageData()
{
	if (imagebuf) {
		delete imagebuf;
		imagebuf = NULL;
	}

	return true;
}

void FitsHandler::Substract(const FitsHandler& rhs)
{
	for (long i = 0; i < imgwidth * imgheight; ++i) {
		imagebuf->Raw()[i] -= rhs.imagebuf->Raw()[i];
	}
}

