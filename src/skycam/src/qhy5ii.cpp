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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "qhy5ii.hpp"
#include "logger.h"

// QHY5-II stuff
int gain_lut[73] =
{
		0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A, 0x00B, 0x00C, 0x00D, 0x00E,
		0x00F, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017, 0x018, 0x019,
		0x01A, 0x01B, 0x01C, 0x01D, 0x01E, 0x01F, 0x051, 0x052, 0x053, 0x054, 0x055,
		0x056, 0x057, 0x058, 0x059, 0x05A, 0x05B, 0x05C, 0x05D, 0x05E, 0x05F, 0x6CE,
		0x6CF, 0x6D0, 0x6D1, 0x6D2, 0x6D3, 0x6D4, 0x6D5, 0x6D6, 0x6D7, 0x6D8, 0x6D9,
		0x6DA, 0x6DB, 0x6DC, 0x6DD, 0x6DE, 0x6DF, 0x6E0, 0x6E1, 0x6E2, 0x6E3, 0x6E4,
		0x6E5, 0x6E6, 0x6E7, 0x6FC, 0x6FD, 0x6FE, 0x6FF
};
// --------------


Qhy5II::Qhy5II()
	: handle (NULL)
	, is_color (false)
	, dev_type (DEVICETYPE_UNKOWN)
	, usb_traf (0)
	, usb_speed (0)
	, transfer_bit (8)
	, exp_time (100)
	, gain (0)
	, width (1280)
	, height (960)
	, wbblue (145)
	, wbred (115)
	, pll (0)
	, long_exp_mode (false)
{
#ifdef ANNIYING_DEBUG
	log_debug("Qhy5II::Qhy5II()");
#endif
}

Qhy5II::~Qhy5II()
{
	if (handle) {
		CloseDevice();
		log_debug("Qhy5II::Qhy5II() - cleaned up");
	}
}

int Qhy5II::OpenDevice()
{

#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::OpenDevice()");
#endif
	if (handle) {
		log_warning("Device is already opened!");
		return QHYCCD_SUCCESS;
	}

	log_debug("Device vendor id=0x%X product id=0x%X", QHY5II_VENDOR_ID, QHY5II_PRODUCT_ID);

	handle = libusb_open_device_with_vid_pid(NULL, QHY5II_VENDOR_ID, QHY5II_PRODUCT_ID);

	if (!handle) {
		return QHYCCD_ERROR_NO_DEVICE;
	}

	if( libusb_kernel_driver_active(handle, 0)) {
        libusb_detach_kernel_driver(handle, 0);
	}

    int open_status = libusb_set_configuration(handle, 1);
    (void)open_status;

    if (libusb_claim_interface(handle, 0) != 0) {
		return QHYCCD_ERROR_INITRESOURCE;
	}

	dev_type = GetDeviceType(handle, is_color);

	usb_traf = 32;
	usb_speed = 0;

	if (dev_type == DEVICETYPE_QHY5LII || dev_type == DEVICETYPE_QHY5RII)
	{
		unsigned char buf[4] = { 0, 0, 0, 0 };
		CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0, 0, buf, 4 );

		log_status("Succesfully connected to QHY5LII/QHY5RII device");

		#ifdef __arm__
			usb_traf = 40;
		#else
			usb_traf = 5;
		#endif
	}
	else if (dev_type == DEVICETYPE_QHY5II)
	{
		log_status("Succesfully connected to QHY5II device");

		#ifdef __arm__
			usb_traf = 150;
		#else
			usb_traf = 30;
		#endif

		is_color = false;
		transfer_bit = 8;
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::OpenDevice()");
#endif
	return QHYCCD_SUCCESS;
}

void Qhy5II::CloseDevice()
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::CloseDevice()");
#endif
	if (handle) {
		InitCmos();

		libusb_release_interface(handle, 0);
		libusb_close(handle);
		handle = NULL;
		log_status("Device closed");
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::CloseDevice()");
#endif
}

int Qhy5II::InitDevice()
{
	log_status("Initializing device with default params");

	usb_traf = usb_traf < 1 ? 1 : usb_traf;
	usb_traf = usb_traf > 255 ? 255 : usb_traf;
	usb_speed = usb_speed < 0 ? 0 : usb_speed;
	usb_speed = usb_speed > 1 ? 1 : usb_speed;

	SetTransferBit(8);
	SetSpeed((usb_speed != 0));

	SetQHY5LIIHDR(false);

	SetResolution(width, height);
	SetGain(gain);

	SetUsbTraffic(usb_traf);

	StopVideo();

	SetExposureTime(exp_time);

	SetGain(gain);
	SetUsbTraffic(usb_traf);

	StartVideo();

	SetExposureTime(exp_time);

	log_status("Device was initialized and ready to work");

	return QHYCCD_SUCCESS;
}

void Qhy5II::UninitDevice()
{
	StopVideo();
}

bool Qhy5II::IsColor()
{
	return is_color;
}

