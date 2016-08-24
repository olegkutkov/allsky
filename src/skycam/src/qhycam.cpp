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

#include <libusb-1.0/libusb.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "qhycam.hpp"
#include "qhy5ii.hpp"
#include "logger.h"

#define MAXDEVICES 100

static unsigned short qhy_vid[MAXDEVICES] = 
{
    0x1618,0x1618,0x1618,0x1618,0x1618,0x1618,0x1618,0x1618,0x1618,
    0x1618,0x1618,0x1618,0x1618,0X1618,0x1618,0x1618
};

static unsigned short qhy_pid[MAXDEVICES] =
{
    0x0921,0x8311,0x6741,0x6941,0x6005,0x1001,0x1201,0x8301,0x6003,
    0x1101,0x8141,0x2851,0x025a,0x6001,0x0931,0xffff
};

/////

int QhyCam::InitializeSystem()
{
	return libusb_init(NULL);
}

void QhyCam::ReleaseSystem()
{
	libusb_exit(NULL);
}

static bool IsQhyDevice(const int vid, const int pid)
{
    for (int i = 0; i < MAXDEVICES; ++i) {
        if ((qhy_vid[i] == vid) && (qhy_pid[i] == pid)) {
            return true;
        }
    }

    return false;
}

int QhyCam::ScanQhy()
{
	static libusb_device **list;
	int numdev = libusb_get_device_list(NULL, &list);

	if (numdev < 0) {
		return QHYCCD_ERROR_NO_DEVICE;
	}

	for (int i = 0; i < numdev; ++i) {
		libusb_device *tdev = list[i];
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(tdev, &desc);

		if (IsQhyDevice(desc.idVendor, desc.idProduct)) {
			std::cout << "QHY device: " << std::hex 
					<< std::endl << "\tVID: 0x" << desc.idVendor 
					<< std::endl << "\tPID: 0x" << desc.idProduct
					<< std::endl << "\tModel: ";

			switch (desc.idProduct) {
				case QHY5II_PRODUCT_ID:
					Qhy5II::DumpDeviceInfo(tdev);
					break;

				default:
					std::cout << " not supported model" << std::endl;
			}
		}
	}

	libusb_free_device_list(list, 0);

	return 0;
}

QhyCam::QhyCam()
	: width (1280)
	, height (960)
	, camera_connected (false)
	, grayscale_mode (true)
	, camera_interface (NULL)
{
}

QhyCam::~QhyCam()
{
	if (camera_interface) {
		StopCapture();
		DisconnectCamera();

		delete camera_interface;
		camera_interface = NULL;
	}
}

int QhyCam::ConnectCamera(const DEVICE_TYPE dev_type)
{
	int res;

	switch (dev_type) {
		case DEVICE_QHY5II:
			log_status("Selecting QHY5II camera interface");
			camera_interface = new Qhy5II();

			if (!camera_interface) {
				log_error("Failed to allocate memory for camera interface");
				return QHYCCD_ERROR_INITCLASS;
			}

			res = camera_interface->OpenDevice();

			if (res != QHYCCD_SUCCESS) {
				log_error("Device opening failed, error = %d", res);
				return res;
			}

			camera_interface->InitDevice();

			camera_connected = true;

			return res;

		case DEVICE_QHY5:
		case DEVICE_QHY6:
			log_status("Selecting QHY5 | QHY6 camera interface");
		default:
			return QHYCCD_ERROR_NOTSUPPORT;
	}

	return QHYCCD_ERROR_NO_DEVICE;
}

void QhyCam::DisconnectCamera()
{
	if (camera_interface) {
		camera_interface->CloseDevice();
		camera_connected = false;
	}
}

bool QhyCam::IsCameraConnected()
{
	return camera_connected;
}

void QhyCam::SetColorMode()
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return;
	}

	if (!camera_interface->IsColor()) {
		return;
	}

	log_status("Camera switched in color mode");

	grayscale_mode = false;
}

void QhyCam::SetGrayscaleMode()
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return;
	}

	if (!camera_interface->IsColor()) {
		return;
	}

	log_status("Camera switched in grayscale mode");

	grayscale_mode = true;
}

int QhyCam::SetCameraResolution(const int w, const int h)
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return QHYCCD_ERROR_NO_DEVICE;
	}

	int res = camera_interface->SetParam(CONTROL_RESOLUTION, param_val(w, h));

	if (res == QHYCCD_SUCCESS) {
		width = w;
		height = h;
	} else {
		log_error("Unable to set requested resolution");
	}

	return res;
}

int QhyCam::SetCameraExposureTime(const unsigned int time_ms)
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return QHYCCD_ERROR_NO_DEVICE;
	}

	return camera_interface->SetParam(CONTROL_EXPOSURE, param_val(time_ms, 0));
}

int QhyCam::SetCameraGain(const unsigned short gain)
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return QHYCCD_ERROR_NO_DEVICE;
	}

	return camera_interface->SetParam(CONTROL_GAIN, param_val(gain, 0));
}

void QhyCam::StartCapture()
{
	camera_interface->StartVideo();
}

void QhyCam::StopCapture()
{
	camera_interface->StopVideo();
}

int QhyCam::GetFrame(cv::Mat &result_image)
{
	if (!camera_connected) {
		log_error("Camera is not connected");
		return QHYCCD_ERROR_NO_DEVICE;
	}

	size_t data_size = camera_interface->GetImageMemorySize();
	unsigned char *buf;

	buf = (unsigned char*) malloc(data_size);

	camera_interface->StartVideo();

	int res = camera_interface->GetFrame(buf, data_size);

	if (res != QHYCCD_SUCCESS) {
		free(buf);
		return res;
	}

	cv::Mat bayer_image(height, width, CV_8UC1, buf);

	if (!grayscale_mode) {
		log_status("Camera is in color mode, perfom BAYER to RGB conversion");

		cv::Mat color_image(height, width, CV_8UC3);
		cv::cvtColor(bayer_image, result_image, CV_BayerGR2RGB);
	} else {
		log_status("Camera is in grayscale mode");

		result_image = bayer_image;
	}

	free(buf);

	return res;
}

