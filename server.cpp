/**
 * OpenCV video streaming over TCP/IP
 * Server: Captures video from a webcam and send it to a client
 * by Isaac Maia
 */
 
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h> 
#include <string.h>
#include <sstream>

// Navio+ sensors
#include "sensors/MS5611.cpp"

using namespace cv;

std::string getTemp(MS5611);
std::string getPress(MS5611);

int main(int argc, char** argv)
{	 
    //--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //--------------------------------------------------------
    int localSocket;
    int remoteSocket;
    int port = 8080;

    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
           
    int addrLen = sizeof(struct sockaddr_in);
       
    if ((argc > 1) && (strcmp(argv[1],"-h") == 0))
    {
        std::cerr << "usage: ./cv_video_srv [port] [capture device]\n" <<
                   "port           : socket port (4097 default)\n" <<
                   "capture device : (0 default)\n" << std::endl;
        exit(1);
    }

    if (argc == 2)
		port = atoi(argv[1]);

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (localSocket == -1)
        perror("socket() call failed!!");

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port);

    if(bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0)
    {
        perror("Can't bind() socket");
        exit(1);
    }

    //Listening
    listen(localSocket , 1);

    std::cout << "Waiting for connections...\n"
              << "Server Port:" << port << std::endl;

    //accept connection from an incoming client
    remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);
    if (remoteSocket < 0)
    {
        perror("accept failed!");
        exit(1);
    }
    std::cout << "Connection accepted" << std::endl;

    //----------------------------------------------------------
    //OpenCV Code
    //----------------------------------------------------------
    int capDev = 0;
    if (argc == 3) capDev = atoi(argv[2]);

    VideoCapture cap(capDev); // open the default camera
    Mat img, imgGray;
    img = Mat::zeros(480 , 640, CV_8U);    

    //make it continuous
    if (!img.isContinuous())
        img = img.clone();

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;

    //make img continuos
    if (!img.isContinuous())
    {
          img = img.clone();
          imgGray = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;

	VideoWriter videoOut;

	videoOut.open("rec.avi", CV_FOURCC('P','I','M','1'), 24, Size(640,480), true);			// File name, MPEG-1, 25 fps, 640x480, isColor = true
    
    MS5611 barometer;
    barometer.initialize();
    
    while(1)
    {
        /* get a frame from camera */
        cap >> img;

        //do video processing here 
        cvtColor(img, imgGray, CV_BGR2GRAY);

		videoOut << imgGray;
		
        putText(imgGray, getTemp(barometer), Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
		putText(imgGray, getPress(barometer), Point(300,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);

        //send processed image
        if ((bytes = send(remoteSocket, imgGray.data, imgSize, 0)) < 0)
        {
             std::cerr << "bytes = " << bytes << std::endl;
             break;
        } 
    }

	videoOut.release();
    close(remoteSocket);

    return 0;
}

std::string getTemp(MS5611 barometer)
{
	barometer.refreshTemperature();
	usleep(10000); // Waiting for temperature data ready
	barometer.readTemperature();

	barometer.calculatePressureAndTemperature();
	
	std::ostringstream ss;
	ss << barometer.getTemperature();
	
	return ss.str();
}

std::string getPress(MS5611 barometer)
{
	barometer.refreshPressure();
	usleep(10000); // Waiting for pressure data ready
	barometer.readPressure();

	barometer.calculatePressureAndTemperature();
	
	std::ostringstream ss;
	ss << barometer.getPressure();
	return ss.str();
}
