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

using namespace cv;

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
 
    while(1)
    {
        cap >> img;
        
        resize(img, img, Size(640, 480));

        // Video processing 
        cvtColor(img, imgGray, CV_BGR2GRAY);

		videoOut << imgGray;

        // Send processed image
        if ((bytes = send(remoteSocket, imgGray.data, imgSize, 0)) < 0)
             break;
    }

	videoOut.release();
    close(remoteSocket);

    return 0;
}
