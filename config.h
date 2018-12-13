#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QJsonDocument>
#include <QSessionManager>

class Config : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString config READ config WRITE setConfig NOTIFY configChanged)
    Q_PROPERTY(QString params READ params WRITE setParams NOTIFY paramsChanged)
public:
    explicit Config(QObject *parent = 0);
    static QJsonDocument parse(const QString& data);
    bool getConfig(const QString& file, const QString& property, char* codecName=0);

    QString config() const { return configData; }
    QString params() const { return paramsData; }
    void setConfig(const QString& c) {
        if (c!=configData) {
            configData=c;
            emit configChanged();
        }
    }
    void setParams(const QString& p) {
        if (p!=paramsData) {
            paramsData=p;
            emit paramsChanged();
        }
    }

//    static el::base::type::StoragePointer getLogStorage();

signals:
    void configChanged();
    void paramsChanged();

public slots:
    void save();
    void commitData(QSessionManager& manager);
private:
    QString configData;
    QString paramsData;
    QString paramsFile;
    QString paramsFileName;
};

#endif // CONFIG_H