int Qhy5II::SetParam(const DEVICE_PARAMS param, const param_val_t &value)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetParam()");
#endif
	log_debug("Param = %i  values = %d:%d", param, value.values[0], value.values[1]);

	switch (param) {
		case CONTROL_RESOLUTION:
			SetResolution(value.values[0], value.values[1]);
			break;

		case CONTROL_GAIN:
			return SetGain(value.values[0]);

		case CONTROL_EXPOSURE:
			return SetExposureTime(value.values[0]);

		case CONTROL_WBR:
			wbred = value.values[0];
			return SetGain(gain);

		case CONTROL_WBB:
			wbblue = value.values[0];
			return SetGain(gain);

		case CONTROL_SPEED:
			usb_speed = value.values[0];
			SetSpeed(usb_speed);
			break;

		case CONTROL_USBTRAFFIC:
			usb_traf = value.values[0];
			SetUsbTraffic(usb_traf);
			break;

		case CONTROL_TRANSFERBIT:
			if (value.values[0] == 8 || value.values[0] == 12) {
				transfer_bit = value.values[0];
			} else {
				transfer_bit = 8;
			}

			SetTransferBit(transfer_bit);
			break;

		default:
			return QHYCCD_ERROR_NOTSUPPORT;
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetParam()");
#endif
	return QHYCCD_SUCCESS;
}

int Qhy5II::GetParam(const DEVICE_PARAMS param, param_val_t &value)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::GetParam()");
#endif
	log_debug("Param = %i", param);

	switch (param) {
		case CONTROL_RESOLUTION:
			value.set(width, height);
			break;

		case CONTROL_GAIN:
			value.set(gain, 0);
			break;

		case CONTROL_EXPOSURE:
			value.set(exp_time, 0);
			break;

		case CONTROL_WBR:
			value.set(wbred, 0);
			break;

		case CONTROL_WBB:
			value.set(wbblue, 0);
			break;

		case CONTROL_SPEED:
			value.set(usb_speed, 0);
			break;

		case CONTROL_USBTRAFFIC:
			value.set(usb_traf, 0);
			break;

		case CONTROL_TRANSFERBIT:
			value.set(transfer_bit, 0);
			break;

		default:
			return QHYCCD_ERROR_NOTSUPPORT;
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::GetParam()");
#endif

	return 0;
}

void Qhy5II::StartVideo()
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::StartVideo()");
#endif
	unsigned char buf[1] = { 100 };
	CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xb3, 0, 0, buf, 1);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::StartVideo()");
#endif
}

void Qhy5II::StopVideo()
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::StopVideo()");
#endif
	unsigned char buf[4] = { 0, 0, 0, 0 };
	CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0, 0, buf, 4 );
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::StopVideo()");
#endif
}

size_t Qhy5II::GetImageMemorySize()
{
	return width * height * (transfer_bit >> 3);
}

int Qhy5II::GetFrame(unsigned char *data, unsigned int data_size) 
{
	if (!handle) {
		return QHYCCD_ERROR_EXPFAILED;
	}

	data_size = 1280 * 960;

	log_status("Camera is in %i bit mode\n", transfer_bit);
	log_status("Capturing %ix%i frame with gain = %i and exposure = %i\n", width, height, gain, exp_time);

	int transfered = 0;
	int try_cnt = 0;
	int pos = 0;
	int to_read = data_size + 5;

	while( to_read )
	{
		int ret = libusb_bulk_transfer(handle, QHYCCD_DATA_READ_ENDPOINT, data + pos,
										to_read, &transfered, (int)exp_time + 1500);

		if( ret != LIBUSB_SUCCESS )
		{
			log_warning("Retrying frame! read: %d, ret: %d.", transfered, ret);

			if( try_cnt > 3 )
			{
				log_error("Frame Failed! bytes read: %d, ret: %d.", transfered, ret);
				return QHYCCD_ERROR_EXPFAILED;
			}

			try_cnt++;
			continue;
		}

		log_debug("Transfered bytes = %d", transfered);

		pos += transfered;
		to_read -= transfered;

		/* Here we are using the pattern as a frame delimiter. If we still have bytes
		   to read and the pattern is found then the frames are missalined and we are at
		   the end of the previous framefram We have to start agin.
		*/
		unsigned char pat[4] = {0xaa, 0x11, 0xcc, 0xee};
		void *ppat = memmem(data + pos - 5, 4, pat, 4);

		if ((to_read) && (ppat))
		{
			//if( DBG_VERBOSITY )
			log_debug("Aligning frame, pos=%d, to_read=%d.", pos, to_read);
			pos = 0;
			to_read = data_size + 5;
			continue;
		}

		/* If by accident to_read is 0 and we are not at the end of the frame
		   we have missed the alignment pattern, so look for the next one.
		*/
		if ((to_read <= 0) && (ppat == NULL))
		{
			//if ( DBG_VERBOSITY )
			log_error("Frame seems to be invalid, retrying!");

			if( try_cnt > 3 )
			{
			//	log_e("Frame Failed - no pattern found!");
				return QHYCCD_ERROR_EXPFAILED;
			}

			pos = 0;
			to_read = data_size + 5;
			try_cnt++;
			continue;
		}

		log_debug("Read: %d of %d, try_cnt=%d, result=%d.", transfered, to_read + transfered, try_cnt, ret);
	}

	log_status("Got %d bytes of the image", transfered);

	return QHYCCD_SUCCESS;
}

