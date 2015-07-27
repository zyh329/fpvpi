/***********************************************************************
 * FPV-Pi.
 * 
 * Author: Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * Created on 24/07/2015
 * Last modification: 24/07/2015
 * 
 * Language: C/C++
 * 
***********************************************************************/

#include "client.cpp"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    FPVPi_Client client;

    return app->run(client);
}
