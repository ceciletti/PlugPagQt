import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtBluetooth 5.12

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "PlugPagQt"

    header: ToolBar {
        Label {
            id: statusL
            anchors.centerIn: parent
        }
    }

    Connections {
        target: plugpag
        onEventReceived: {
            statusL.text = message + " (" + code + ")"
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        Page {
            ColumnLayout {
                anchors.centerIn: parent
                Label {
                    text: "PlugPag: " + plugpag.version
                }
                Button {
                     enabled: !plugpag.initted
                    text: "Init"
                    onClicked: plugpag.init("PlugPagQt", "1.0.0")
                }
                Button {
                    enabled: plugpag.initted
                    text: plugpag.authenticated ? "Logout" : "Authenticate"
                    onClicked: plugpag.authenticated ? plugpag.logout() : plugpag.authenticate()
                }
                RowLayout {
                    Button {
                        enabled: plugpag.initted && plugpag.authenticated && !plugpag.inittedBt && btCombo.currentIndex !== -1
                        text: "Init BT"
                        onClicked: plugpag.initBt(btCombo.currentText)
                    }
                    ComboBox {
                        Layout.fillWidth: true
                        id: btCombo
                        textRole: "deviceName"
                        editable: true
                        model: BluetoothDiscoveryModel {

                        }
                        Component.onCompleted: editText = plugpag.lastDevice
                    }
                }
                Button {
                    enabled: plugpag.initted && plugpag.authenticated && plugpag.inittedBt
                    text: "Show Last Approved Transaction"
                    onClicked: plugpag.getLastApprovedTransaction()
                }
                Button {
                    enabled: plugpag.initted && plugpag.authenticated && plugpag.inittedBt
                    text: "Void Last Payment "
                    onClicked: plugpag.voidLastPayment()
                }
                Button {
                    enabled: plugpag.paying
                    text: "Abort"
                    onClicked: plugpag.abort()
                }
                TextField {
                    id: payValue
                    enabled: !plugpag.paying
                    text: "1,00"
                    inputMethodHints: Qt.ImhDigitsOnly
                    validator: DoubleValidator {
                        decimals: 2
                        locale: "pt_BR"
                        notation: DoubleValidator.StandardNotation
                    }
                }
                Button {
                    enabled: plugpag.initted && plugpag.authenticated && plugpag.inittedBt && payValue.acceptableInput && !plugpag.paying
                    text: "Pay with Debt"
                    onClicked: plugpag.startPaymentDebt(Number.fromLocaleString(Qt.locale("pt_BR"), payValue.text) * 100)
                }
                Button {
                    enabled: plugpag.initted && plugpag.authenticated && plugpag.inittedBt && payValue.acceptableInput && !plugpag.paying
                    text: "Pay with Credit"
                    onClicked: plugpag.startPaymentCredit(Number.fromLocaleString(Qt.locale("pt_BR"), payValue.text) * 100)
                }
            }

        }
    }
}
