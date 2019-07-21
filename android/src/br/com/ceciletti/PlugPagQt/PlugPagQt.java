package br.com.ceciletti.PlugPagQt;

import android.app.Activity;
import android.view.Window;

import br.com.uol.pagseguro.plugpag.PlugPag;
import br.com.uol.pagseguro.plugpag.PlugPagAppIdentification;

import br.com.uol.pagseguro.plugpag.PlugPagEventData;
import br.com.uol.pagseguro.plugpag.PlugPagEventListener;

import br.com.uol.pagseguro.plugpag.PlugPagDevice;

import br.com.uol.pagseguro.plugpag.PlugPagAuthenticationListener;
import br.com.uol.pagseguro.plugpag.PlugPagPaymentData;
import br.com.uol.pagseguro.plugpag.PlugPagTransactionResult;
import br.com.uol.pagseguro.plugpag.PlugPagVoidData;

class PlugPagNatives
{
    // declare the native method
    public static native void sendInitted(boolean authenticated, String libVersion);
    public static native void sendInittedBt(int code);
    public static native void sendEvent(int code, String message);
    public static native void sendAuthenticationSuccess();
    public static native void sendTransactionResult(PlugPagTransactionResult result);
    public static native void sendLastTransactionResult(PlugPagTransactionResult result);
}

class PlugPagEventListenerQt implements PlugPagEventListener
{
    @Override
    public int onEvent(PlugPagEventData plugPagEventData) {
        System.out.println("Java PlugPage event: " + PlugPagEventData.getDefaultMessage(plugPagEventData.getEventCode()));
        System.out.println("Java PlugPage event code: " + plugPagEventData.getEventCode());

        int code = plugPagEventData.getEventCode();
        PlugPagNatives.sendEvent(code, PlugPagEventData.getDefaultMessage(code));

        return PlugPag.RET_OK;
    }
}

class PlugPagAuthenticationListenerQt implements PlugPagAuthenticationListener
{
    @Override
    public void onSuccess() {
        System.out.println("Java Setup onSuccess.......");
        PlugPagNatives.sendAuthenticationSuccess();
    }

    @Override
    public void onError() {
        System.out.println("Java Setup onError.......");
    }
}

public class PlugPagQt
{
    public static PlugPag mPlugPag = null;
    public static PlugPagAuthenticationListenerQt mPlugPagAuthListener = null;

    public void setup(final Activity qtActivity, final String appName, final String appVersion)
    {
        System.out.println("Java Setup: " + appName + " " + appVersion);
        if (this.mPlugPag == null) {
            System.out.println("Java Setup init: " + appName + " " + appVersion);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    System.out.println("Inside : " + Thread.currentThread().getName() + appName + " " + appVersion);
                    PlugPagAppIdentification appIdentification = new PlugPagAppIdentification(appName, appVersion);
                    PlugPagQt.mPlugPag = new PlugPag(qtActivity, appIdentification);
                    PlugPagQt.mPlugPag.setEventListener(new PlugPagEventListenerQt());
                    PlugPagQt.mPlugPagAuthListener = new PlugPagAuthenticationListenerQt();

                    PlugPagNatives.sendInitted(PlugPagQt.mPlugPag.isAuthenticated(), PlugPagQt.mPlugPag.getLibVersion());
                    System.out.println("Inside END: " + Thread.currentThread().getName());
                    int ret = PlugPagQt.mPlugPag.checkRequirements(PlugPagDevice.TYPE_PINPAD);
                    System.out.println("Inside END: " + ret);
                    ret = PlugPagQt.mPlugPag.checkRequirements(PlugPagDevice.TYPE_TERMINAL);
                    System.out.println("Inside END: " + ret);
                }
            }).start();
        }
    }

    public void authenticate()
    {
        System.out.println("requestAuthentication: " + Thread.currentThread().getName() + " isauth: " + this.mPlugPag.isAuthenticated());
        this.mPlugPag.requestAuthentication(this.mPlugPagAuthListener);
    }

    public void initBt(final String macOrName)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                PlugPagDevice device = new PlugPagDevice(macOrName);
                int ret = PlugPagQt.mPlugPag.initBTConnection(device);
                PlugPagNatives.sendInittedBt(ret);
            }
        }).start();
    }

    public boolean isAuthenticated()
    {
        return this.mPlugPag.isAuthenticated();
    }

    public void logout()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                PlugPagQt.mPlugPag.invalidateAuthentication();
            }
        }).start();
    }

    public void abort()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                PlugPagQt.mPlugPag.abort();
            }
        }).start();
    }

    public void startPaymentDebt(final int value)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                System.out.println("Java startPaymentDebt: " + value);
                PlugPagPaymentData paymentData = new PlugPagPaymentData(
                PlugPag.TYPE_DEBITO,
                value, // Valor: R$ 1,23
                PlugPag.INSTALLMENT_TYPE_A_VISTA,
                1, // Parcelas
                "...");
                PlugPagTransactionResult result = PlugPagQt.mPlugPag.doPayment(paymentData);
                System.out.println("Java startPaymentDebt getResult: " + result.getResult());
                PlugPagNatives.sendTransactionResult(result);
             }
         }).start();
    }

    public void startPaymentCredit(final int value)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                System.out.println("Java startPaymentCredit: " + value);
