#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QDebug>
#include<QHostAddress>
#include<QNetworkInterface>
#include<QVariantMap>
MainWindow::MainWindow(int port,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),blocksize(0),id_rooms(0)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    start_room = new QTimer(this);
    if(!server->listen(QHostAddress::Any,port))

    {
        QMessageBox::critical(this,"Error",
                              "Unable to start the server:"+
                              server->errorString());
        server->close();
        return;
    }
    connect(server,&QTcpServer::newConnection,
            this,&MainWindow::slotNewConnection);
    connect(start_room,&QTimer::timeout,this,&MainWindow::startRoom);
    QList<QHostAddress> address = QNetworkInterface::allAddresses();
    for(int i =0;i<address.size();++i)
    {
        ui->textEdit->append(address[i].toString());
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::slotNewConnection()
{
    QTcpSocket* newconnection = server->nextPendingConnection();

    connect(newconnection,&QTcpSocket::disconnected,
            newconnection,&QTcpSocket::deleteLater);
    connect(newconnection,&QTcpSocket::disconnected,
            this,&MainWindow::slotDisconected);
    connect(newconnection,&QTcpSocket::readyRead,
             this,&MainWindow::slotReadClient);
    if(current_room == nullptr)
    {
        current_room = new Room(++id_rooms);
        current_room->add_player(newconnection);
        current_room->sendToClient("black",newconnection);
        //sendToClient(newconnection,"black");
    }
    else
    {
        //sendToClient(newconnection,"white");
        current_room->add_player(newconnection);
        current_room->sendToClient("white",newconnection);
        rooms.push_back(current_room);
        start_room->start(200);
        current_room = nullptr;
    }
    ui->statusBar->showMessage("Комнат создано :" + QString::number(rooms.size()));
}
void MainWindow::sendToClient(QTcpSocket* pSocket,QString str)
{
    QByteArray arrblock;
    QDataStream out(&arrblock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    QJsonObject obj;
    obj["type"] = "color";
    obj["id_room"] = QString::number(id_rooms);
    obj["color"] = str;
    out<<quint16(0)<<obj.toVariantMap();
    out.device()->seek(0);
    out<<quint16(arrblock.size() - sizeof(quint16));
    pSocket->write(arrblock);
}
void MainWindow::slotDisconected()
{
    QTcpSocket* sender_ = qobject_cast<QTcpSocket*>(sender());
    for(int i = 0;i<rooms.size();++i)
    {
        if(rooms[i]->isExist(sender_))
        {
            rooms[i]->disconnectAll(sender_);
            rooms.erase(rooms.begin()+i);
            delete rooms[i];
        }
    }
    ui->statusBar->showMessage("Комнат создано :" + QString::number(rooms.size()));
}
void MainWindow::sendToClientData(QTcpSocket *pSocket,const QVariantMap& data)
{
    QByteArray arrblock;
    QDataStream out(&arrblock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    out<<quint16(0)<<data;
    out.device()->seek(0);
    out<<quint16(arrblock.size() - sizeof(quint16));
    pSocket->write(arrblock);
}// перенести всю обработку отправки пакетов в Room?
// так как тогда не будет нарушатся инкапсуляция в классе Room
void MainWindow::startRoom()
{
    auto *arr = rooms[rooms.size()-1];
    arr->start_room();
    start_room->stop();
}
void MainWindow::slotReadClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    QVariantMap map;
    Room *curr_room = nullptr;
    in.setVersion(QDataStream::Qt_5_6);
    if(!blocksize)
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
        {
            return;
        }
        in >> blocksize;
    }
    if(socket->bytesAvailable() < blocksize)
    {
        return;
    }
    blocksize = 0;
    in >> map;
    for(Room *temp : rooms)
    {
        if(temp->get_room_id() == map["id_room"].toInt())
        {
            curr_room = temp;
            break;
        }
    }
    if(map["type"] == "data")
    {
        curr_room->sendToClient(map,curr_room->get_player_to_send(socket));
        /*
        qDebug()<<map["row"].toString()<<map["col"].toString();
        for(Room *temp : rooms)
        {
            //qDebug()<<map["id_room"].toInt();
            if(temp->get_room_id() == map["id_room"].toInt())
            {

                //sendToClientData(temp->get_player_to_send(socket),map);
                break;
            }
        }
        */
    }
    else if(map["type"] == "time_out")
    {
        QVariantMap req;
        req["type"] = "responce";
        curr_room->sendToClient(req,socket);
    }
}
