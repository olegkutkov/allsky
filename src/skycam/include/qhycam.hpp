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

#ifndef QHYCAM_HPP
#define QHYCAM_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include "qhybase.hpp"

class QhyCam
{
public:
	QhyCam();
	~QhyCam();

	static int InitializeSystem();
	static void ReleaseSystem();
	static int ScanQhy();

	int ConnectCamera(const DEVICE_TYPE dev_type);
	void DisconnectCamera();
	bool IsCameraConnected();

	void SetColorMode();
	void SetGrayscaleMode();

	int SetCameraResolution(const int w, const int h);
	int SetCameraExposureTime(const unsigned int time_ms);
	int SetCameraGain(const unsigned short gain);

	void StartCapture();
	int GetFrame(cv::Mat &result_image);
	void StopCapture();

private:
	int width;
	int height;
	bool camera_connected;
	bool grayscale_mode;
	QhyBase *camera_interface;
};

#endif

