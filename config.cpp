#include "config.h"
#include <QJsonParseError>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>

#define ELPP_QT_LOGGING
#include "easylogging++.h"

//SHARE_EASYLOGGINGPP(Config::getLogStorage())

Config::Config(QObject *parent) : QObject(parent)
{

}

QJsonDocument Config::parse(const QString& data) {
    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(data.toUtf8(),&err));
    if (err.error) {
        LOG(WARNING)<<"Parse config:"<<err.error<<err.errorString()<<data.toUtf8();
    }
    return doc;
}

bool Config::getConfig(const QString& file, const QString& property, char* codecName) {
    QString info;
    QJsonDocument doc;
    {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly)) {
            LOG(WARNING)<<QString("No access to config file %1.").arg(file);
            return false;
        }
        QTextStream reader(&f);
        if (codecName) reader.setCodec(codecName);
        info = reader.readAll();
        doc = parse(info);
    }
    if (doc.isEmpty()) {
        LOG(WARNING)<<"Config file is empty";
        return false;
    }
    if (!doc.isObject()) {
        LOG(WARNING)<<"Config file is not a JSON object";
        return false;
    }
    if (!property.isEmpty()) {
        if (!doc.object().contains(property)) {
            LOG(WARNING)<<"Config file have not a config property specified";
            return false;
        }
        doc = QJsonDocument(doc.object()[property].toObject());
        if (!doc.isObject()) {
            LOG(WARNING)<<"Config file is not a JSON object";
            return false;
        }
        info = doc.toJson();
    }
    setConfig(info);
    paramsFileName = "params.json";
    if (doc.object().contains("paramsFile")) {
        QString name = doc.object()["paramsFile"].toString();
        if (name.isEmpty()) { LOG(WARNING)<<"Bad paramsFile property in config";
        } else paramsFileName = "params.json";
    }
    {
        QFile par(paramsFileName);
        if (par.open(QIODevice::ReadOnly)) {
            QTextStream reader(&par);
            reader.setCodec("UTF-8");
            QString data = reader.readAll();
            QJsonDocument d = parse(info);
            if (!d.isEmpty() && d.isObject()) {
                setParams(data);
                paramsFile = data;
            }
        }
    }
    return true;
}

void Config::save() {
    if (!paramsFileName.isEmpty() && (paramsData != paramsFile)) {
        QFile par(paramsFileName);
        if (par.open(QIODevice::WriteOnly)) {
            par.write(paramsData.toUtf8());
        } else {
            LOG(WARNING)<<"Could not open params file for write";
        }
    }
}

void Config::commitData(QSessionManager& manager) {
    Q_UNUSED(manager);
    qDebug()<<"Call commitData";
    save();
}
