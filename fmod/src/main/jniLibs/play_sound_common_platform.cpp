#include "common.h"
#include "include/fmod.hpp"
#include <string.h>
#include <jni.h>
#include <unistd.h>
#include <string>
#include <vector>

JNIEnv *gJNIEnv;
jobject gMainActivityObject;
int gDownButtons;
int gLastDownButtons;
int gPressedButtons;
bool gSuspendState;
bool gQuitState;
std::string gUIString;
std::vector<char *> gPathList;




int FMOD_Main()
{
    FMOD::System     *system;
    FMOD::Sound      *sound1, *sound2, *sound3;
    FMOD::Channel    *channel = 0;
    FMOD_RESULT       result;
    unsigned int      version;
    void             *extradriverdata = 0;

    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);

    result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
    ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */

    result = system->createSound(Common_MediaPath("jaguar.wav"), FMOD_DEFAULT, 0, &sound2);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("swish.wav"), FMOD_DEFAULT, 0, &sound3);
    ERRCHECK(result);

    /*
        Main loop
    */
    do
    {
        Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            result = system->playSound(sound1, 0, false, &channel);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            result = system->playSound(sound2, 0, false, &channel);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION3))
        {
            result = system->playSound(sound3, 0, false, &channel);
            ERRCHECK(result);
        }

        result = system->update();
        ERRCHECK(result);

        {
            unsigned int ms = 0;
            unsigned int lenms = 0;
            bool         playing = 0;
            bool         paused = 0;
            int          channelsplaying = 0;

            if (channel)
            {
                FMOD::Sound *currentsound = 0;

                result = channel->isPlaying(&playing);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPaused(&paused);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                channel->getCurrentSound(&currentsound);
                if (currentsound)
                {
                    result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }
            }

            system->getChannelsPlaying(&channelsplaying, NULL);

            Common_Draw("==================================================");
            Common_Draw("Play Sound Example.");
            Common_Draw("Copyright (c) Firelight Technologies 2004-2017.");
            Common_Draw("==================================================");
            Common_Draw("");
            Common_Draw("Press %s to play a mono sound (drumloop)", Common_BtnStr(BTN_ACTION1));
            Common_Draw("Press %s to play a mono sound (jaguar)", Common_BtnStr(BTN_ACTION2));
            Common_Draw("Press %s to play a stereo sound (swish)", Common_BtnStr(BTN_ACTION3));
            Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
            Common_Draw("");
            Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
            Common_Draw("Channels Playing %d", channelsplaying);
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound1->release();
    ERRCHECK(result);
    result = sound2->release();
    ERRCHECK(result);
    result = sound3->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}


void Common_Init(void **extraDriverData)
{
	gDownButtons = 0;
	gLastDownButtons = 0;
	gPressedButtons = 0;
	gSuspendState = false;
	gQuitState = false;
}

void Common_Close()
{
    for (std::vector<char *>::iterator item = gPathList.begin(); item != gPathList.end(); ++item)
    {
        free(*item);
    }

    gPathList.clear();
}

void Common_Update()
{
	jstring text = gJNIEnv->NewStringUTF(gUIString.c_str());
	jclass mainActivityClass = gJNIEnv->GetObjectClass(gMainActivityObject);
    jmethodID updateScreenMethodID = gJNIEnv->GetMethodID(mainActivityClass, "updateScreen", "(Ljava/lang/String;)V");

    gJNIEnv->CallVoidMethod(gMainActivityObject, updateScreenMethodID, text);

    gJNIEnv->DeleteLocalRef(text);
    gJNIEnv->DeleteLocalRef(mainActivityClass);

    gUIString.clear();

    gPressedButtons = (gLastDownButtons ^ gDownButtons) & gDownButtons;
    gLastDownButtons = gDownButtons;

    if (gQuitState)
    {
    	gPressedButtons |= (1 << BTN_QUIT);
    }
}

void Common_Sleep(unsigned int ms)
{
    usleep(ms * 1000);
}

void Common_Exit(int returnCode)
{
    exit(returnCode);
}

void Common_DrawText(const char *text)
{
    char s[256];
    snprintf(s, sizeof(s), "%s\n", text);
    
    gUIString.append(s);
}

void Common_LoadFileMemory(const char *name, void **buff, int *length)
{
    FILE *file = fopen(name, "rb");
    
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    void *mem = malloc(len);
    fread(mem, 1, len, file);
    
    fclose(file);

    *buff = mem;
    *length = len;
}

void Common_UnloadFileMemory(void *buff)
{
    free(buff);
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gPressedButtons & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return ((gDownButtons & (1 << btn)) != 0);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1: return "A";
        case BTN_ACTION2: return "B";
        case BTN_ACTION3: return "C";
        case BTN_ACTION4: return "D";
        case BTN_UP:      return "Up";
        case BTN_DOWN:    return "Down";
        case BTN_LEFT:    return "Left";
        case BTN_RIGHT:   return "Right";
        case BTN_MORE:    return "E";
        case BTN_QUIT:    return "Back";
        default:          return "Unknown";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    char *filePath = (char *)calloc(256, sizeof(char));

    strcat(filePath, "file:///android_asset/");
    strcat(filePath, fileName);
    gPathList.push_back(filePath);

    return filePath;
}

const char *Common_WritePath(const char *fileName)
{
	return Common_MediaPath(fileName);
}

bool Common_SuspendState()
{
	return gSuspendState;
}

extern "C"
{

jstring Java_org_fmod_example_MainActivity_getButtonLabel(JNIEnv *env, jobject thiz, jint index)
{
    return env->NewStringUTF(Common_BtnStr((Common_Button)index));
}

void Java_org_fmod_example_MainActivity_buttonDown(JNIEnv *env, jobject thiz, jint index)
{
    gDownButtons |= (1 << index);
}

void Java_org_fmod_example_MainActivity_buttonUp(JNIEnv *env, jobject thiz, jint index)
{
    gDownButtons &= ~(1 << index);
}

void Java_org_fmod_example_MainActivity_setStateCreate(JNIEnv *env, jobject thiz)
{

}

void Java_org_fmod_example_MainActivity_setStateStart(JNIEnv *env, jobject thiz)
{
	gSuspendState = false;
}

void Java_org_fmod_example_MainActivity_setStateStop(JNIEnv *env, jobject thiz)
{
	gSuspendState = true;
}

void Java_org_fmod_example_MainActivity_setStateDestroy(JNIEnv *env, jobject thiz)
{
	gQuitState = true;
}

void Java_org_fmod_example_MainActivity_main(JNIEnv *env, jobject thiz)
{
	gJNIEnv = env;
	gMainActivityObject = thiz;

	FMOD_Main();
}

} /* extern "C" */
