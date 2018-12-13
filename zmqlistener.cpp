#include "zmqlistener.h"
#include <QTimer>
#include <zmq.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

#define ELPP_QT_LOGGING
#include "easylogging++.h"

int zmqType(const QString& t) {
    if (t.toLower()=="sub") return ZMQ_SUB;
    if (t.toLower()=="pub") return ZMQ_PUB;
    if (t.toLower()=="req") return ZMQ_REQ;
    if (t.toLower()=="rep") return ZMQ_REP;
    if (t.toLower()=="dealer") return ZMQ_DEALER;
    if (t.toLower()=="router") return ZMQ_ROUTER;
    if (t.toLower()=="push") return ZMQ_PUSH;
    if (t.toLower()=="pull") return ZMQ_PULL;
    if (t.toLower()=="pair") return ZMQ_PAIR;
    return -1;
}

int optionType(const QString& t) {
    if (t.toLower()=="sndhwm") return ZMQ_SNDHWM;
    if (t.toLower()=="rcvhwm") return ZMQ_RCVHWM;
    if (t.toLower()=="affinity") return ZMQ_AFFINITY;
    if (t.toLower()=="identity") return ZMQ_IDENTITY;
    if (t.toLower()=="subscribe") return ZMQ_SUBSCRIBE;
    if (t.toLower()=="rate") return ZMQ_RATE;
    if (t.toLower()=="recovery_ivl") return ZMQ_RECOVERY_IVL;
    if (t.toLower()=="sndbuf") return ZMQ_SNDBUF;
    if (t.toLower()=="rcvbuf") return ZMQ_RCVBUF;
    return -1;
}

enum {optInteger,optString, optBoolean, optUnknown};
int optParamType(int t) {
    switch(t) {
        case ZMQ_SNDHWM: case ZMQ_RCVHWM: case ZMQ_AFFINITY: case ZMQ_RATE: case ZMQ_RECOVERY_IVL:
        case ZMQ_SNDBUF: case ZMQ_RCVBUF: return optInteger;

        case ZMQ_IDENTITY: case ZMQ_SUBSCRIBE: return optString;
    }
    return optUnknown;
}