unsigned char Qhy5II::MSB(unsigned short i)
{
	unsigned short j;
	j = (i&~0x00ff) >> 8;

	return j;
}

unsigned char Qhy5II::LSB(unsigned short i)
{
	unsigned short j;
	j = i&~0xff00;
	return j;
}

void Qhy5II::I2CTwoWrite(uint16_t addr, uint16_t value)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::I2CTwoWrite()");
#endif
	unsigned char data[2];
	data[0] = MSB(value);
	data[1] = LSB(value);

	CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xbb, 0, addr, data, 2);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::I2CTwoWrite()");
#endif
}

uint16_t Qhy5II::I2CTwoRead(uint16_t addr)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::I2CTwoWrite()");
#endif
	unsigned char data[2];

	CtrlMsg(handle, QHYCCD_REQUEST_READ, 0xb7, 0, addr, data, 2);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::I2CTwoWrite()");
#endif
    return (uint16_t)(data[0]) * 256 +  (uint16_t)data[1];
}

int Qhy5II::CtrlMsg(struct libusb_device_handle *handle
					, unsigned char request_type
					, unsigned char request
					, unsigned int value
					, unsigned int index
					, unsigned char *data
					, unsigned char len)
{
	if (!handle) {
		return QHYCCD_ERROR_NO_DEVICE;
	}
#ifdef ANNIYING_DEBUG
	log_debug("Control transfer request = 0x%02X value = 0x%02X", request, value);
#endif
	return libusb_control_transfer(handle, request_type, request, value, index, data, len, 5000);
}

int Qhy5II::EepromRead(struct libusb_device_handle *handle
						, unsigned char addr
						, unsigned char* data
						, unsigned short len)
{
	if (!handle) {
		return QHYCCD_ERROR_NO_DEVICE;
	}

	return Qhy5II::CtrlMsg(handle, QHYCCD_REQUEST_READ, 0xCA, 0, addr, data, len);
}

void Qhy5II::SetQHY5LIIHDR(bool on)
{
	if (on) {
		I2CTwoWrite(0x3082, 0x0028);
	} else {
		I2CTwoWrite(0x3082, 0x0001);
	}
}

void Qhy5II::SetResolution(const int w, const int h)
{
	if (dev_type == DEVICETYPE_QHY5LII) {
		CorrectQHY5LIIWH(w, h);
	} else if (dev_type == DEVICETYPE_QHY5II) {
		CorrectQHY5IIWH(w, h);
	}

	width = w;
	height = h;

	log_status("Set camera resolution, width = %i  height = %i", width, height);

	SetExposureTime(exp_time);
	SetGain(gain);
	SetUsbTraffic(usb_traf);
}

void Qhy5II::SetSpeed(const bool high_speed)
{
	if (high_speed) {
		usb_speed = 1;
	} else {
		usb_speed = 0;
	}

	log_debug("Set speed mode = %i", usb_speed);

	if (dev_type == DEVICETYPE_QHY5LII || dev_type == DEVICETYPE_QHY5II)
	{
		if (high_speed)
		{
			if (transfer_bit == 16) {
				SetSpeedQHY5LII(1);
			} else {
				SetSpeedQHY5LII(2);
			}
		} else {
			if (transfer_bit == 16) {
				SetSpeedQHY5LII(0);
			} else {
				SetSpeedQHY5LII(1);
			}
		}
	}
}

void Qhy5II::SetUsbTraffic(const int i)
{
	if (dev_type == DEVICETYPE_QHY5II) {
		I2CTwoWrite(0x05, 0x0009 + i * 50);
	} else if (dev_type == DEVICETYPE_QHY5LII) {
		if (width == 1280)
			I2CTwoWrite(0x300c, 1650 + i*50);
		else
			I2CTwoWrite(0x300c, 1388 + i*50);
	}

	log_debug("Set usb traffic = %i", usb_traf);

	usb_traf = i;
}

void Qhy5II::SetTransferBit(const int bit)
{
	if (dev_type == DEVICETYPE_QHY5LII) {
		transfer_bit = bit;

		if (bit == 16) {
			Set14Bit(1);
		} else {
			Set14Bit(0);
		}

		SetSpeed((usb_speed != 0));
	} else {
		transfer_bit = 8;
	}

	log_debug("Set transfer bit = %i", transfer_bit);
}

int Qhy5II::SetExposureTime(const int exptime)
{
	bool err = false;

	if (dev_type == DEVICETYPE_QHY5LII) {
		log_debug("Set QHY5LII exposure time = %i ms", exptime);

		SetExposureTime_QHY5LII(exptime);
		exp_time = exptime;
	} else if (dev_type == DEVICETYPE_QHY5II) {
		log_debug("Set QHY5II exposure time = %i ms", exptime);

		SetExposureTime_QHY5II(exptime);
		exp_time = exptime;
	} else {
		err = true;
	}

	return err ? QHYCCD_ERROR_SETEXPOSE : QHYCCD_SUCCESS;
}

