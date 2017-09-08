#include "room.h"
#include<QDataStream>
Room::Room(size_t _id_room):id_room(_id_room)
{
    players.reserve(2);
}

Room::~Room()
{
    for(QTcpSocket* player : players)
    {
        //delete player;
    }
    players.clear();
}

void Room::add_player(QTcpSocket *player)
{
    if(players.size() < 2)
    {
        players.push_back(player);
    }
}
size_t Room::get_room_id() const
{
    return id_room;
}
QTcpSocket* Room::get_player_to_send(QTcpSocket *player)
{
    for(int i = 0;i<players.size();++i)
    {
        if(players[i] != player)
        {
            return players[i];
        }
    }
}
QTcpSocket *Room::get_current_player(QTcpSocket *player)
{
    for(QTcpSocket *room : players)
    {
        if(room == player)
        {
            return room;
        }
    }
}

bool Room::isExist(QTcpSocket *socket)
{
    for(QTcpSocket* socket_ : players)
    {
        if(socket_ == socket)
        {
            return true;
        }
    }
    return false;
}

void Room::disconnectAll(QTcpSocket *socket)
{
    for(QTcpSocket* socket_ : players)
    {
        if(socket_ != socket)
        {
            //socket
            socket_->deleteLater();
        }
    }
}

std::vector<QTcpSocket *>& Room::get_players()
{
    return players;
}
void Room::start_room()
{
    QVariantMap start_pack;
    start_pack["type"] = "start";
    for(QTcpSocket* socket: players)
    {
        sendToClient(start_pack,socket);
    }
}
void Room::sendToClient(const QVariantMap& data, QTcpSocket *socket)
{
    QByteArray arrblock;
    QDataStream out(&arrblock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    out<<quint16(0)<<data;
    out.device()->seek(0);
    out<<quint16(arrblock.size() - sizeof(quint16));
    socket->write(arrblock);
}
void Room::sendToClient(const QString& data, QTcpSocket *socket)
{
    QByteArray arrblock;
    QVariantMap map_color;
    map_color["type"] = "color";
    map_color["id_room"] = QString::number(id_room);
    map_color["color"] = data;
    QDataStream out(&arrblock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    out<<quint16(0)<<map_color;
    out.device()->seek(0);
    out<<quint16(arrblock.size() - sizeof(quint16));
    socket->write(arrblock);
}
