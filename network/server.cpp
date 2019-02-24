#include "server.h"

//doc: https://doc.qt.io/qt-5/qtnetwork-programming.html

//test connection with
// nc -v localhost PORT_NUMBER (see server.h)
//Or
// nc -v localhost PORT_NUMBER < path/to/file (could be an image!)

/**
 * @brief Server::Server
 * @param parent
 */
Server::Server(QObject *parent) :
    QTcpServer(parent)
{

}

/**
 * @brief Server::StartServer Start the TCP server. Currently using localhost and the port define in the header
 */
void Server::StartServer()
{
    /*
    * Return false if no server can be created
    * Last parameters is the port
    */
    //if(!this->listen(QHostAddress::Any, PORT_NUMBER))
    if(!this->listen(QHostAddress::Any, PORT_NUMBER))
    {
        qDebug() << "Could not start server. Port maybe already used.";
    }
    else
    {
        qDebug() << "Adress is: " << this->serverAddress().toString() << "\nPort is: " << PORT_NUMBER;
        qDebug() << "\n *** Listening... ***";
    }
}


//void Server::incomingConnection(int socketDescriptor)     //Old Qt's signature, if that crash in the cremin use this one
void Server::incomingConnection(qintptr socketDescriptor)   //Use this for the newer version of QT
{
    qDebug() << socketDescriptor << " A Client is Connecting...";
    TCP_Thread *thread = new TCP_Thread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}
