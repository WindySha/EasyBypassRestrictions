package com.wind.storm.bypass;

import androidx.annotation.Keep;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import com.wind.storm.easybypass.EasyBypass;

import java.lang.reflect.Method;

@Keep
public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("test-lib");
    }

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        testHiddenApiAccessable();
        EasyBypass.bypassHiddenApiRestriction();
        testHiddenApiAccessable();

        testDlFunction();
    }

    /**
     * enable hidden api restriction command:
     * android 9 :
     * adb shell settings delete global hidden_api_policy_pre_p_apps
     * adb shell settings delete global hidden_api_policy_p_apps
     * <p>
     * android 10 and above:
     * adb shell settings delete global hidden_api_policy
     */
    private void testHiddenApiAccessable() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.P) {
            return;
        }
        try {
            Class clazz = Class.forName("dalvik.system.VMRuntime");
            Method method =
                    clazz.getDeclaredMethod("setHiddenApiExemptions", String[].class);
            Log.d(TAG, " setHiddenApiExemptions method -> " + method);
        } catch (Exception e) {
            Log.e(TAG, " Access HiddenApi failed !!! ", e);
        }
    }

    // test the bypass_dlopen and fake_dlopen functions
    @Keep
    private static native void testDlFunction();
}