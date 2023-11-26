#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

int main(){

	//Create VideoCapture object, pass 0 to open the first attached camera device.
	cv::VideoCapture cam(0);

	cam.set(cv::CAP_PROP_AUTO_EXPOSURE,0);
	cam.set(cv::CAP_PROP_EXPOSURE,-6);
	cam.set(cv::CAP_PROP_GAIN,100);
	cam.set(cv::CAP_PROP_SATURATION,20);


	//could not get working
	//cam.set(cv::CAP_PROP_AUTO_WB,0);
	//cam.set(cv::CAP_PROP_WB_TEMPERATURE,5000);



	if (!cam.isOpened()){
		std::cout << "Error opening video stream" << std::endl;
	}

	std::string windowTitle = "Video";



	cv::Mat frame;
	//get first frame for determining window size.
	cam >> frame;

	cv::namedWindow(windowTitle,cv::WINDOW_NORMAL | cv::WINDOW_GUI_NORMAL);
	cv::resizeWindow(windowTitle,frame.size() * 2);

	while(true){


		//Get frame from camera
		cam >> frame;

		

		//Exit loop incase camera is unplugged.
		//imshow will throw error for empty frame
		if (frame.empty()){
			break;
		}

		//cv::pyrUp(frame,frame);

		//Display frame
		cv::imshow(windowTitle, frame);

		//Process GUI and get keypress
		int k = cv::waitKey(1);

		//Exit on ESC press (char 27)
		if (k == 27){
			break;
		}

		//Exit if window is closed (Must be checked after waitkey())
		double visible = cv::getWindowProperty(windowTitle,cv::WND_PROP_VISIBLE);
		if (visible == 0) {
			break;
		}	
	}

	cam.release();
	cv::destroyAllWindows();

	return 0;
}