int Qhy5II::SetGain(const unsigned short new_gain)
{
	int ret = QHYCCD_ERROR_SETGAIN;

	StopVideo();

	/* TO BE CLARIFIED: set_gain strange cludge to fix XXXX issuses on ARM MK808,
	though this is not needed on x86. */
#ifdef __arm__
	if (dev_type == DEVICETYPE_QHY5LII)
	{
		CorrectQHY5LIIWH(width, height);
		SetExposureTime(exp_time);
		ret = SetGainCore(new_gain);
		SetUsbTraffic(usb_traf);
	} else {
		ret = SetGainCore(new_gain);
	}
#else
	ret = SetGainCore(new_gain);
#endif

	if (ret == QHYCCD_SUCCESS) {
		gain = new_gain;
	}

	if (is_color) {
		log_debug("Set gain = %i  wbblue = %i  wbred = %i", gain, wbblue, wbred);
	} else {
		log_debug("Set gain = %i\n", gain);
	}

	StartVideo();

	return ret;
}

int Qhy5II::SetGainCore(const unsigned short new_gain)
{
    // one more crutch
    unsigned short normal_gain = new_gain > 0 ? new_gain : 1;

	bool err = false;

	if (dev_type == DEVICETYPE_QHY5LII) {
		SetQHY5LIIGain(normal_gain);
	} else if (dev_type == DEVICETYPE_QHY5II) {
        SetQHY5IIGain(normal_gain);
	} else {
		err = true;
	}

    return err ? QHYCCD_ERROR_SETGAIN : QHYCCD_SUCCESS;
}

void Qhy5II::SetSpeedQHY5LII(const unsigned char i)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetSpeedQHY5LII()");
#endif
	unsigned char buf[2] = { i, 0 };

	CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc8, 0x00, 0x00, buf, 1);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetSpeedQHY5LII()");
#endif
}

void Qhy5II::SetQHY5LIIGain(const unsigned short set_gain)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetQHY5LIIGain()");
#endif
	unsigned short real_gain = (set_gain * 10); // gain 0-100  => 0 - 1000

	if (long_exp_mode) {
		SetExposureTime_QHY5LII(1);

		if (is_color) {
			double RG,BG;

			RG = (double) wbred / 100;
			BG = (double) wbblue / 100;

			SetGainColorQHY5LII(real_gain, RG, BG);
		} else {
			SetGainMonoQHY5LII(real_gain);
		}

		usleep(500000);
		SetExposureTime_QHY5LII(exp_time);

	} else {
		if (is_color){
			double RG,BG;

			RG = (double) wbred / 100;
			BG = (double) wbblue / 100;

			SetGainColorQHY5LII(real_gain, RG, BG);
		} else {
			SetGainMonoQHY5LII(real_gain);
		}
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetQHY5LIIGain()");
#endif
}

