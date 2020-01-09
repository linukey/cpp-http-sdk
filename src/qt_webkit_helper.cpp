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
    QNetworkRequest request(req);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Linux; Android 5.0; SM-G900P Build/LRX21T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Mobile Safari/537.36");
    return QNetworkAccessManager::createRequest(op, request, data);
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
    _app.quit();
    string& data = _response.setData();
    data = mainFrame()->toHtml().toStdString();
}

}}
