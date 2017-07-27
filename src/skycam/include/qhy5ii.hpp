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

#ifndef QHY5II_HPP
#define QHY5II_HPP

#include <libusb-1.0/libusb.h>
#include "qhybase.hpp"

#define QHY5II_VENDOR_ID   0x1618
#define QHY5II_PRODUCT_ID  0x0921

class Qhy5II : public QhyBase
{
public:
	Qhy5II();
	~Qhy5II();

	int OpenDevice();
	void CloseDevice();

	int InitDevice();
	void UninitDevice();

	bool IsColor();

	int SetParam(const DEVICE_PARAMS param, const param_val_t &value);
	int GetParam(const DEVICE_PARAMS param, param_val_t &value);

	void StartVideo();
	void StopVideo();

	size_t GetImageMemorySize();
	int GetFrame(unsigned char *data, unsigned int data_size);


///

	static int CtrlMsg(struct libusb_device_handle *handle
						, unsigned char request_type
						, unsigned char request
						, unsigned int value
						, unsigned int index
						, unsigned char *data
						, unsigned char len);


	static int EepromRead(struct libusb_device_handle *handle
							, unsigned char addr
							, unsigned char* data
							, unsigned short len);

	static int GetDeviceType(libusb_device_handle *handle, bool &is_color);
	static void DumpDeviceInfo(libusb_device *tdev);

private:
	unsigned char MSB(unsigned short i);
	unsigned char LSB(unsigned short i);
	void I2CTwoWrite(uint16_t addr, uint16_t value);
	uint16_t I2CTwoRead( uint16_t addr);

	void SetQHY5LIIHDR(bool on);

	void SetResolution(const int w, const int h);
	void SetSpeed(const bool high_speed);
	void SetUsbTraffic(const int i);
	void SetTransferBit(const int bit);
	int SetExposureTime(const unsigned int exptime);
	int SetGain(const unsigned short new_gain);
	int SetGainCore(const unsigned short new_gain);
	void CorrectQHY5LIIWH(const int w, const int h);

	void Set14Bit(unsigned char i);
	void InitCmos();

	void SetSpeedQHY5LII(const unsigned char i);
	void SetQHY5LIIGain(const unsigned short set_gain);
	double SetQHY5LREG_PLL(unsigned char clk);
	void SetGainMonoQHY5LII(const double set_gain);
	void SetGainColorQHY5LII(const double set_gain, const double RG, const double BG);
	void SetExposureTime_QHY5LII(const unsigned int val);

	void InitQHY5LII_XGA();
	void InitQHY5LII_1280X960();
	void InitQHY5LII_SVGA();
	void InitQHY5LII_VGA();
	void InitQHY5LII_QVGA();

/// QHY5II
	void SetExposureTime_QHY5II( uint32_t i);
	void QHY5IISetResolution(int x, int y);
	void CorrectQHY5IIWH(int w, int h);
	void initQHY5II_SXGA();
	void initQHY5II_XGA();
	void initQHY5II_SVGA();
	void initQHY5II_VGA();
	void SetQHY5IIGain(unsigned short gain);
///

	struct libusb_device_handle *handle;
	bool is_color;
	int dev_type;
	int usb_traf;
	int usb_speed;
	int transfer_bit;
	unsigned int exp_time;
	unsigned short gain;
	int width;
	int height;
	int wbblue;
	int wbred;
	double pll;
	bool long_exp_mode;
};

#endif

