#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QTimer>
#include"room.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int port,QWidget *parent = 0);
    ~MainWindow();
public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
    void slotDisconected();
    void startRoom();
private:
    void sendToClient(QTcpSocket* pSocket,QString str);
    void sendToClientData(QTcpSocket* pSocket, const QVariantMap& data);

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    quint16 blocksize;
    size_t id_rooms;
    Room *current_room = nullptr;
    std::vector<Room*> rooms;
    QList<QTcpSocket*> sockets; // заменить на vector<Room*> rooms
    int num_of_players = 0;
    QTimer* start_room;
};

#endif // MAINWINDOW_H
