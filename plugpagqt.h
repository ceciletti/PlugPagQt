#ifndef PLUGPAGQT_H
#define PLUGPAGQT_H

#include <QObject>
#include <QVariantHash>

class PlugPagQtPrivate;
class PlugPagQt : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PlugPagQt)
    Q_PROPERTY(QString lastDevice READ lastDevice NOTIFY lastDeviceChanged)
    Q_PROPERTY(QString version READ version NOTIFY inittedChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)
    Q_PROPERTY(bool initted READ initted NOTIFY inittedChanged)
    Q_PROPERTY(bool inittedBt READ inittedBt NOTIFY inittedBtChanged)
    Q_PROPERTY(bool paying READ paying NOTIFY payingChanged)
public:
    enum {
        RET_OK = 0,
    };
    explicit PlugPagQt(QObject *parent = nullptr);

    QString lastDevice() const;
    QString version() const;
    bool authenticated() const;
    bool initted() const;
    bool inittedBt() const;
    bool paying() const;

public Q_SLOTS:
    void init(const QString &applicationName, const QString &applicationVersion);
    void authenticate();
    void logout();
    void initBt(const QString &macOrName);
    void startPaymentDebt(int value);
    void startPaymentCredit(int value);
    void abort();
    void voidLastPayment();
    QVariantHash getLastApprovedTransaction();

Q_SIGNALS:
    void inittedChanged();
    void inittedBtChanged();
    void authenticatedChanged();
    void payingChanged();
    void lastDeviceChanged();
    void eventReceived(int code, const QString &message);
    void transactionResult(const QVariantHash &result);

private:
    PlugPagQtPrivate *d_ptr;
};

#endif // PLUGPAGQT_H