/*
ZMQListener::ZMQListener()
{

}

void ZMQListener::AddListener(void* socket, const QString& sname) {
    int no = sockets.size();
    sockets.append(socket);
    name[socket]=sname;
    if (no==0) QTimer::singleShot(50,this,SLOT(poll()));
}

void ZMQListener::RemoveListener(void* socket) {
    int idx = sockets.indexOf(socket);
    if (idx<0) return;
    sockets.remove(idx);
    name.remove(socket);
}

void ZMQListener::RemoveListener(const QString& socket) { RemoveListener(name.key(socket,0));
}

void ZMQListener::poll() {
    if (sockets.size()==0) return;
    QVector<zmq_pollitem_t> pi(sockets.size());
    int i=0;
    for(zmq_pollitem_t& it:pi) {
        it.socket=sockets.at(i++);
        it.events=ZMQ_POLLIN;
        it.revents = 0;
    }
    int ret = zmq_poll(pi.data(),pi.size(),500);
    if (ret!=-1) {
        for(zmq_pollitem_t& it:pi) {
            if (ret==0) break;
            if (it.revents & ZMQ_POLLIN) {
                for(int res=0;res>=0;) {
                    QByteArray d(2048,0);
                    res = zmq_recv(it.socket,d.data(),2048,ZMQ_DONTWAIT);
                    if (res>=0) {
                        d.truncate(res);
                        emit recv(name[it.socket],QString::fromUtf8(d));
                    }
                }
                ret--;
            }
        }
    }
    QTimer::singleShot(50,this,SLOT(poll()));
}


ZMQConstruct::ZMQConstruct(ZMQListener* l):listener(l),ctx(zmq_ctx_new()) {}

ZMQConstruct::~ZMQConstruct() {
    if (sockets.size()>0) cleanupConnections();
    zmq_ctx_term(ctx);
}

bool ZMQConstruct::addJson(QJsonObject& cfg) {
    bool ok = true;
    for(QString key : cfg.keys()) {
        QJsonObject o = cfg[key].toObject();
        void* ret = createConnection(key,o);
        if (!ret) {
            ok = false;
            continue;
        }
        sockets[key]=ret;
    }
    return ok;
}


void* ZMQConstruct::createConnection(const QString& name, QJsonObject& cfg) {
    if (!cfg.contains("type")) { qCritical("ZeroMQ connection (%s). Type is not specified.",name.toStdString().c_str()); return 0; }
    if (!(cfg.contains("bind") || cfg.contains("connect"))) { qCritical("ZeroMQ connection (%s). Bind or Connect are not specified.",name.toStdString().c_str()); return 0; }
    int type = zmqType(cfg["type"].toString());
    if (type==-1) { qCritical("ZeroMQ connection (%s). Wrong type '%s'.",name.toStdString().c_str(),cfg["type"].toString().toStdString().c_str());  return 0; }
    void* ret = zmq_socket(ctx,type);
    if (cfg.contains("bind")) {
        auto val = cfg["bind"];
        if (val.isString()) {
            int r = zmq_bind(ret,(char*)val.toString().toLocal8Bit().data());
            if (r) { qCritical("ZeroMQ connection (%s). Bind to %s error: '%s'",name.toStdString().c_str(),val.toString().toStdString().c_str(),zmq_strerror(zmq_errno())); return 0; }
        } else if (val.isArray()) {
            for(auto addr : val.toArray()) {
                if (!addr.isString()) { qCritical("ZeroMQ connection (%s). Wrong address type",name.toStdString().c_str()); return 0; }
                int r = zmq_bind(ret,(char*)addr.toString().toLocal8Bit().data());
                if (r) { qCritical("ZeroMQ connection (%s). Bind to %s error: '%s'",name.toStdString().c_str(),addr.toString().toStdString().c_str(),zmq_strerror(zmq_errno())); return 0; }
            }
        } else { qCritical("ZeroMQ connection (%s). Wrong bind address type",name.toStdString().c_str()); return 0; }
    }
    if (cfg.contains("connect")) {
        auto val = cfg["connect"];
        if (val.isString()) {
            int r = zmq_connect(ret,(char*)val.toString().toLocal8Bit().data());
            if (r) { qCritical("ZeroMQ connection (%s). Connect to %s error: '%s'",name.toStdString().c_str(),val.toString().toStdString().c_str(),zmq_strerror(zmq_errno())); return 0; }
        } else if (val.isArray()) {
            for(auto addr : val.toArray()) {
                if (!addr.isString()) { qCritical("ZeroMQ connection (%s). Wrong address type",name.toStdString().c_str()); return 0; }
                int r = zmq_connect(ret,(char*)addr.toString().toLocal8Bit().data());
                if (r) { qCritical("ZeroMQ connection (%s). Connect to %s error: '%s'",name.toStdString().c_str(),addr.toString().toStdString().c_str(),zmq_strerror(zmq_errno())); return 0; }
            }
        } else { qCritical("ZeroMQ connection (%s). Wrong connect address type",name.toStdString().c_str()); return 0; }
    }
    if (cfg.contains("option")) {
        if (cfg["option"].isObject()) {
            for(auto opt : cfg["option"].toObject().keys()) {
                auto val = cfg["option"].toObject()[opt];
                int t = optionType(opt);
                if (t==-1) { qWarning("ZeroMQ connection (%s). Unknown option type %s.",name.toStdString().c_str(),opt.toStdString().c_str());
                } else {
                    int ot = optParamType(t);
                    if ((ot==optInteger) && !val.isDouble()) {
                        qWarning("ZeroMQ connection (%s). Option %s must be integer",name.toStdString().c_str(),opt.toStdString().c_str());
                        continue;
                    }
                    if ((ot==optString) && !val.isString()) {
                        qWarning("ZeroMQ connection (%s). Option %s must be string",name.toStdString().c_str(),opt.toStdString().c_str());
                        continue;
                    }
                    if (val.isDouble()) {
                        int v = val.toInt();
                        zmq_setsockopt(ret,t,&v,sizeof(v));
                    }
                    if (val.isString()) {
                        QByteArray v = val.toString().toLatin1();
                        zmq_setsockopt(ret,t,v.data(),v.size());
                    }
                }
            }
        } else {
            qWarning("ZeroMQ connection (%s). Wrong option type.",name.toStdString().c_str());
        }
    }
    if (listener && (type!=ZMQ_PUB)) listener->AddListener(ret,name);
    return ret;
}


void ZMQConstruct::cleanupConnections() {
    for (auto key : sockets.keys()) { zmq_close(sockets[key]);
    }
}
*/
ZMQ::ZMQ():ctx(0) {}
void ZMQ::configure(const QVariantMap& obj) {
    qDebug()<<obj;
    cleanup();
    ctx = zmq_ctx_new();
    const QVariant var = obj["sockets"];
    if (var.type()!=QVariant::Type::Map) {
        LOG(ERROR)<<"Wrong ZeroMQ sockets configuration";
        emit errConfigure(1,"Wrong ZeroMQ sockets configuration");
        return;
    }
    const QVariantMap socks = var.toMap();
    bool ok = true;
    for(QString key : socks.keys()) {
        if (socks[key].type()!=QVariant::Type::Map) {
            LOG(ERROR)<<"Wrong ZeroMQ socket "<< key << " configuration";
            emit errConfigure(2,QString("Wrong ZeroMQ socket %1 configuration").arg(key));
            ok = false;
            break;
        }
        void* ret = createConnection(key,socks[key].toMap());
        if (!ret) {
            ok = false;
            break;
        }
        sockets[key]=ret;
    }
    if (ok) {
        QTimer::singleShot(50,this,SLOT(poll()));
        emit okConfigure();
    } else { cleanup();
    }
}

