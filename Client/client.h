/***********************************************************************
 * FPV-Pi Client.
 * 
 * Author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * Created on 24/07/2015
 * Last modification: 24/07/2015
 * 
 * Language: C/C++
 * 
***********************************************************************/

#ifndef FPVPI_CLIENT_H_
#define FPVPI_CLIENT_H_

#include <gtkmm.h>
#include <map>
#include <opencv2/opencv.hpp>

using namespace cv;

class FPVPi_Client: public Gtk::Window
{
        int sokt;
        bool connected;
        Mat img;
        uchar *iptr;
        int bytes;
    public:
        FPVPi_Client();
        virtual ~FPVPi_Client();
    protected:
        //Signal handlers:
        void Connect();
        void Disconnect();
        void ReceiveImage();
        void Error(const char*);
        bool timer();

        // Member widgets:
        Gtk::Fixed fixed;
            // Labels
        Gtk::Label address_label;
        Gtk::Label port_label;
            // Buttons
        Gtk::Button connect_button;
        Gtk::Button disconnect_button;
            // Entries
        Gtk::Entry address_entry;
        Gtk::Entry port_entry;
            // Image
        Gtk::Image main_display_image;
};

#endif
