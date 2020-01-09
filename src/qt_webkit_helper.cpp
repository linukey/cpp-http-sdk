#include "qt_webkit_helper.h"

namespace http {
namespace qt_helper {

NetworkAccessManager::NetworkAccessManager(http::response::Response& response, QObject *parent)
    : QNetworkAccessManager(parent),
      _response(response) {
    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
}

void NetworkAccessManager::replyFinished(QNetworkReply *reply) {
    if (_response.StatusCode().empty() || _response.StatusCode() == "301") {
        _response.setStatusCode(std::to_string(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
        _response.setStatusDescribe(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toStdString());
        _response.setProtocol("HTTP/1.1");
        foreach (QByteArray headerName, reply->rawHeaderList()) {
            _response.setHeader(headerName.toStdString(), reply->rawHeader(headerName).toStdString());
        }
    }
}

QNetworkReply* NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op,
                                                   const QNetworkRequest& req,
                                                   QIODevice *data) {
    // 不需要手动指定 accept-encoding : gzip
    // https://code.qt.io/cgit/qt/qtbase.git/tree/src/network/access/qhttpnetworkconnection.cpp?h=5.11#n299
    return QNetworkAccessManager::createRequest(op, req, data);
}

WebPage::WebPage(QApplication& app,
                 http::response::Response& response,
                 QObject *parent)

    : QWebPage(parent),
      _response(response), 
      _app(app) {

    _nam = std::unique_ptr<NetworkAccessManager>(new NetworkAccessManager(_response));
    setNetworkAccessManager(_nam.get());
    connect(mainFrame(), &QWebFrame::loadFinished, this, &WebPage::pageLoaded);
}

void WebPage::load(const QString& url) { 
    mainFrame()->load(QUrl::fromUserInput(url));
}

void WebPage::pageLoaded(bool ok) {
    string& data = _response.setData();
    data = mainFrame()->toHtml().toStdString();
    _app.quit();
}

}}