void ZMQ::cleanup() {
    if (ctx) {
        for (auto key : sockets.keys()) { zmq_close(sockets[key]);
        }
        zmq_ctx_term(ctx);
        ctx=0;
    }
}

void* ZMQ::createConnection(const QString& name, const QVariantMap& cfg) {
    void* ret = 0;
    QString err;
    int err_no = 0;
    do {
        int type = zmqType(cfg["type"].toString());
        if (type==-1) {
            err = QString("Wrong type '%1'.").arg(cfg["type"].toString());
            err_no=3;
            break;
        }
        if (!(cfg.contains("bind") || cfg.contains("connect"))) {
            err = "Bind or Connect are not specified.";
            err_no=4;
            break;
        }
        ret = zmq_socket(ctx,type);
        if (cfg.contains("bind")) {
            const QVariant val = cfg["bind"];
            if (val.type()==QVariant::Type::String) {
                int r = zmq_bind(ret,(char*)val.toString().toLocal8Bit().data());
                if (r) {
                    err_no = zmq_errno();
                    err = QString("Bind to %1 error: '%2'").arg(val.toString(),zmq_strerror(err_no));
                }
            } else if (val.type()==QVariant::Type::List) {
                for(const QVariant addr : val.toList()) {
                    if (addr.type()!=QVariant::Type::String) {
                        err = "Wrong address type";
                        err_no=5;
                        break;
                    }
                    int r = zmq_bind(ret,(char*)addr.toString().toLocal8Bit().data());
                    if (r) {
                        err_no=zmq_errno();
                        err=QString("Bind to %1 error: '%2'").arg(addr.toString(),zmq_strerror(err_no));
                        break;
                    }
                }
            } else if (val.type()==QVariant::Type::StringList) {
                for(const QString addr : val.toStringList()) {
                    int r = zmq_bind(ret,(char*)addr.toLocal8Bit().data());
                    if (r) {
                        err_no=zmq_errno();
                        err=QString("Bind to %1 error: '%2'").arg(addr,zmq_strerror(err_no));
                        break;
                    }
                }
            } else {
                err="Wrong bind address type";
                err_no=6;
            }
            if (err_no) break;
        }
        if (cfg.contains("connect")) {
            const QVariant val = cfg["connect"];
            if (val.type()==QVariant::Type::String) {
                int r = zmq_connect(ret,(char*)val.toString().toLocal8Bit().data());
                if (r) {
                    err_no=zmq_errno();
                    err=QString("Connect to %1 error: '%2'").arg(val.toString(),zmq_strerror(err_no));
                }
            } else if (val.type()==QVariant::Type::List) {
                for(const QVariant addr : val.toList()) {
                    if (addr.type()!=QVariant::Type::String) {
                        err="Wrong address type";
                        err_no=7;
                        break;
                    }
                    int r = zmq_connect(ret,(char*)addr.toString().toLocal8Bit().data());
                    if (r) {
                        err_no=zmq_errno();
                        err=QString("Connect to %1 error: '%2'").arg(addr.toString(),zmq_strerror(err_no));
                        break;
                    }
                }
            } else if (val.type()==QVariant::Type::StringList) {
                for(const QString addr : val.toStringList()) {
                    int r = zmq_connect(ret,(char*)addr.toLocal8Bit().data());
                    if (r) {
                        err_no=zmq_errno();
                        err=QString("Connect to %1 error: '%2'").arg(addr,zmq_strerror(err_no));
                        break;
                    }
                }
            } else {
                err="Wrong connect address type";
                err_no=8;
            }
            if (err_no) break;
        }
        bool set_filter = false;
        if (cfg.contains("option")) {
            if (cfg["option"].type()==QVariant::Type::Map) {
                for(auto opt : cfg["option"].toMap().keys()) {
                    const QVariant val = cfg["option"].toMap()[opt];
                    int t = optionType(opt);
                    if (t==-1) {
                        LOG(WARNING)<<"ZeroMQ connection ("<<name<<"). Unknown option type "<<opt<<".";
                    } else {
                        int ot = optParamType(t);
                        if ((ot==optInteger) && !val.canConvert(QMetaType::Int)) {
                            LOG(WARNING)<<"ZeroMQ connection ("<<name<<"). Option "<<opt<<" must be integer";
                            continue;
                        }
                        if ((ot==optString) && !val.canConvert(QMetaType::QString)) {
                            LOG(WARNING)<<"ZeroMQ connection ("<<name<<"). Option "<<opt<<" must be string";
                            continue;
                        }
                        if (ot==optInteger) {
                            int v = val.toInt();
                            zmq_setsockopt(ret,t,&v,sizeof(v));
                        }
                        if (ot==optString) {
                            QByteArray v = val.toString().toLatin1();
                            zmq_setsockopt(ret,t,v.data(),v.size());
                            if (t==ZMQ_SUBSCRIBE) set_filter=true;
                        }
                    }
                }
            } else {
                LOG(WARNING)<<"ZeroMQ connection ("<<name<<"). Wrong option type.";
            }
        }
        if ((type==ZMQ_SUB) && !set_filter) { zmq_setsockopt(ret,ZMQ_SUBSCRIBE,0,0);
        }
        bool is_listen = type!=ZMQ_PUB;
        if (cfg.contains("listen")) {
            is_listen = cfg["listen"].toBool();
        }
        if (is_listen) listen.append(ret);
        return ret;
    } while(false);
    if (ret) zmq_close(ret);
    if (!err.isEmpty() || errno) {
        if (!err.isEmpty()) LOG(ERROR)<<QString("ZeroMQ connection (%1).").arg(name)+err;
        emit errConfigure(err_no,err);
    }
    return 0;
}

