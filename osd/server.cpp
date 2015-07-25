/***********************************************************************
 * FPV-Pi.
 * 
 * Author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * Created on 23/07/2015
 * Last modification: 24/07/2015
 * 
 * Language: C/C++
 * 
***********************************************************************/

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
//**********************************************************************
//-- Network code ------------------------------------------------------
//**********************************************************************
    int localSocket;
    int remoteSocket;
    int port = 8080;

    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
           
    int addrLen = sizeof(struct sockaddr_in);

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    
    if (localSocket == -1)
        exit(1);

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port);

    if (bind(localSocket, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
        exit(1);

    // Listening
    listen(localSocket , 1);

    // Accept connection from an incoming client
    remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);
    
    if (remoteSocket < 0)
        exit(1);

//**********************************************************************
//-- OpenCV code -------------------------------------------------------
//**********************************************************************
    VideoCapture cap(0);                                                // "0": Open the default camera
    Mat img, imgGray;
    img = Mat::zeros(480 , 640, CV_8U);    

    if (!img.isContinuous())
        img = img.clone();

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;

    if (!img.isContinuous())
    {
          img = img.clone();
          imgGray = img.clone();
    }

	VideoWriter videoOut;

	videoOut.open("rec.avi", CV_FOURCC('P','I','M','1'),                // File name, MPEG-1, 25 fps, 640x480, isColor = true
                  20, Size(640,480), false);


    MS5611 barometer;
    barometer.initialize();

    while(1)
    {
        cap >> img;
        
        resize(img, img, Size(640, 480));

        // Video processing 
        cvtColor(img, imgGray, CV_BGR2GRAY);
        
        putText(imgGray, getTemp(barometer), Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
		putText(imgGray, getPress(barometer), Point(300,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,255,255), 1, CV_AA);
        
		videoOut << imgGray;

        // Send processed image
        if ((bytes = send(remoteSocket, imgGray.data, imgSize, 0)) < 0)
             break;
    }

	videoOut.release();
    close(remoteSocket);

    return 0;
}

std::string getTemp(MS5611 barometer)
{
	barometer.refreshTemperature();
	usleep(10000);                                                      // Waiting for temperature data ready
	barometer.readTemperature();

	barometer.calculatePressureAndTemperature();
	
	std::ostringstream ss;
	ss << barometer.getTemperature();
	
	return ss.str();
}

std::string getPress(MS5611 barometer)
{
	barometer.refreshPressure();
	usleep(10000);                                                      // Waiting for pressure data ready
	barometer.readPressure();

	barometer.calculatePressureAndTemperature();
	
	std::ostringstream ss;
	ss << barometer.getPressure();
	return ss.str();
}
