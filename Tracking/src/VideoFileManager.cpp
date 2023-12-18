#include <VideoFileManager.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

VideoFileManager::VideoFileManager(std::string videoDirectory){
	//remove slashes
	char last = videoDirectory.back();
	size_t length = videoDirectory.length();
	while(last == '\\' || last == '/'){
		videoDirectory.erase(length-1);
	}

	this->videoDirectory = videoDirectory;
}

/**
 * @brief Opens a video file with filename "MOV_<index>.avi", using the current index from the video index file `.videoIndex` 
 * If successful, the video index file gets incremented.
 * 
 * @param fourcc four-character code of codec
 * @param fps frames per second of the video stream
 * @param videoDimensions resolution of the video stream
 * @param isColor whether the video is color, or grayscale.
 * @param openedFileName std::string refrence to be set to the generated filename.
 * @return `true` on success, `false` if the video file could not be opened.
 * 
 */
bool VideoFileManager::open(const int fourcc, const int fps, const cv::Size videoDimensions,bool isColor){


	if (!std::filesystem::directory_entry(videoDirectory).exists()){
		if (!std::filesystem::create_directory(videoDirectory)){
			return false;
		}
	}

	int n = getVideoNumber();
	std::string vidTitle = videoDirectory + "\\MOV_" + std::to_string(n) + ".avi";
	videoWriter.open(vidTitle,fourcc,fps,videoDimensions,false);
	if (videoWriter.isOpened()){
		videoFileName = vidTitle;
		numRecordedFrames = 0;
		setVideoNumber(n+1);
		return true;
	}
	return false;
}

bool VideoFileManager::isOpened(){
	return videoWriter.isOpened();
}

void VideoFileManager::close(){
	numRecordedFrames = 0;
	videoFileName.clear();
	videoWriter.release();
	return;
}

void VideoFileManager::write(cv::Mat image){
	videoWriter.write(image);
	numRecordedFrames++;
}

unsigned long long VideoFileManager::getNumRecordedFrames(){
	return numRecordedFrames;
}

std::string VideoFileManager::getFileName(){
	return videoFileName;
}


int VideoFileManager::getVideoNumber(){
	std::ifstream file;
	file.open(videoDirectory+"\\"+videoIndexFilename);
	std::string str;
	std::getline(file,str);
	int out;
	if (str.empty()){
		str = "1";
	}
	try{
		out = std::stoi(str);
	}
	catch(...){
		out = 1;
	}
	file.close();

	return out;
}	


void VideoFileManager::setVideoNumber(int num){
	std::ofstream file;
	file.open(videoDirectory+"\\"+videoIndexFilename,std::ios::trunc); //Clear file when opening
	file << num << std::endl;
	file.close();
	return;
}		