void ZMQ::addListener(const QString& socket) {
    int no = listen.size();
    if (!sockets.contains(socket)) {
        emit errAdd(1,"No socket name");
        return;
    }
    if (listen.contains(sockets[socket])) return;
    listen.append(sockets[socket]);
    if (no==0) QTimer::singleShot(50,this,SLOT(poll()));
}

void ZMQ::removeListener(const QString& socket) {
    for(int idx = listen.indexOf(sockets[socket]);
        idx!=-1;idx = listen.indexOf(sockets[socket])) {
        listen.remove(idx);
    }
}

void ZMQ::poll() {
    if (listen.size()==0) return;
    QVector<zmq_pollitem_t> pi(listen.size());
    int i=0;
    for(zmq_pollitem_t& it:pi) {
        it.socket=listen.at(i++);
        it.events=ZMQ_POLLIN;
        it.revents = 0;
    }
    int ret = zmq_poll(pi.data(),pi.size(),250);
    if (ret!=-1) {
        for(zmq_pollitem_t& it:pi) {
            if (ret==0) break;
            if (it.revents & ZMQ_POLLIN) {
                for(int res=0;res>=0;) {
                    QByteArray d(2048,0);
                    res = zmq_recv(it.socket,d.data(),2048,ZMQ_DONTWAIT);
                    if (res>=0) {
                        d.truncate(res);
                        QJsonParseError err;
                        QJsonDocument doc(QJsonDocument::fromJson(QString::fromUtf8(d).toUtf8(),&err));
                        if (err.error) { LOG(ERROR)<<"Wrong JSON ("<<err.errorString()<<") in message "<<QString::fromUtf8(d);
                        } else {
                            QString name = sockets.key(it.socket);
                            if (name.isEmpty()) { LOG(ERROR)<<"Wrong socket receives message";
                            } else emit recv(name,doc.toVariant());
                        }

                    }
                }
                ret--;
            }
        }
    }
    QTimer::singleShot(50,this,SLOT(poll()));
}