void Qhy5II::SetGainMonoQHY5LII(const double set_gain)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetGainMonoQHY5LII()");
#endif
	// gain input range 0-1000
	int Gain_Min = 0;
	int Gain_Max = 796;

	double real_gain = (Gain_Max - Gain_Min) * set_gain / 1000;

	real_gain = real_gain / 10; // range:0-39.8

	unsigned short REG30B0;

	if (long_exp_mode) {
		REG30B0 = 0X5330;
	} else {
		REG30B0 = 0X1330;
	}

	unsigned short baseDGain;

	double C[8] = {10, 8, 5, 4, 2.5, 2, 1.25, 1};
	double S[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int A[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int B[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	double Error[8];

	for (int i = 0; i < 8; i++) {
		S[i] = real_gain / C[i];
		A[i] = (int)(S[i]);
		B[i] = (int)((S[i] - A[i]) / 0.03125);

		if (A[i] > 7) {
			A[i] = 10000;
		}

		if (A[i] == 0) {
			A[i] = 10000;
		}

		Error[i] = fabs(((double)(A[i])+(double)(B[i]) * 0.03125) * C[i] - real_gain);
	}

	double minValue;
	int minValuePosition;

	minValue = Error[0];
	minValuePosition = 0;

	for (int i = 0; i < 8; i++) {

		if (minValue > Error[i]) {
			minValue = Error[i];
			minValuePosition = i;
		}
	}

	int AA, BB, CC;
	double DD;
	double EE;
	(void)EE;

	AA = A[minValuePosition];
	BB = B[minValuePosition];

	if (minValuePosition == 0) {
		CC = 8;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 1) {
		CC = 8;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 2) {
		CC = 4;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 3) {
		CC = 4;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 4) {
		CC = 2;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 5) {
		CC = 2;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 6) {
		CC = 1;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 7) {
		CC = 1;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	EE = fabs(((double)(AA)+(double)(BB) * 0.03125) * CC * DD - real_gain);

	baseDGain = BB + AA * 32;
	I2CTwoWrite(0x305E, baseDGain);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetGainMonoQHY5LII()");
#endif
}

void Qhy5II::SetGainColorQHY5LII(const double set_gain, const double RG, const double BG)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetGainColorQHY5LII()");
#endif
	// gain input range 0-1000
	int Gain_Min = 0;
	int Gain_Max = 398;

	double real_gain = set_gain;

	if (real_gain < 26) {
		real_gain = 26;
	}

	real_gain = (Gain_Max - Gain_Min) * real_gain / 1000;

	real_gain = real_gain / 10; // range:0-39.8

	unsigned short REG30B0;

	if (long_exp_mode) {
		REG30B0 = 0x5330;
	} else {
		REG30B0 = 0x1330;
	}

	unsigned short baseDGain;

	double C[8] = {10, 8, 5, 4, 2.5, 2, 1.25, 1};
	double S[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int A[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int B[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	double Error[8];

	for (int i = 0; i < 8; i++) {
		S[i] = real_gain / C[i];
		A[i] = (int)(S[i]);
		B[i] = (int)((S[i] - A[i]) / 0.03125);
		if (A[i] > 3)
			A[i] = 10000;
		if (A[i] == 0)
			A[i] = 10000;
		Error[i] = fabs(((double)(A[i])+(double)(B[i]) * 0.03125) * C[i] - real_gain);
	}

	double minValue;
	int minValuePosition;

	minValue = Error[0];
	minValuePosition = 0;

	for (int i = 0; i < 8; i++) {
		if (minValue > Error[i])
		{
			minValue = Error[i];
			minValuePosition = i;
		}
	}

	int AA, BB, CC;
	double DD;
	double EE;
	(void)EE;

	AA = A[minValuePosition];
	BB = B[minValuePosition];

	if (minValuePosition == 0) {
		CC = 8;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 1) {
		CC = 8;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 2) {
		CC = 4;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 3) {
		CC = 4;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 4) {
		CC = 2;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 5) {
		CC = 2;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	if (minValuePosition == 6) {
		CC = 1;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD308);
	}

	if (minValuePosition == 7) {
		CC = 1;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	EE = fabs(((double)(AA)+(double)(BB) * 0.03125) * CC * DD - real_gain);

	baseDGain = BB + AA * 32;

	I2CTwoWrite(0x3058, (unsigned short)(baseDGain*BG));
	I2CTwoWrite(0x305a, (unsigned short)(baseDGain*RG));
	I2CTwoWrite(0x305c, baseDGain);
	I2CTwoWrite(0x3056, baseDGain);
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetGainColorQHY5LII()");
#endif
}

void Qhy5II::SetExposureTime_QHY5LII(const int val)
{
#ifdef ANNIYING_DEBUG
	log_debug("--> Qhy5II::SetExposureTime_QHY5LII()");
#endif
	unsigned int i = (val * 1000); // ms => us

	unsigned curr_expo = (exp_time) * 1000;

	if ((curr_expo < 4000 && i >= 4000) || (curr_expo >= 4000 && i < 4000)) {
		//CorrectQHY5LIIWH( &m_width, &m_height );
		//log_e( "Must not be called" );
	}

	if (i >= 1200000) {
		long_exp_mode = true;
	} else {
		long_exp_mode = false;
	}

	// Required input parameters: CMOSCLK
	double CMOSCLK;

	if (usb_speed == 1) {
		if (transfer_bit == 16) {
			CMOSCLK = 24;
		} else {
			CMOSCLK = 48;
		}
	} else {
		if (transfer_bit == 16) {
			CMOSCLK = 12;
		} else {
			CMOSCLK = 24;
		}
	}

	double pixelPeriod;
	pixelPeriod = 1 / (CMOSCLK * pll); // unit: us

	double RowTime;
	uint32_t ExpTime;
	unsigned short REG300C, REG3012;

	double MaxShortExpTime;

	if (!long_exp_mode) {
		if (i > 2 * 1000 * 1000)
			i = 2 * 1000 * 1000;
	}

	REG300C = I2CTwoRead(0x300C);

	RowTime = REG300C * pixelPeriod;

	MaxShortExpTime = 65000 * RowTime;

	if (!long_exp_mode) {
		if ((double)i > MaxShortExpTime) {
			i = (uint32_t) MaxShortExpTime;
		}
	}

	ExpTime = i;

	unsigned char buf[4];

	if (ExpTime > MaxShortExpTime) {
		I2CTwoWrite(0x3012, 65000);

		ExpTime = ExpTime - MaxShortExpTime;

		buf[0] = 0;
		buf[1] = (uint8_t)(((ExpTime / 1000) & (~0xff00ffff)) >> 16);
		buf[2] = (uint8_t)(((ExpTime / 1000) & (~0xffff00ff)) >> 8);
		buf[3] = (uint8_t)((ExpTime / 1000) & (~0xffffff00));

		CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4);

		ExpTime = ExpTime + MaxShortExpTime;
		REG3012 = 65000;

		long_exp_mode = true;

	} else {

		if (long_exp_mode == true) {
			long_exp_mode = false;

			// After switching over, it should be performed once more
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
			buf[3] = 0;

			CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4);

			usleep(100);

			REG3012 = (unsigned short)(ExpTime / RowTime);

			if (REG3012 < 1) {
				REG3012 = 1;
			}

			I2CTwoWrite(0x3012, REG3012);
			ExpTime = (uint32_t)REG3012 * RowTime;
		}

		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4);

		usleep(100);

		REG3012 = (unsigned short)(ExpTime / RowTime);

		if (REG3012 < 1) {
			REG3012 = 1;
		}

		I2CTwoWrite(0x3012, REG3012);
		ExpTime = (uint32_t)REG3012 * RowTime;
		long_exp_mode = false;
	}
#ifdef ANNIYING_DEBUG
	log_debug("<-- Qhy5II::SetExposureTime_QHY5LII()");
#endif
}

void Qhy5II::CorrectQHY5LIIWH(const int w, const int h)
{
	int local_width = w;
	int local_height = h;

    if (local_width <= 320 && local_height <=240) {
		InitQHY5LII_QVGA();

		local_width = 320;
		local_height = 240;
	} else if (local_width <= 640 && local_height <= 480) {
		InitQHY5LII_VGA();

		local_width = 640;
		local_height = 480;
	} else if (local_width <= 800 && local_height <= 600) {
		InitQHY5LII_SVGA();

		local_width = 800;
		local_height = 600;
	} else if (local_width <= 1024 && local_height <= 768) {
		InitQHY5LII_XGA();

		local_width = 1024;
		local_height = 768;
	} else {
		InitQHY5LII_1280X960();

		local_width = 1280;
		local_height = 960;
	}

	width = local_width;
	height = local_height;
}

void Qhy5II::InitQHY5LII_XGA()
{
	int x = 1024;
	int y = 768;

	InitCmos();
	pll = SetQHY5LREG_PLL(0);

	int xstart = 4 + (1280 - 1024) / 2;
	int ystart = 4 + (960 - 768) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	log_status("Selected camera resolution is 1024x768");

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 795); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}

void Qhy5II::InitQHY5LII_1280X960()
{
	int x = 1280;
	int y = 960;

	InitCmos();
	pll = SetQHY5LREG_PLL(0);

	int xstart = 4;
	int ystart = 4;
	int xsize = x - 1;
	int ysize = y - 1;

	log_status("Selected camera resolution is 1280x960");

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 990); // frame length
	I2CTwoWrite(0x300c, 1650); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}

void Qhy5II::InitQHY5LII_SVGA()
{
	int x = 800;
	int y = 600;

	InitCmos();
	pll = SetQHY5LREG_PLL(2);

	int xstart = 4 + (1280 - 800) / 2;
	int ystart = 4 + (960 - 600) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	log_status("Selected camera resolution is 800x600");

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 626); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}