//                PlugPagPaymentData paymentData = new PlugPagPaymentData(
//                PlugPag.TYPE_CREDITO,
//                value, // Valor: R$ 1,23
//                PlugPag.INSTALLMENT_TYPE_A_VISTA,
//                1, // Parcelas
//                "...");

                PlugPagPaymentData paymentData = null;

                paymentData = new PlugPagPaymentData.Builder()
                        .setType(PlugPag.TYPE_CREDITO)
                        .setInstallmentType(PlugPag.INSTALLMENT_TYPE_A_VISTA)
                        .setAmount(value)
                        .setUserReference("qt-ref")
                        .build();

                PlugPagTransactionResult result = PlugPagQt.mPlugPag.doPayment(paymentData);
                System.out.println("Java startPaymentCredit getResult: " + result.getResult());
                PlugPagNatives.sendTransactionResult(result);
             }
         }).start();
    }

    public void voidLastPayment()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                PlugPagTransactionResult lastResult = PlugPagQt.mPlugPag.getLastApprovedTransaction();
                if (lastResult == null) {
                    PlugPagNatives.sendTransactionResult(lastResult);
                    return;
                }

                PlugPagVoidData voidData = new PlugPagVoidData
                .Builder()
                .setTransactionCode(lastResult.getTransactionCode())
                .setTransactionId(lastResult.getTransactionId())
                .build();

                PlugPagTransactionResult result = PlugPagQt.mPlugPag.voidPayment(voidData);
                System.out.println("Java voidPayment: " + result.getResult());
                PlugPagNatives.sendTransactionResult(result);
             }
         }).start();
    }

    public void getLastApprovedTransaction()
    {
        PlugPagTransactionResult result = this.mPlugPag.getLastApprovedTransaction();
        PlugPagNatives.sendLastTransactionResult(result);
//        if (result == null) {
//            return result;
//        }
    //        String bin = result.getBin();
    //        String cardBrand = result.getCardBrand();
    //        String date = result.getDate();
    //        String time = result.getTime();
    //        String holder = result.getHolder();
    //        String hostNsu = result.getHostNsu();
    //        String message = result.getMessage();
    //        String terminalSerialNumber = result.getTerminalSerialNumber();
    //        String userReference = result.getUserReference();
    //        String transactionCode = result.getTransactionCode();
    //        String transactionid = result.getTransactionId();
    //        String errorCode = result.getErrorCode();
    //        String amount = result.getAmount();
    //        String availableBalance = result.getAvailableBalance();

//        return result;
    }
}
