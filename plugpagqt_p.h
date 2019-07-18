#ifndef PLUGPAGQT_P_H
#define PLUGPAGQT_P_H

#include <QObject>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

class PlugPagQt;
class PlugPagQtPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(PlugPagQt)
public:
    PlugPagQtPrivate(QObject *parent, PlugPagQt *q) : QObject(parent)
#ifdef Q_OS_ANDROID
      , plugPag("br/com/ceciletti/PlugPagQt/PlugPagQt")
#endif
      , q_ptr(q)
    {

    }

    Q_INVOKABLE void sendInitted(const QString libVersion);
    Q_INVOKABLE void sendInittedBt(int code);
    Q_INVOKABLE void sendAuthenticated(bool ok);
    Q_INVOKABLE void sendTransactionResult(const QVariantHash &result);

#ifdef Q_OS_ANDROID
    QAndroidJniObject plugPag;
#endif
    PlugPagQt *q_ptr;
    QString lastDevice;
    QString version;
    bool authenticated = false;
    bool initted = false;
    bool inittedBt = false;
    bool paying = false;
};

#endif // PLUGPAGQT_P_H
