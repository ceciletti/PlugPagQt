#include "plugpagqt.h"
#include "plugpagqt_p.h"

#include <QMetaObject>
#include <QSettings>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif

#include <QLoggingCategory>

static PlugPagQt *ppInstance = nullptr;
static PlugPagQtPrivate *ppInstanceP = nullptr;

#ifdef Q_OS_ANDROID
inline QVariantHash jResultToQVariantHash(const QAndroidJniObject &resultObj)
{
    QVariantHash resultHash;
    resultHash.insert(QStringLiteral("result"), static_cast<int>(resultObj.callMethod<jint>("getResult")));
    resultHash.insert(QStringLiteral("bin"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getBin")).toString());
    resultHash.insert(QStringLiteral("cardBrand"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getCardBrand")).toString());
    resultHash.insert(QStringLiteral("date"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getDate")).toString());
    resultHash.insert(QStringLiteral("time"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getTime")).toString());
    resultHash.insert(QStringLiteral("holder"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getHolder")).toString());
    resultHash.insert(QStringLiteral("hostNsu"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getHostNsu")).toString());
    resultHash.insert(QStringLiteral("message"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getMessage")).toString());
    resultHash.insert(QStringLiteral("terminalSerialNumber"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getTerminalSerialNumber")).toString());
    resultHash.insert(QStringLiteral("userReference"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getUserReference")).toString());
    resultHash.insert(QStringLiteral("transactionCode"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getTransactionCode")).toString());
    resultHash.insert(QStringLiteral("transactionid"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getTransactionId")).toString());
    resultHash.insert(QStringLiteral("errorCode"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getErrorCode")).toString());
    resultHash.insert(QStringLiteral("amount"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getAmount")).toString());
    resultHash.insert(QStringLiteral("availableBalance"), QAndroidJniObject(resultObj.callObjectMethod<jstring>("getAvailableBalance")).toString());
    return resultHash;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_br_com_ceciletti_PlugPagQt_PlugPagNatives_sendInitted(JNIEnv *env, jobject obj, jboolean authenticated, jstring version)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    qDebug() << "sendInitted";
    QMetaObject::invokeMethod(ppInstanceP, "sendInitted", Qt::QueuedConnection, Q_ARG(QString, QAndroidJniObject(version).toString()));
    QMetaObject::invokeMethod(ppInstanceP, "sendAuthenticated", Qt::QueuedConnection, Q_ARG(bool, authenticated));
}

JNIEXPORT void JNICALL
Java_br_com_ceciletti_PlugPagQt_PlugPagNatives_sendInittedBt(JNIEnv *env, jobject obj, jint code)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    qDebug() << "sendInittedBt" << code;
    QMetaObject::invokeMethod(ppInstanceP, "sendInittedBt", Qt::QueuedConnection, Q_ARG(int, code));
}

JNIEXPORT void JNICALL
Java_br_com_ceciletti_PlugPagQt_PlugPagNatives_sendAuthenticationSuccess(JNIEnv *env, jobject obj)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    qDebug() << "sendAuthenticationSuccess";
    QMetaObject::invokeMethod(ppInstanceP, "sendAuthenticated", Qt::QueuedConnection, Q_ARG(bool, true));
}

JNIEXPORT void JNICALL
Java_br_com_ceciletti_PlugPagQt_PlugPagNatives_sendTransactionResult(JNIEnv *env, jobject obj, jobject result)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    qDebug() << "sendTransactionResult";
    QVariantHash resultHash;
    QAndroidJniObject resultObj(result);
    if (resultObj.isValid()) {
        resultHash = jResultToQVariantHash(resultObj);
    }

    qDebug() << "sendTransactionResult" << resultHash;
    QMetaObject::invokeMethod(ppInstanceP, "sendTransactionResult", Qt::QueuedConnection, Q_ARG(QVariantHash, resultHash));
}

JNIEXPORT void JNICALL
Java_br_com_ceciletti_PlugPagQt_PlugPagNatives_sendEvent(JNIEnv *env, jobject obj, jint code, jstring message)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    qDebug() << "sendEvent" << code;
    QMetaObject::invokeMethod(ppInstance, "eventReceived", Qt::QueuedConnection,
                              Q_ARG(int, code),
                              Q_ARG(QString, QAndroidJniObject(message).toString()));
}

#ifdef __cplusplus
}
#endif

#endif // Q_OS_ANDROID

PlugPagQt::PlugPagQt(QObject *parent) : QObject(parent), d_ptr(new PlugPagQtPrivate(this, this))
{
    ppInstance = this;
    ppInstanceP = d_ptr;

    QSettings settings;
    settings.beginGroup(QStringLiteral("Config"));
    d_ptr->lastDevice = settings.value(QStringLiteral("macOrName")).toString();
    settings.endGroup();
}

QString PlugPagQt::lastDevice() const
{
    Q_D(const PlugPagQt);
    return d->lastDevice;
}

QString PlugPagQt::version() const
{
    Q_D(const PlugPagQt);
    return d->version;
}

bool PlugPagQt::authenticated() const
{
    Q_D(const PlugPagQt);
    return d->authenticated;
}

bool PlugPagQt::initted() const
{
    Q_D(const PlugPagQt);
    return d->initted;
}

bool PlugPagQt::inittedBt() const
{
    Q_D(const PlugPagQt);
    return d->inittedBt;
}

bool PlugPagQt::paying() const
{
    Q_D(const PlugPagQt);
    return d->paying;
}

void PlugPagQt::init(const QString &applicationName, const QString &applicationVersion)
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    d->plugPag.callMethod<void>("setup",
                                "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
                                QtAndroid::androidActivity().object(),
                                QAndroidJniObject::fromString(applicationName).object(),
                                QAndroidJniObject::fromString(applicationVersion).object());
#endif
}

void PlugPagQt::authenticate()
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Authenticate";
    d->plugPag.callMethod<void>("authenticate");
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Authenticate exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
#endif
}

void PlugPagQt::logout()
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Logout";
    d->plugPag.callMethod<void>("logout");
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Logout exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    d->authenticated = false;
    Q_EMIT authenticatedChanged();
#endif
}

void PlugPagQt::initBt(const QString &macOrName)
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Init BT..." << macOrName;
    QSettings settings;
    settings.beginGroup(QStringLiteral("Config"));
    settings.setValue(QStringLiteral("macOrName"), macOrName);
    d->lastDevice = macOrName;
    settings.endGroup();

    d->plugPag.callMethod<void>("initBt",
                                "(Ljava/lang/String;)V",
                                QAndroidJniObject::fromString(macOrName).object());
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Init BT exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    qDebug() << "Init BT";
#endif
}

void PlugPagQt::startPaymentDebt(int value)
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Start Payment Debt..." << value;
    d->paying = true;
    Q_EMIT payingChanged();

    d->plugPag.callMethod<void>("startPaymentDebt",
                                "(I)V",
                                value);
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Payment Debt exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    qDebug() << "End Payment Debt...";
#endif
}

void PlugPagQt::startPaymentCredit(int value)
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Start Payment Credit..." << value;
    d->paying = true;
    Q_EMIT payingChanged();

    d->plugPag.callMethod<void>("startPaymentCredit",
                                "(I)V",
                                value);
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Payment Credit exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    qDebug() << "End Payment Credit...";
#endif
}

void PlugPagQt::abort()
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Abort";
    d->plugPag.callMethod<void>("abort");
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Abort exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
#endif
}

void PlugPagQt::voidLastPayment()
{
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "Void last Payment...";
    d->paying = true;
    Q_EMIT payingChanged();

    d->plugPag.callMethod<void>("voidLastPayment");
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Payment Credit exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    qDebug() << "End void last payment...";
#endif
}

QVariantHash PlugPagQt::getLastApprovedTransaction()
{
    QVariantHash ret;
#ifdef Q_OS_ANDROID
    Q_D(PlugPagQt);
    qDebug() << "getLastApprovedTransaction";
    QAndroidJniObject result = d->plugPag.callObjectMethod("getLastApprovedTransaction", "(V)Lbr/com/uol/pagseguro/plugpag.PlugPagTransactionResult;");

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "getLastApprovedTransaction exception";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }

    if (result.isValid()) {
        ret = jResultToQVariantHash(result);
    }

    qDebug() << "getLastApprovedTransaction" << result.isValid() << ret;
#endif
    return ret;
}

void PlugPagQtPrivate::sendInitted(const QString libVersion)
{
    Q_Q(PlugPagQt);
    initted = true;
    version = libVersion;
    Q_EMIT q->inittedChanged();
}

void PlugPagQtPrivate::sendInittedBt(int code)
{
    Q_Q(PlugPagQt);
    inittedBt = code == 0;
    Q_EMIT q->inittedBtChanged();
}

void PlugPagQtPrivate::sendAuthenticated(bool ok)
{
    Q_Q(PlugPagQt);
    authenticated = ok;
    Q_EMIT q->authenticatedChanged();
}

void PlugPagQtPrivate::sendTransactionResult(const QVariantHash &result)
{
    Q_Q(PlugPagQt);
    paying = false;
    Q_EMIT q->payingChanged();
    Q_EMIT q->transactionResult(result);
}
