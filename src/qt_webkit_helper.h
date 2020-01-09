#ifndef WEBPAGEWORKER_WEBPAGE_H
#define WEBPAGEWORKER_WEBPAGE_H

#include <iostream>
#include <memory>
#include <QApplication>
#include <QWebPage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebFrame>
#include <QWebElement>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QtCore>
#include "request.h"
#include "response.h"

namespace http {
namespace qt_helper {

/*
 * NetworkAccessManager
 */
class NetworkAccessManager: public QNetworkAccessManager {
	Q_OBJECT

public:
    NetworkAccessManager(http::response::Response& response, QObject *parent = 0);

protected:
    QNetworkReply* createRequest(QNetworkAccessManager::Operation op,
                                 const QNetworkRequest& req,
                                 QIODevice *data);
private slots:
    void replyFinished(QNetworkReply *reply);

private:
    http::response::Response& _response;
};

/*
 * WebPage
 */
class WebPage: public QWebPage {
	Q_OBJECT

public:
    WebPage(QApplication& app,
            http::response::Response& response,
            QObject *parent = 0);

public slots:
    void load(const QString& url);

protected slots:
    void pageLoaded(bool ok);

private:
	std::unique_ptr<NetworkAccessManager> _nam;
    QApplication& _app;
    http::response::Response& _response;
};

}}

#endif
