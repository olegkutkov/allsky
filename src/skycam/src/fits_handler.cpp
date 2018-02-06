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

#include <fitsio.h>
#include "fits_handler.hpp"

FitsException::FitsException(int error)
	: errcode (error)
{	
}

const char* FitsException::what() const throw()
{
	fits_get_errstatus(errcode, buf);

	return strdup(buf);
}

FitsHandler::FitsHandler(const std::string &filename, bool creat)
	: fhandle (NULL)
	, imagebuf (NULL)
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
//fits_get_errstatus(status, buf);
}

FitsHandler::~FitsHandler()
{
	
}

bool FitsHandler::LoadData()
{
	
}

bool FitsHandler::SaveData()
{
	
}

bool FitsHandler::ReleaseData()
{
	
}

void FitsHandler::operator-(const FitsHandler& rhs)
{
	
}

