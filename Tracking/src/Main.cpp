/* 
Tracking calibration program to provide stepper feedback.
Detects stars, calculates their movement, and sends speed adjustments via usb to counter the movement.

This code is intended to work only for the Celestron Neximage Burst Monochrome usb camera.

Author: Alek Harden
*/

//Library includes
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//Standard includes
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>

//Project includes
#include <VideoFileManager.hpp>

void subtractMean(const cv::Mat & input, cv::Mat & dst);
void applySignedOffsetDark(const cv::Mat & input, const cv::Mat & signedOffsetDark, cv::Mat & dst);
void signedGrayscale16ToRedBlue8(const cv::Mat & signedGrayscale, cv::Mat & rgbOut);
void stack(std::vector<cv::Mat> imageArray, cv::Mat & dst);


enum CalibrationFrameType {
	CAL_DARK,

	CAL_FLAT,
	CAL_DARKFLAT //Darks for the flats
};



int main(){

	//Create VideoCapture object, pass 0 to open the first attached camera device.
	cv::VideoCapture cam(0,cv::CAP_DSHOW); //Neximage is compatible with DirectShow (Windows)

	if (!cam.isOpened()){
		std::cout << "Error opening video stream." << std::endl;
		std::cout << "Make sure you have plugged in: Neximage Burst Monochrome" << std::endl;
		return -1;
	}

	/*
	Valid Modes:
		1280x960
		1280x720
		1024x768
		640x480
	*/
	//Frame Size must be set before other properties, or they will not take effect.
	//Only valid width-height pair will take effect
	cam.set(cv::CAP_PROP_FRAME_WIDTH,1280);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT,960);

	//Store the actual size
	cv::Size frameSize = cv::Size(cam.get(cv::CAP_PROP_FRAME_WIDTH),cam.get(cv::CAP_PROP_FRAME_HEIGHT));

	//Non-Negotiables
	cam.set(cv::CAP_PROP_FOURCC,cv::VideoWriter::fourcc('Y','8','0','0'));
	cam.set(cv::CAP_PROP_AUTO_WB,true); //NEVER CHANGE (Checkerboarding may occur if rgb white balance values are not all equal to 64)
	cam.set(cv::CAP_PROP_AUTO_EXPOSURE,false);


	//Settings
	cam.set(cv::CAP_PROP_GAIN,2047);     //  34 --- 2047
	cam.set(cv::CAP_PROP_EXPOSURE,-13);  // -13 --- 4        //Exposure Time is Roughly equal to 2^(Exposure value)
	cam.set(cv::CAP_PROP_BRIGHTNESS,70); //   0 --- 255      


	//SHOW SETTINGS WINDOW!!!
	cam.set(cv::CAP_PROP_SETTINGS,true);

	
	//Create Video Window
	std::string videoWindow = "Video";
	cv::namedWindow(videoWindow,cv::WINDOW_NORMAL | cv::WINDOW_GUI_NORMAL);
	cv::resizeWindow(videoWindow,frameSize);
	cv::moveWindow(videoWindow, 600,200);


	//Generate Signed Offset Dark Frame 
	cv::Mat MasterDark = cv::imread("res\\calibration\\bias\\MasterBias.tif",cv::IMREAD_GRAYSCALE);
	cv::Mat MasterDarkSigned; 
	cv::Mat OffsetDarkSigned(MasterDarkSigned.size(),CV_16SC1);
	MasterDark.convertTo(MasterDarkSigned,CV_16SC1);
	subtractMean(MasterDarkSigned,OffsetDarkSigned);
	bool applyDarkFrameCalib = false;


	std::vector<cv::Mat> stackFrames;
	unsigned int numDarkFrames = 4000;
	unsigned int recordedFrames = 0;
	bool recDarkFrame = false;









	//Class for handling video recording and indexing
	VideoFileManager videoFileManager("bin\\videos");

	cv::Mat frame;

	bool running = true;
	while(running){

		//get frame from camera
		cam >> frame;

		//exit loop incase camera gets unplugged.
		//imshow will throw error for empty frame
		if (frame.empty()){
			break;
		}


		if (recDarkFrame){

			if (recordedFrames < numDarkFrames){
				stackFrames.push_back(frame.clone());
				recordedFrames++;
				std::cout << recordedFrames << std::endl;

			}
			else{
				recDarkFrame = false;
				std::cout << "Done recording " << recordedFrames << " dark frames." << std::endl;
			}

		}

		/*
		std::vector<cv::Point> features;
		cv::goodFeaturesToTrack(frame,features,1000,0.01,0);

		for(size_t i = 0; i < features.size(); i++){
			cv::circle(frame,features[i],1,cv::Scalar(0,255,255));
		}
		*/

		//Effects Pipeline

		if (applyDarkFrameCalib){
			cv::Mat calibratedFrame;
			applySignedOffsetDark(frame,OffsetDarkSigned,calibratedFrame);
			frame = calibratedFrame;

		}
		

		//Display frame
		cv::imshow(videoWindow,frame);

		//Record to video if opened
		if (videoFileManager.isOpened()){
			videoFileManager.write(frame);
		}


		//Process GUI and get keypress
		int k = cv::waitKey(1);
		switch(k){
			case 27: { //Exit on ESC press (char 27)
				running = false; 
				break;
			}
			case 'd': {
				applyDarkFrameCalib = !applyDarkFrameCalib;
				std::cout << "Dark Frame Calibration " << (applyDarkFrameCalib ? "On" : "Off") << std::endl;
				break;
			}
			case 'p': { //open properties window
				cam.set(cv::CAP_PROP_SETTINGS,true);
				break;
			}
			case 'r': {
				if (videoFileManager.isOpened()) {
					std::cout << "Recorded " << videoFileManager.getNumRecordedFrames() << " frames as " << videoFileManager.getFileName() << std::endl;
					videoFileManager.close();
				}
				else{
					bool openSuccess = videoFileManager.open(cv::VideoWriter::fourcc('Y','8','0','0'),25,frameSize,false);
					std::cout << (openSuccess ? "Recording..." : "Failed to start recording.") << std::endl;
				}
				break;
			}
			case 'c': {
				recDarkFrame = !recDarkFrame;
				break;
			}
			case 'v': {

				if (!stackFrames.empty()){
					cv::Mat image = stackFrames.back();
					stackFrames.pop_back();
					recordedFrames--;

					if(image.empty()){
						break;
					}
					cv::imshow("Pop",image);
					std::cout << recordedFrames << std::endl;
				}
				else{
					stackFrames.shrink_to_fit();
					cv::destroyWindow("Stack");
					std::cout << "No more images" << std::endl;
				}
				
				break;


			}
			case 'x' : {
				
				cv::Mat completedStack;
				stack(stackFrames,completedStack);
				stackFrames.clear();
				recordedFrames = 0;
				std::cout << recordedFrames << std::endl;
				

				if (completedStack.empty()){
					std::cout << "Nothing to stack" << std::endl;
					break;
				}
	

				cv::Mat stackDisplay;
				completedStack.convertTo(stackDisplay,CV_8UC1);
				cv::imshow("Stack",stackDisplay);


				//Set the new dark frame
				cv::Mat newDarkDSigned;
				completedStack.convertTo(newDarkDSigned,CV_16SC1);
				subtractMean(newDarkDSigned,OffsetDarkSigned);

				break;
			}
			default:
				break;
		}

		//Exit if video window gets closed (Must be checked after waitkey())
		double visible = cv::getWindowProperty(videoWindow,cv::WND_PROP_VISIBLE);
		if (visible == 0) {
			running = false;
		}

	}

	cam.release();
	cv::destroyAllWindows();

	std::cout << "Program Closed" << std::endl;
	return 0;
}



