# EasyBypassRestrictions

![](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)
![](https://img.shields.io/badge/release-1.0.1-red.svg?style=flat)
![](https://img.shields.io/badge/Android-7%20--%2012-blue.svg?style=flat)
![](https://img.shields.io/badge/arch-armeabi--v7a%20%7C%20arm64--v8a-blue.svg?style=flat)

[EasyBypassRestrictions](https://github.com/WindySha/EasyBypassRestrictions) is a tool that can easily bypass non-SDK interfaces restrictions started in android 9, also include a native library that can bypass dlopen system library restricton since android 7.


## Features

* Support bypass android hidden api restriction;  [Restrictions on non-SDK interfaces](https://developer.android.com/guide/app-compatibility/restrictions-non-sdk-interfaces)
* Support bypass restrcition of accessing private platform libraries;  [android-7.0-changes#ndk](https://developer.android.com/about/versions/nougat/android-7.0-changes#ndk)
* Support find the address of symbols in the elf .dynsym and .symtab section;
* Support Android 7 - 12 (API level 24 - 31);
* Support armeabi-v7a, arm64-v8a;


## Usage

### 1. Add dependency in build.gradle

This tool is published on [Maven Central](https://search.maven.org/), and uses [Prefab](https://google.github.io/prefab/) package format for [native dependencies](https://developer.android.com/studio/build/native-dependencies), which is supported by [Android Gradle Plugin 4.0+](https://developer.android.com/studio/releases/gradle-plugin?buildsystem=cmake#native-dependencies).

```Gradle
allprojects {
    repositories {
        mavenCentral()
    }
}
```

```Gradle
android {
    defaultConfig {
        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a'
        }
    }

    packagingOptions {
        pickFirst '**/libeasyBypass.so'
        // if this is used in a sdk project:
        // exclude '**/libeasyBypass.so'
    }

    // If you only need to bypass non-SDK interfaces restrictions, buildFeatures is not needed.
    buildFeatures {
        prefab true
    }
}

dependencies {
    implementation 'io.github.windysha:EasyBypassRestriction:1.0.1'
}
```



### 2. Bypass non-SDK interfaces restrictions

Add the code to your application attachBaseContext or onCreate method:

```java
import com.wind.storm.easybypass.EasyBypass;

if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
    EasyBypass.bypassHiddenApiRestriction();
}
```

If you only need to bypass non-SDK interfaces restrictions, please skip the following steps.



### 3. Use dlopen to open private platform libraries

#### 1. Add dependency in CMakeLists.txt

> CMakeLists.txt

```cmake
find_package(easyBypass REQUIRED CONFIG)

add_library(mylib SHARED mylib.c)
target_link_libraries(mylib easyBypass::easyBypass)
```

#### 2. Native Api

```c
#include "bypassed_dlfcn.h"

void *bypass_dlopen(const char *filename, int flag);
void *bypass_loader_dlopen(const char *filename, int flag);
```

*  `bypass_dlopen`: Call dlopen function through jni method, so  private platform libraries can also be opened easily.

*  `bypass_loader_dlopen: `Call system private function `__loader_dlopen` to achieve the dlopen function calling.

Most of the time, you only need to use `bypass_dlopen` to open any private platform libraries.



  ```
#include "fake_dlfcn.h"

void *fake_dlopen(const char *filename);
void *fake_dlsym(void *handle, const char *name);
void *fake_dlsym_symtab(void *handle, const char *name);
void fake_dlclose(void *handle);
void *get_base_address(const char* filepath);
  ```

This is the enhancements of system dlopen and dlsym function.


## Support

* Using sample, please check the [app](app).
* Communicate on [GitHub issues](https://github.com/WindySha/EasyBypassRestrictions/issues).


## License

EasyBypassRestrictions is MIT licensed, as found in the [LICENSE](LICENSE) file.