void Qhy5II::InitQHY5LII_VGA()
{
	int x = 640;
	int y = 480;

	InitCmos();
	pll = SetQHY5LREG_PLL(1);

	int xstart = 4 + (1280 - 640) / 2;
	int ystart = 4 + (960 - 480) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	log_status("Selected camera resolution is 640x480");

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 506); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}

void Qhy5II::InitQHY5LII_QVGA()
{
	int x = 320;
	int y = 240;

	InitCmos();
	pll = SetQHY5LREG_PLL(1);

	int xstart = 4 + (1280 - 320) / 2; ;
	int ystart = 4 + (960 - 320) / 2; ;
	int xsize = x - 1;
	int ysize = y - 1;

	log_status("Selected camera resolution is 320x240");

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 266); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}

void Qhy5II::Set14Bit(unsigned char i)
{
	unsigned char buf[2] = { i, 0 };
	//buf[0] = i;
	CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xcd, 0x00, 0x00, buf, 1);
}

double Qhy5II::SetQHY5LREG_PLL(unsigned char clk)
{
	double i = 0;

	if (clk == 0) {
		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 42); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);

		if(long_exp_mode) {
			I2CTwoWrite(0x30B0, 0x5330);
			i = 1.0;
		} else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = 1.0;
		}

		// 5330
		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);

	} else if (clk == 1) {
		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 65); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);
		// OPERATION_MODE_CTRL

		if (long_exp_mode) {
			I2CTwoWrite(0x30B0, 0x5330);
			i = 1.0;
		} else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = (double)65.0 / 14.0 / 3.0;
		}

		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);

	} else if (clk == 2) {
		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 57); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);
		// OPERATION_MODE_CTRL

		if (long_exp_mode) {
			I2CTwoWrite(0x30B0, 0x5330);
			// DIGITAL_TEST    5370: PLL BYPASS   1370  USE PLL
			i = 1.0;
		} else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = (double)57.0 / 14.0 / 3.0;
		}

		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);
	}

	return i;
}


