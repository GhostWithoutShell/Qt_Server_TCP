#ifndef ROOM_H
#define ROOM_H
#include<vector>
#include<QTcpSocket>
#include<QVariantMap>
class Room
{
public:
    explicit Room(size_t _id_room);
    ~Room();
    void add_player(QTcpSocket* player);
    size_t get_room_id()const;
    QTcpSocket* get_player_to_send(QTcpSocket* player);
    QTcpSocket* get_current_player(QTcpSocket* player);
    bool isExist(QTcpSocket* socket);
    void disconnectAll(QTcpSocket* socket);
    void start_room();
    void sendToClient(const QVariantMap& , QTcpSocket*);
    void sendToClient(const QString& , QTcpSocket*);
private:
    size_t id_room;
    std::vector<QTcpSocket*> players;
};

#endif // ROOM_H
