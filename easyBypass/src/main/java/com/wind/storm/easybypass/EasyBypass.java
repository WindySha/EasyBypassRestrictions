package com.wind.storm.easybypass;

import android.os.Build;

import java.lang.reflect.Method;

public class EasyBypass {

    static {
        System.loadLibrary("easybypass");
    }

    public static void bypassHiddenApiRestriction() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.P) {
            return;
        }
        try {
            Method method = EasyBypass.class.getDeclaredMethod("nativeSetHiddenApiExemptions", String[].class);
            method.setAccessible(true);
            method.invoke(null, (Object) (new String[]{"L"}));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static native void nativeSetHiddenApiExemptions(String[] signaturePrefixes);

    private static native long nativeDlopen(long address, long filename, int flags);
}