//----------------------------------- QHY5-II stuff -----------------------------------
void Qhy5II::SetExposureTime_QHY5II(uint32_t i)
{
	i *= 1000; // ms => us

	// Required input parameters: CMOSCLK  REG04  REG05 REG0C REG09
	double CMOSCLK;

	if (usb_speed == 1) {
		CMOSCLK = 48;
	} else {
		CMOSCLK = 24;
	}

	double pixelPeriod;
	pixelPeriod = 1 / CMOSCLK; // unit: us

	double A, Q;
	double P1, P2;
	double RowTime; // unit: us
	uint32_t ExpTime; // unit: us
	unsigned short REG04, REG05, REG0C, REG09;
	double MaxShortExpTime;

	REG04 = I2CTwoRead(0x04);
	REG05 = I2CTwoRead(0x05);
	REG09 = I2CTwoRead(0x09);
	REG0C = I2CTwoRead(0x0C);
	ExpTime = i;

	A = REG04 + 1;
	P1 = 242;
	P2 = 2 + REG05 - 19;
	Q = P1 + P2;
	RowTime = (A + Q) * pixelPeriod;

	MaxShortExpTime = 15000 * RowTime - 180 * pixelPeriod - 4 * REG0C * pixelPeriod;

	unsigned char buf[4];

	if (ExpTime > MaxShortExpTime)
	{
		I2CTwoWrite(0x09, 15000);
		ExpTime = (uint32_t)(ExpTime - MaxShortExpTime);
		buf[0] = 0;
		buf[1] = (uint8_t)(((ExpTime / 1000) & (~0xff00ffff)) >> 16);
		buf[2] = (uint8_t)(((ExpTime / 1000) & (~0xffff00ff)) >> 8);
		buf[3] = (uint8_t)((ExpTime / 1000) & (~0xffffff00));

		CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4);
		ExpTime = (uint32_t)(ExpTime + MaxShortExpTime);
	} else {
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		CtrlMsg(handle, QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4);
		usleep(100);
		REG09 = (unsigned short)((ExpTime + 180 * pixelPeriod + 4 * REG0C * pixelPeriod) / RowTime);
		if( REG09 < 1 )
			REG09 = 1;
		I2CTwoWrite( 0x09, REG09 );
		ExpTime = (uint32_t)(REG09 * RowTime - 180 * pixelPeriod - 4 * REG0C * pixelPeriod);
	}
}

void Qhy5II::QHY5IISetResolution(int x, int y)
{
	I2CTwoWrite(0x09, 200);
	I2CTwoWrite(0x01, 8 + (1024 - y) / 2); // y start
	I2CTwoWrite(0x02, 16 + (1280 - x) / 2); // x start
	I2CTwoWrite(0x03, (unsigned short)(y - 1)); // y size
	I2CTwoWrite(0x04, (unsigned short)(x - 1)); // x size
	I2CTwoWrite(0x22, 0x00); // normal bin
	I2CTwoWrite(0x23, 0x00); // normal bin
}

void Qhy5II::CorrectQHY5IIWH(int w, int h)
{
    if (w <= 640 && h <= 480) {
        w = 640;
        h = 480;
        initQHY5II_VGA();
    } else if (w <= 800 && h <= 600) {
        w = 800;
        h = 600;
        initQHY5II_SVGA();
    } else if (w <= 1024 && h <= 768) {
        w = 1024;
        h = 768;
        initQHY5II_XGA();
    } else if(w <= 1280 && h <= 1024) {
        w = 1280;
        h = 1024;
        initQHY5II_SXGA();
    }
}

void Qhy5II::initQHY5II_SXGA()
{
	QHY5IISetResolution( 1280, 1024 );
}

void Qhy5II::initQHY5II_XGA()
{
	QHY5IISetResolution(1024, 768);
}

void Qhy5II::initQHY5II_SVGA()
{
	QHY5IISetResolution(800, 600);
}

void Qhy5II::initQHY5II_VGA()
{
	QHY5IISetResolution(640, 480);
}

void Qhy5II::SetQHY5IIGain(unsigned short gain)
{
	gain *= 10; // gain 0-100  => 0 - 1000

	int i = 0;
	int Gain_Min = 0;
	int Gain_Max = 72;

	i = (Gain_Max - Gain_Min) * gain / 1000;

	I2CTwoWrite(0x35, gain_lut[i]);
}
//----------------------------------------