void subtractMean(const cv::Mat & input, cv::Mat & dst){


	cv::Mat meanOut;
	cv::Mat stdDevOut;
	cv::meanStdDev(input,meanOut,stdDevOut);
	double mean = meanOut.at<double>(0);
	//double stdDev = stdDevOut.at<double>(0);


	cv::Mat MeanOffset(input.size(),input.type());
	cv::subtract(input,(int)mean,MeanOffset);

	dst = MeanOffset;

}


void applySignedOffsetDark(const cv::Mat & input, const cv::Mat & signedOffsetDark, cv::Mat & dst){

	cv::Mat inputSigned;
	input.convertTo(inputSigned,CV_16SC1);

	cv::Mat dstSigned;
	cv::subtract(inputSigned,signedOffsetDark,dstSigned);

	dstSigned.convertTo(dst,CV_8UC1);
};

void signedGrayscale16ToRedBlue8(const cv::Mat & signedGrayscale, cv::Mat & rgbOut){

	cv::Size size = signedGrayscale.size();

	cv::Mat Positive(size,CV_16SC1);
	cv::Mat	Negative(size,CV_16SC1);
	
	int threshold = 0;
	int maxValue = 255; //Value threshold uses as max when using binary mode.

	cv::threshold(signedGrayscale,Positive,threshold,maxValue,cv::THRESH_TOZERO);
	cv::threshold(signedGrayscale*-1,Negative,threshold,maxValue,cv::THRESH_TOZERO);

	cv::Mat PositiveDisplay(size,CV_8UC1);
	cv::Mat NegativeDisplay(size,CV_8UC1);
	Positive.convertTo(PositiveDisplay,CV_8UC1);
	Negative.convertTo(NegativeDisplay,CV_8UC1);

	//cv::imshow("Positive",PositiveDisplay);
	//cv::imshow("Negative",NegativeDisplay);


	cv::Mat green(size,CV_8UC1);
	cv::Mat channels[3] = {NegativeDisplay,green,PositiveDisplay};

	cv::merge(channels,3,rgbOut);
}




void stack(std::vector<cv::Mat> imageArray, cv::Mat & dst){

	if (imageArray.empty()){
		return;
	}

	dst = cv::Mat(imageArray.back().size(),CV_64FC1);

	for (cv::Mat &image : imageArray) { //Access by refrence to avoid copying
		cv::Mat image64f;
		image.convertTo(image64f,CV_64FC1);

		cv::add(dst,image64f,dst);
	}

	dst = dst/(double)imageArray.size();



}

