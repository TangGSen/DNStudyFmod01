
#include <unistd.h>
#include "android/log.h"
#include "sen_effect_utils.h"
#include "fmod.hpp"
#define LOG_TAG "sen"
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
using namespace FMOD;

#define TYPE_NORMAL 0
#define TYPE_LUOLI 1
#define TYPE_DASHU 2

JNIEXPORT void JNICALL Java_sen_com_fmod_EffectUtils_playNormal
(JNIEnv *env, jclass clazz, jstring filePath, jint jType){
    const char *cFilePath = env->GetStringUTFChars(filePath,NULL);
    void  *extradriverdata = 0;
    //初始化
    System * system;
    System_Create(&system);
    //32 是同一个时间，播放32通道
    system->init(32, FMOD_INIT_NORMAL,extradriverdata);
    Sound      *sound;
    Channel    *channel;
    DSP *dsp;
    bool   playing = false;
    //创建声音
   // const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound)
    try {

        system->createSound(cFilePath,FMOD_DEFAULT,0,&sound);

        switch (jType){
            case TYPE_NORMAL:
                LOG_E("TYPE_NORMAL %d",jType);
                system->playSound(sound,NULL, false,&channel);
                system->update();


                break;
            case TYPE_LUOLI:
                system->playSound(sound, NULL, false, &channel);
                system->update();
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT,&dsp);
                //设置音调参数
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH,1.8);
                channel->addDSP(0,dsp);
                break;
            case TYPE_DASHU:
                system->playSound(sound, NULL, false, &channel);
                system->update();
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                //设置音调参数
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8);
                channel->addDSP(0, dsp);
                break;
        }
    }catch (...){
        //...代表捕获很多异常
        goto end;
    }

    channel->isPlaying(&playing);
    //每秒判断一下
    while (playing){
        channel->isPlaying(&playing);
        usleep(1000*1000);
    }
    goto end;
    end:
    sound->release();
    system->close();
    system->release();
    env->ReleaseStringUTFChars(filePath,cFilePath);
};