void Qhy5II::InitCmos()
{
	// [720p, 25fps input27Mhz,output50Mhz, ]
	I2CTwoWrite(0x301A, 0x0001); // RESET_REGISTER
	I2CTwoWrite(0x301A, 0x10D8); // RESET_REGISTER
	usleep(200000);
	/////Linear sequencer
	I2CTwoWrite(0x3088, 0x8000); // SEQ_CTRL_PORT
	I2CTwoWrite(0x3086, 0x0025); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2D26); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0D17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0926); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA728); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0725); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8080); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2925); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0040); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2702); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x3626); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0326); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA417); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0425); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0519); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1741); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2660); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x175A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1122); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1741); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x9027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0026); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x002E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2A28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x081C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7003); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7005); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x170C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0414); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0414); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0514); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2405); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5001); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2550); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2608); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x280D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1709); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2600); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A7); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2807); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2580); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8029); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0216); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1627); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1736); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A6); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A4); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171F); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2804); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2520); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1D25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1710); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1A17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0327); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4126); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x6017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xAE25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0090); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2618); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2800); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2E2A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2808); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1D05); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1720); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2024); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2550); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2608); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x280D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1709); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2600); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A7); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2807); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2580); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8029); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0216); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1627); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x3626); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0326); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA417); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0425); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2021); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1710); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1B17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0327); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4126); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x6017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xAE25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0090); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2618); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2800); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2E2A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2808); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1E17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0A05); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2024); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502B); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x302C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C00); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0225); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2D26); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0D17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0926); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA728); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0725); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8080); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2917); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0525); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0040); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2702); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1736); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A6); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A4); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171F); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2804); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2520); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1E25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2117); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x051B); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1747); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2660); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x17AE); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x9027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0026); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x002E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2A28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x081E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0831); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1440); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1410); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1034); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4013); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1802); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7003); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5022); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x309E, 0x018A); // RESERVED_MFR_309E
	I2CTwoWrite(0x301A, 0x10D8); // RESET_REGISTER
	I2CTwoWrite(0x3082, 0x0029); // OPERATION_MODE_CTRL
	I2CTwoWrite(0x301E, 0x00C8); // DATA_PEDESTAL
	I2CTwoWrite(0x3EDA, 0x0F03); // RESERVED_MFR_3EDA
	I2CTwoWrite(0x3EDE, 0xC007); // RESERVED_MFR_3EDE
	I2CTwoWrite(0x3ED8, 0x01EF); // RESERVED_MFR_3ED8
	I2CTwoWrite(0x3EE2, 0xA46B); // RESERVED_MFR_3EE2
	I2CTwoWrite(0x3EE0, 0x067D); // RESERVED_MFR_3EE0
	I2CTwoWrite(0x3EDC, 0x0070); // RESERVED_MFR_3EDC
	I2CTwoWrite(0x3044, 0x0404); // DARK_CONTROL
	I2CTwoWrite(0x3EE6, 0x4303); // RESERVED_MFR_3EE6
	I2CTwoWrite(0x3EE4, 0xD208); // DAC_LD_24_25
	I2CTwoWrite(0x3ED6, 0x00BD); // RESERVED_MFR_3ED6
	I2CTwoWrite(0x3EE6, 0x8303); // RESERVED_MFR_3EE6
	I2CTwoWrite(0x30E4, 0x6372); // RESERVED_MFR_30E4
	I2CTwoWrite(0x30E2, 0x7253); // RESERVED_MFR_30E2
	I2CTwoWrite(0x30E0, 0x5470); // RESERVED_MFR_30E0
	I2CTwoWrite(0x30E6, 0xC4CC); // RESERVED_MFR_30E6
	I2CTwoWrite(0x30E8, 0x8050); // RESERVED_MFR_30E8
	usleep(100000);
	I2CTwoWrite(0x302A, 14); // DIV           14
	I2CTwoWrite(0x302C, 1); // DIV
	I2CTwoWrite(0x302E, 3); // DIV
	I2CTwoWrite(0x3030, 65); // MULTI          44
	I2CTwoWrite(0x3082, 0x0029);
	// OPERATION_MODE_CTRL
	I2CTwoWrite(0x30B0, 0x1330);
	I2CTwoWrite(0x305e, 0x00ff); // gain
	I2CTwoWrite(0x3012, 0x0020);
	// coarse integration time
	I2CTwoWrite(0x3064, 0x1802);
}


int Qhy5II::GetDeviceType(libusb_device_handle *handle, bool &is_color)
{
	unsigned char data[16];

	int ret = Qhy5II::EepromRead(handle, 0x10, data, sizeof(data));

	int dev_type = DEVICETYPE_UNKOWN;
	is_color = false;

	if (ret == 0x10) {
		int model = data[0];
		is_color = data[1];

		if (model == 1) {
			dev_type = DEVICETYPE_QHY5II;
		} else if (model == 6) {
			dev_type = DEVICETYPE_QHY5LII;
		} else if (model == 9) {
			dev_type = DEVICETYPE_QHY5RII;
		}
	}

	return dev_type;
}

void Qhy5II::DumpDeviceInfo(libusb_device *tdev)
{
	libusb_device_handle *handle;

	if (libusb_open(tdev, &handle) != 0) {
		std::cerr << "Failed to open QHY device" << std::endl;
		libusb_close(handle);
	}

	bool is_color;
	int dev_type = Qhy5II::GetDeviceType(handle, is_color);

	switch (dev_type) {
		case DEVICETYPE_QHY5II:
			std::cout << "QHY5II monochrome ccd camera" << std::endl;
			break;

		case DEVICETYPE_QHY5LII:
			std::cout << "QHY5LII"
				<< (is_color == 1 ? "C color" : "M monochrome") << " ccd camera." << std::endl;
			break;

		case DEVICETYPE_QHY5RII:
			std::cout << "QHY5RII ccd camera" << std::endl;
			break;

		case DEVICETYPE_UNKOWN:
			std::cout << "Unknown ccd camera" << std::endl;
	}

	std::cout << std::endl;

	libusb_close(handle);
}

