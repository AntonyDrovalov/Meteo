#ifndef ZMQLISTENER_H
#define ZMQLISTENER_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QMap>
#include <QVariantMap>
#include <QJsonObject>
/*
class ZMQListener : public QObject
{
    Q_OBJECT
public:
    ZMQListener();
public slots:
    void AddListener(void* socket, const QString& sname);
    void RemoveListener(void* socket);
    void RemoveListener(const QString& socket);
private slots:
    void poll();
signals:
    //void recv(void* socket, const QByteArray& data);
    void recv(const QString& name, const QString& data);
private:
    QVector<void*> sockets;
    QMap<void*,QString> name;

};

class ZMQConstruct {
private:
    void* ctx;
    ZMQListener* listener;
    void cleanupConnections();
    void* createConnection(const QString& name, QJsonObject& cfg);
public:
    ZMQConstruct(ZMQListener* l=0);
    virtual ~ZMQConstruct();

    bool addJson(QJsonObject& cfg);

    QMap<QString,void*> sockets;
};
*/
class ZMQ : public QObject {
    Q_OBJECT
public:
    ZMQ();
signals:
    void recv(const QString& socket, const QVariant& data);
    void errConfigure(int code, const QString& message);
    void errSend(int code, const QString& message);
    void errAdd(int code, const QString& message);
    void okConfigure();
public slots:
    void configure(const QVariantMap& obj);
    void addListener(const QString& socket);
    void removeListener(const QString& socket);
    void send(const QString& socket, const QVariant& data);
private slots:
    void poll();
private:
    void* ctx;
    void cleanup();
    void* createConnection(const QString& name, const QVariantMap& cfg);
    QMap<QString,void*> sockets;
    QVector<void*> listen;
};

class ZMQProxy : public QObject {
    Q_OBJECT
public:
    ZMQProxy();
signals:
    void recv(const QString& socket, const QVariant& data);

    void configure_prx(const QVariantMap& obj);
    void addListener_prx(const QString& socket);
    void removeListener_prx(const QString& socket);
    void errConfigure(int code, const QString& message);
    void errSend(int code, const QString& message);
    void errAdd(int code, const QString& message);
    void okConfigure();

    void send_prx(const QString& socket, const QVariant& data);
public slots:
    void configure(const QVariantMap& obj);
    void addListener(const QString& socket);
    void removeListener(const QString& socket);
    void send(const QString& socket, const QVariant& data);

    void recv_prx(const QString& socket, const QVariant& data);
    void errConfigure_prx(int code, const QString& message);
    void errSend_prx(int code, const QString& message);
    void errAdd_prx(int code, const QString& message);
    void okConfigure_prx();
};


#endif // ZMQLISTENER_H
