#include <opencv2/videoio.hpp>    //cv::VideoWriter
#include <opencv2/core/mat.hpp>   //cv::Mat
#include <opencv2/core/types.hpp> //cv::Size

#include <string>


class VideoFileManager{

	private:
		cv::VideoWriter videoWriter;

		std::string videoDirectory;
		const std::string videoIndexFilename = ".videoIndex";

		std::string videoFileName;
		unsigned long long numRecordedFrames = 0;

		int getVideoNumber();
		void setVideoNumber(const int num); 

	public:
		VideoFileManager(const std::string videoDir);

		bool isOpened(void);
		bool open(const int fourcc, const int fps, const cv::Size videoDimensions,bool isColor);
		void close();
		void write(const cv::Mat image);

		unsigned long long getNumRecordedFrames();
		std::string getFileName();

















};