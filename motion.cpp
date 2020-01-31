
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

int main() {

	// Create a VideoCapture object and open the input file
	// If the input is the web camera, pass 0 instead of the video file name
	VideoCapture cap("belt_bg.wmv");

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int width = cap.get(CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CAP_PROP_FRAME_HEIGHT);
	int count = cap.get(CAP_PROP_FRAME_COUNT);

	Mat Mean = Mat::zeros(height, width, CV_32F);
	Mat SD = Mat::zeros(height, width, CV_32F);


	while (1) {

		Mat frame;
		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		Mat greyFrame = Mat::zeros(height, width, CV_32F);

		cvtColor(frame, greyFrame, COLOR_RGBA2GRAY);
		greyFrame.convertTo(greyFrame, CV_32F, 1 / 255.0);
		accumulate(greyFrame, Mean, noArray());

	}

	Mean = Mean / count;
	cap.release();

	cap.open("belt_bg.wmv");

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	while (1) {

		Mat frame;
		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		Mat greySDFrame = Mat::zeros(height, width, CV_32F);
		Mat meanDiffFrame = Mat::zeros(height, width, CV_32F);
		cvtColor(frame, greySDFrame, COLOR_RGBA2GRAY);
		greySDFrame.convertTo(greySDFrame, CV_32F, 1 / 255.0);
		subtract(greySDFrame, Mean, meanDiffFrame, noArray(), CV_32F);
		accumulateSquare(meanDiffFrame, SD, noArray());

	}

	sqrt(SD / count, SD);
	cap.release();

	cap.open("belt_fg.wmv");

	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int threshold = 20;
	int washerCount = 0,
		QKeyChainCount = 0,
		pegCount = 0,
		nutCount = 0, prongCount = 0,
		pipeCount = 0;
	bool washerFlag = false,
		QKeyChainCountFlag = false,
		pegFlag = false,
		pipeFlag = false,
		nutFlag = false,
		prongFlag = false;

	while (1) {
		Mat resultFrame = Mat::zeros(height, width, CV_32F);
		Mat foreGroundFrame;
		// Capture frame-by-frame
		cap >> foreGroundFrame;

		// If the frame is empty, break immediately
		if (foreGroundFrame.empty())
			break;

		Mat backGroundFrame = Mat::zeros(height, width, CV_32F);

		cvtColor(foreGroundFrame, backGroundFrame, COLOR_RGBA2GRAY);
		backGroundFrame.convertTo(backGroundFrame, CV_32F, 1 / 255.0);
		for (int r = 0; r < backGroundFrame.rows; r++) {
			for (int c = 0; c < backGroundFrame.cols; c++) {

				// Main Background Subtractor 
				if ((backGroundFrame.at<float>(r, c) - Mean.at<float>(r, c)) >
					threshold * SD.at<float>(r, c)) {
					resultFrame.at<float>(r, c) = 1.0;  // set pixel to white
				}
				else {
					resultFrame.at<float>(r, c) = 0.0; // set pixel to black
				}

			}
		}

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		resultFrame.convertTo(resultFrame, CV_8U, 255);

		findContours(resultFrame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


		for (int i = 0; i < contours.size(); i++) {
			//std::cout << i << endl;
			int area = contourArea(contours[i]);
			int arc = arcLength(contours[i], true);
			if (area > 4000 && area < 6000 && arc > 260 && arc < 303) {
				washerCount++;
				if (washerCount > 10 && washerFlag == false) {
					washerFlag = true;
					cout << "Widget Identified as Washer" << endl;
					cv::Mat washer;
					washer = imread("washer1.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("washer", washer);

				}
			}
			if (area > 6000 && area < 7900 && arc > 400 && arc < 500) {
				QKeyChainCount++;
				if (QKeyChainCount > 10 && QKeyChainCountFlag == false) {
					QKeyChainCountFlag = true;
					cout << "Widget Identified as QKeyChain" << endl;
					cv::Mat qKey;
					qKey = imread("q1.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("qKey", qKey);


				}
			}

			if (area > 3700 && area < 4600 && arc > 225 && arc < 280) {
				pegCount++;
				if (pegCount > 10 && pegFlag == false) {
					pegFlag = true;
					cout << "Widget Identified as Peg" << endl;
					cv::Mat peg;
					peg = imread("peg1.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("peg", peg);

				}
			}

			if (area > 8000 && area < 10200 && arc > 355 && arc < 410) {
				nutCount++;
				if (nutCount > 10 && nutFlag == false) {
					nutFlag = true;
					cout << "Widget Identified as Nut" << endl;
					cv::Mat nut;
					nut = imread("nut1.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("nut", nut);

				}
			}

			if (area > 3000 && area < 4400 && arc > 370 && arc < 500) {
				pipeCount++;
				if (pipeCount > 10 && pipeFlag == false) {
					pipeFlag = true;
					cout << "Widget Identified as Pipe" << endl;
					cv::Mat pipe;
					pipe = imread("pipe1.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("pipe", pipe);

				}
			}

			if (area > 7000 && area < 9200 && arc > 470 && arc < 625) {
				prongCount++;
				if (prongCount > 10 && prongFlag == false) {
					prongFlag = true;
					cout << "Widget Identified as Prong" << endl;
					cv::Mat prong;
					prong = imread("prong4.bmp", cv::IMREAD_UNCHANGED);
					cv::imshow("Prong", prong);

				}
			}

		}

		imshow("resultFrame", resultFrame);
		imshow("foreGroundFrame", foreGroundFrame);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	// When everything done, release the video capture object
	cap.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}