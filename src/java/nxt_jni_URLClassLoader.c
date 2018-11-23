
/*
 * Copyright (C) NGINX, Inc.
 */

#include <nxt_unit.h>
#include <jni.h>

#include "nxt_jni_URLClassLoader.h"

static jclass     nxt_java_URLClassLoader_class;
static jmethodID  nxt_java_URLClassLoader_ctor;
static jmethodID  nxt_java_URLClassLoader_loadClass;
static jmethodID  nxt_java_URLClassLoader_addURL;

static jclass     nxt_java_URL_class;
static jmethodID  nxt_java_URL_ctor;

int
nxt_java_initURLClassLoader(JNIEnv *env)
{
    jclass  cls;

    cls = (*env)->FindClass(env, "java/net/URLClassLoader");
    if (cls == NULL) {
        nxt_unit_warn(NULL, "java.net.URLClassLoader not found");
        return NXT_UNIT_ERROR;
    }

    nxt_java_URLClassLoader_class = (*env)->NewGlobalRef(env, cls);
    (*env)->DeleteLocalRef(env, cls);
    cls = nxt_java_URLClassLoader_class;

    nxt_java_URLClassLoader_ctor = (*env)->GetMethodID(env, cls,
        "<init>", "([Ljava/net/URL;)V");
    if (nxt_java_URLClassLoader_ctor == NULL) {
        nxt_unit_warn(NULL, "java.net.URLClassLoader constructor not found");
        goto failed;
    }

    nxt_java_URLClassLoader_loadClass = (*env)->GetMethodID(env, cls,
        "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (nxt_java_URLClassLoader_loadClass == NULL) {
        nxt_unit_warn(NULL, "java.net.URLClassLoader.loadClass not found");
        goto failed;
    }

    nxt_java_URLClassLoader_addURL = (*env)->GetMethodID(env, cls,
        "addURL", "(Ljava/net/URL;)V");
    if (nxt_java_URLClassLoader_addURL == NULL) {
        nxt_unit_warn(NULL, "java.net.URLClassLoader.addURL not found");
        goto failed;
    }


    cls = (*env)->FindClass(env, "java/net/URL");
    if (cls == NULL) {
        nxt_unit_warn(NULL, "java.net.URL not found");
        return NXT_UNIT_ERROR;
    }

    nxt_java_URL_class = (*env)->NewGlobalRef(env, cls);
    (*env)->DeleteLocalRef(env, cls);
    cls = nxt_java_URL_class;

    nxt_java_URL_ctor = (*env)->GetMethodID(env, cls,
        "<init>", "(Ljava/lang/String;)V");
    if (nxt_java_URL_ctor == NULL) {
        nxt_unit_warn(NULL, "java.net.URL constructor not found");
        goto failed;
    }

    return NXT_UNIT_OK;

failed:

    (*env)->DeleteGlobalRef(env, cls);
    return NXT_UNIT_ERROR;
}

jobject
nxt_java_newURLClassLoader(JNIEnv *env, int url_count, char **urls)
{
    jobjectArray jurls = nxt_java_newURls(env, url_count, urls);

    if (jurls == NULL) {
        return NULL;
    }

    return (*env)->NewObject(env, nxt_java_URLClassLoader_class,
                             nxt_java_URLClassLoader_ctor, jurls);
}

jobjectArray
nxt_java_newURls(JNIEnv *env, int url_count, char **urls)
{
    jobjectArray jurls = (*env)->NewObjectArray(env, url_count,
                                                nxt_java_URL_class, NULL);
    if (jurls == NULL) {
        return NULL;
    }

    for (int i = 0; i < url_count; i++) {
        jstring surl = (*env)->NewStringUTF(env, urls[i]);
        if (surl == NULL) {
            return NULL;
        }

        jobject jurl = (*env)->NewObject(env, nxt_java_URL_class,
                                         nxt_java_URL_ctor, surl);
        if (jurl == NULL) {
            return NULL;
        }

        (*env)->SetObjectArrayElement(env, jurls, i, jurl);
    }

    return jurls;
}

jclass
nxt_java_loadClass(JNIEnv *env, jobject cl, const char *name)
{
    jstring jname = (*env)->NewStringUTF(env, name);
    if (jname == NULL) {
        return NULL;
    }

    return (*env)->CallObjectMethod(env, cl, nxt_java_URLClassLoader_loadClass,
                                    jname);
}

void
nxt_java_addURL(JNIEnv *env, jobject cl, const char *url)
{
    jstring surl = (*env)->NewStringUTF(env, url);
    if (surl == NULL) {
        return;
    }

    jobject jurl = (*env)->NewObject(env, nxt_java_URL_class,
                                     nxt_java_URL_ctor, surl);
    if (jurl == NULL) {
        return;
    }

    (*env)->CallVoidMethod(env, cl, nxt_java_URLClassLoader_addURL, jurl);
}
