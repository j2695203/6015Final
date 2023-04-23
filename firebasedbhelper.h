#ifndef FIREBASEDBHELPER_H
#define FIREBASEDBHELPER_H

#include <QDebug>
#include <QFuture>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <user.h>
//ewqeqw
class FirebaseDBHelper : public QObject
{
    Q_OBJECT
public:
    explicit FirebaseDBHelper(QObject *parent = nullptr);
    ~FirebaseDBHelper() { this->networkManager->deleteLater(); }

    QNetworkAccessManager *getNetworkManager() { return networkManager; }
    void uploadToDatabase(User *user);
    QList<int> sortRankScore();

    void setScore(User *user, int score);

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *networkReply;

signals:
};

#endif // FIREBASEDBHELPER_H