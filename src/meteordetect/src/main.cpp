/*
 meteor detector
 - program entry point

 Copyright 2018  Oleg Kutkov <elenbert@gmail.com>

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

#include <memory>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>

#include "fits_handler.hpp"

int threshold_value = 5;
int threshold_type = 0;

int threshold_value_back = 0;
int threshold_type_back = 1;

int const max_BINARY_value = 255;

char* window_name = "img view";

///
cv::Mat proc_img, proc_img_thresholded, back_img, back_img_thresholded, edges, proc_img_c;

///

int min_threshold = 50;
int max_trackbar = 159;

int s_trackbar = max_trackbar;
int p_trackbar = max_trackbar;


void Threshold_Demo( int, void* )
{
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */

	cv::threshold(proc_img, proc_img_thresholded, threshold_value, max_BINARY_value, threshold_type);
//	cv::threshold(back_img, back_img_thresholded, threshold_value_back, max_BINARY_value, threshold_type_back);

//	proc_img_thresholded -= back_img_thresholded;

//	cv::Canny(proc_img_thresholded, edges, 20, 200, 3);

  std::vector<cv::Vec4i> p_lines;
//  cv::cvtColor(proc_img_thresholded, probabilistic_hough, cv::COLOR_GRAY2BGR );

  cv::cvtColor(proc_img_thresholded, proc_img_c, CV_GRAY2BGR);

  /// 2. Use Probabilistic Hough Transform
  cv::HoughLinesP(proc_img_thresholded, p_lines, 1, CV_PI/180, min_threshold + p_trackbar, 172, 3 );

  /// Show the result
  for( size_t i = 0; i < p_lines.size(); i++ )
     {
       cv::Vec4i l = p_lines[i];
       cv::line(proc_img_c, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, CV_AA);
     }

//imshow( probabilistic_name, probabilistic_hough );

//	cv::imshow(window_name, proc_img_c);

//	cv::imshow(window_name, proc_img_thresholded);
}

int main(int argc, char **argv)
{ 
	std::unique_ptr<FitsHandler> current_fits = std::unique_ptr<FitsHandler>(new FitsHandler(argv[1], false));

	current_fits->LoadImageData();

	std::unique_ptr<FitsHandler> previous_fits = std::unique_ptr<FitsHandler>(new FitsHandler(argv[2], false));

	previous_fits->LoadImageData();

//	current_fits->Substract(*previous_fits);

	// Create Mat object from array
//	cv::Mat proc_img, proc_img_thresholded, back_img, back_img_thresholded;
 
	current_fits->ToCvMat8(proc_img);
	previous_fits->ToCvMat8(back_img);

    int offset_x = 25;
    int offset_y = 25;

	cv::Rect roi;
	roi.x = offset_x;
	roi.y = offset_y;
	roi.width = proc_img.size().width - (offset_x*2);
	roi.height = proc_img.size().height - (offset_y*2);

	cv::Mat crop = proc_img(roi);
	proc_img = crop;

	crop = back_img(roi);
	back_img = crop;

//	proc_img -= back_img;

	cv::namedWindow(window_name);

  cv::createTrackbar( "type",
                  window_name, &threshold_type,
                  4, Threshold_Demo );

  cv::createTrackbar( "value",
                  window_name, &threshold_value,
                  255, Threshold_Demo );

  cv::createTrackbar("p hough", window_name, &p_trackbar, max_trackbar, Threshold_Demo);


  cv::createTrackbar( "back type",
                  window_name, &threshold_type_back,
                  4, Threshold_Demo );

  cv::createTrackbar( "back value",
                  window_name, &threshold_value_back,
                  255, Threshold_Demo );


	proc_img -= back_img;

	Threshold_Demo(0, 0);

//	cv::threshold(proc_img, proc_img_thresholded, threshold_value, max_BINARY_value, threshold_type);
//	cv::threshold(back_img, back_img_thresholded, threshold_value, max_BINARY_value, threshold_type);

//	proc_img_thresholded -= back_img_thresholded;

	// Display in window and wait for key press
//	cv::imshow("img view", proc_img_thresholded);

//	cv::waitKey(0);
}

