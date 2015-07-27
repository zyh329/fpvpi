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
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
 
using namespace cv;

int main(int argc, char** argv)
{
//**********************************************************************
//-- Network code ------------------------------------------------------
//**********************************************************************
    int sokt;
    char* serverIP;
    int serverPort;

    serverIP = argv[1];
    serverPort = atoi(argv[2]);

    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    if ((sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        exit(1);

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0)
        exit(1);

//**********************************************************************
//-- OpenCV code -------------------------------------------------------
//**********************************************************************
    Mat img;
    img = Mat::zeros(480 , 640, CV_8U);    
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key;

    if (!img.isContinuous())
        img = img.clone();

    namedWindow("CV Video Client",1);

    while (key != 'q')
    {
        if ((bytes = recv(sokt, iptr, imgSize , MSG_WAITALL)) == -1)
            exit(1);
        
        cv::imshow("CV Video Client", img); 
      
        if (key = cv::waitKey(10) >= 0)
            break;
    }

    close(sokt);

    return 0;
}
