/***********************************************************************
 * FPV-Pi Client.
 * 
 * Author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * Created on 24/07/2015
 * Last modification: 26/07/2015
 * 
 * Language: C/C++
 * 
***********************************************************************/
 
#ifndef FPVPI_CLIENT_CPP_
#define FPVPI_CLIENT_CPP_

#include "client.h" 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

//using namespace cv;

FPVPi_Client::FPVPi_Client()
{
    set_title("FPVPi Client");
//  set_icon_from_file("icon.png");
    set_border_width(10);
    set_default_size(1280, 720);
    set_position(Gtk::WIN_POS_CENTER);
    set_resizable(false);

    add(fixed);

    fixed.put(address_label, 700, 13);
    address_label.set_label("Address: ");
    
    fixed.put(port_label, 700, 53);
    port_label.set_label("Port: ");
    
    fixed.put(connect_button, 700, 100);
    connect_button.set_label("Connect");
    connect_button.signal_clicked().connect(sigc::mem_fun(*this, &FPVPi_Client::Connect));

    fixed.put(disconnect_button, 850, 100);
    disconnect_button.set_label("Disconnect");
    disconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &FPVPi_Client::Disconnect));

    fixed.put(address_entry, 770, 10);
    address_entry.set_text("192.168.0.");
    
    fixed.put(port_entry, 770, 50);
    port_entry.set_text("8080");
    
    fixed.put(main_display_image, 10, 7);
    main_display_image.set("black.png");

    connected = false;

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &FPVPi_Client::timer), 30);

    show_all_children();
}

FPVPi_Client::~FPVPi_Client()
{
    
}

void FPVPi_Client::Connect()
{
    if (!connected)
    {
        const char* serverIP = address_entry.get_text().data();
        int serverPort;
        
        std::istringstream ss(port_entry.get_text());
        ss >> serverPort;

        struct sockaddr_in serverAddr;
        socklen_t addrLen = sizeof(struct sockaddr_in);

        serverIP  = "127.0.0.1";
        serverPort = 8080;

        if ((this->sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
            this->Error("Erro 2!");
            connected = false;
        }

        serverAddr.sin_family = PF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(serverIP);
        serverAddr.sin_port = htons(serverPort);

        if (connect(this->sokt, (sockaddr*)&serverAddr, addrLen) < 0)
        {
            this->Error("Erro 3!");
            connected = false;
        }
        else
        {
            img = Mat::zeros(480 , 640, CV_8U);
            iptr = img.data;
            bytes = 0;

            if (!img.isContinuous())
                img = img.clone();
            connected = true;
        }
    }
}

void FPVPi_Client::Disconnect()
{
    if (connected)
    {
        //close(sokt);
        connected = false;
    }
}

void FPVPi_Client::ReceiveImage()
{
    if ((bytes = recv(sokt, iptr, img.total()*img.elemSize(), MSG_WAITALL)) == -1)
        Error("Erro 1!");
    else
    {    
        cvtColor(img, img, CV_GRAY2RGB);
        
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data(
                                            (guint8*) img.data,
                                            Gdk::COLORSPACE_RGB,
                                            false,
                                            8,
                                            img.cols,
                                            img.rows,
                                            img.step);
        main_display_image.set(pixbuf);
    }
}

void FPVPi_Client::Error(const char* error)
{
    Gtk::MessageDialog dialog(*this, error, false, Gtk::MESSAGE_ERROR,
                              Gtk::BUTTONS_OK);
    dialog.run();
}

bool FPVPi_Client::timer()
{
    if (connected)
        ReceiveImage();

    return true;
}

#endif
