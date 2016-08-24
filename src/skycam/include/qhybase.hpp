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

#ifndef QHYBASE_HPP
#define QHYBASE_HPP

#include "qhydefs.hpp"
#include "qhyerrs.hpp"

class QhyBase
{
public:
	QhyBase() {};
	virtual ~QhyBase() {};

	virtual int OpenDevice() =0;
	virtual void CloseDevice() =0;

	virtual int InitDevice() =0;

	virtual size_t GetImageMemorySize() =0;

	virtual bool IsColor() =0;

	virtual int SetParam(const DEVICE_PARAMS param, const param_val_t &value) =0;
	virtual int GetParam(const DEVICE_PARAMS param, param_val_t &value) =0;

	virtual void StartVideo() =0;
	virtual void StopVideo() =0;

	virtual int GetFrame(unsigned char *data, unsigned int data_size) =0;
};

#endif