inline MAKE_LOGGABLE(QVariant,v,os) {
    if (v.canConvert(QMetaType::QString)) { os<<v.toString();
    } else if (v.canConvert(QMetaType::QVariantMap)) {os<<v.toMap();
    } else if (v.canConvert(QMetaType::QVariantList)) {os<<v.toList();
    }
    return os;
}

void ZMQ::send(const QString& socket, const QVariant& data) {
    if (!sockets.contains(socket)) {
        QString err = QString("Unknown socket name %1 in send procedure").arg(socket);
        LOG(ERROR)<<err;
        emit errSend(1,err);
        return;
    }
    QJsonDocument doc = QJsonDocument::fromVariant(data);
    if (doc.isEmpty()) {
        QString err = QString("Can not send empty or not valid JSON from variant %1").arg(data.toString());
        LOG(ERROR)<<"Can not send empty or not valid JSON from variant"<<data;
        emit errSend(2,err);
        return;
    }
    const QByteArray d = doc.toBinaryData();
    int ret = zmq_send(sockets[socket],d.data(),d.length(),0);
    if (ret) {
        QString err = QString("Send JSON error %1").arg(zmq_strerror(zmq_errno()));
        LOG(ERROR)<<err;
        emit errSend(zmq_errno(),zmq_strerror(zmq_errno()));
    }
}

ZMQProxy::ZMQProxy() {};
void ZMQProxy::configure(const QVariantMap& obj) { emit configure_prx(obj);}
void ZMQProxy::addListener(const QString& socket) {emit addListener_prx(socket);}
void ZMQProxy::removeListener(const QString& socket) {emit removeListener_prx(socket);}
void ZMQProxy::send(const QString& socket, const QVariant& data) {emit send_prx(socket,data);}

void ZMQProxy::recv_prx(const QString& socket, const QVariant& data) {emit recv(socket,data);}
void ZMQProxy::errConfigure_prx(int code, const QString& message){emit errConfigure(code,message);}
void ZMQProxy::errSend_prx(int code, const QString& message){emit errSend(code,message);}
void ZMQProxy::errAdd_prx(int code, const QString& message){emit errAdd(code,message);}
void ZMQProxy::okConfigure_prx(){emit okConfigure();}
