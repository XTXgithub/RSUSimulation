#ifndef SERVER_H
#define SERVER_H

#include <QtCore>
#include <QtNetwork>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QThread>
#include <QAtomicInteger>
#include <QMap>

class Frame {
public:
    quint16 STX;
    quint8 VER;
    quint8 SEQ;
    quint32 LEN;
    QByteArray DATA;
    quint16 CRC;

    Frame(quint8 seq = 0, const QByteArray& data = QByteArray());
};

class FrameOperations {
public:
    static QByteArray encode(Frame& f);
    static Frame decode(const QByteArray& buf);
    static quint16 calcCRC16(quint16 initCRC, const QByteArray& crcData);
};

class Server : public QThread {
    Q_OBJECT

public:
    static Server& getInstance() {
        static Server instance;
        return instance;
    }

    void startServer(const QString& address, quint16 port);
    void stopServer();

    void setSwitch(bool state);
    bool isSwitchOn();

    void setB2Data(const QString& data);
    void setB3Data(const QString& data);
    void setB4Data(const QString& data);
    void setB5Data(const QString& data);

    QMap<QString, QTcpSocket*> GetMap();

    void sendFrameBasedOnNum(QTcpSocket* conn, quint8 frameNum);

protected:
    void run() override;

private:
    Server();
    ~Server();

    QTcpServer* listener;
    QFile logFile;
    QAtomicInteger<bool> stop;
    QMutex mutex;
    bool switchState;
    QString B2Data;
    QString B3Data;
    QString B4Data;
    QString B5Data;

    QTcpSocket* clientSocket;  // 记录当前连接的客户端
    bool isClientConnected=false;    // 标志当前是否有客户端连接
    quint8 maxClientCount=2;
    quint8 ClientConnectedCount=0;

    QMap<QString, QTcpSocket*> connMap;  // 存储客户端连接的Map

    void handleConnection(QTcpSocket* conn);
    Q_INVOKABLE void sendHeartbeat(QTcpSocket* conn, quint8& seq);
    void sendB0Frame(QTcpSocket* conn, quint8 version);
    void sendB2Frame(QTcpSocket* conn);
    void sendB3Frame(QTcpSocket* conn);
    void sendB4Frame(QTcpSocket* conn);
    void sendB5Frame(QTcpSocket* conn);


    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
};

#endif // SERVER_H