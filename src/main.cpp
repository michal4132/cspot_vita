#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/audioout.h>
#include <psp2/bgapputil.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/io/fcntl.h>
#include <imgui_vita.h>
#include <vitaGL.h>

#include "Paf.h"
#include <SpircController.h>
#include <JSONObject.h>
#include <ConfigJSON.h>
#include <Logger.h>

#include "CliFile.h"
#include "VitaAudioSink.h"
#include "Keyboard.h"
#include "Utils.h"
#include "Gui.h"

// TODO(michal4132):
// - proper http downloader cleanup
// - pause ImGui in sleep mode
// - settings screen
// - make GUI global?
// - fix crash caused by ImGui_ImplVitaGL_Shutdown when in system mode
// - mDNS / ZeroConf
// - mainline cspot
// - system music volume control
// - system music title, controls

// DEBUG
// #define CRASH_TEST

#define DEVICE_NAME           "PS Vita (CSpot)"
#define CREDENTIALS_FILE_NAME "ux0:data/cspot/authBlob.json"
#define CONFIG_FILE_NAME      "ux0:data/cspot/config.json"

std::shared_ptr<ConfigJSON> configMan;
std::shared_ptr<CliFile> file;
std::shared_ptr<MercuryManager> mercuryManager;
std::shared_ptr<SpircController> spircController;
std::shared_ptr<LoginBlob> blob;

static int watch_id;
static int cspot_id;

SceVoid watch_dog(SceSize _args, void *_argp) {
    GUI* gui = *((GUI**)_argp);

#if defined(CRASH_TEST)
    sceKernelDelayThread(15000000);
    gui->isRunning = false;
    return;
#endif

    SceAppMgrEvent appEvent;

    while (gui->isRunning) {
        sceAppMgrReceiveEvent(&appEvent);
        switch (appEvent.event) {
            case SCE_APP_EVENT_REQUEST_QUIT:
                gui->isRunning = false;
                return;
        }
        sceKernelDelayThread(1000000);
        sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
    }
}

void login_cspot(const char *user, const char *password) {
    blob->loadUserPass(user, password);
}

// TODO(michal4132): get from cspot
#define CLIENT_ID_ANDROID "65b708073fc0480ea92a077233ca87bd"
#define DEVICE_ID         "142137fd329622137a14901634264e6f332e2411"
#define SCOPES            "user-read-playback-state,user-modify-playback-state"

int start_cspot(SceSize _args, void *_argp) {
    GUI* gui = *((GUI**)_argp);

    CSPOT_LOG(info, "Creating player");
    auto session = std::make_unique<Session>();
    session->connectWithRandomAp();
    auto token = session->authenticate(blob);

    // Auth successful
    if (token.size() > 0) {
        // credentials ok, save for later
        file->writeFile(CREDENTIALS_FILE_NAME, blob->toJson());

        auto audioSink = std::make_shared<VitaAudioSink>();

        mercuryManager = std::make_shared<MercuryManager>(std::move(session));
        mercuryManager->startTask();

        while (!mercuryManager->isRunning) {
            sceKernelDelayThread(10000);
        }

        spircController = std::make_shared<SpircController>(mercuryManager, blob->username, audioSink);

        gui->cspot_started = true;

        // Request token for player control
        mercuryCallback responseLambda = [=](std::unique_ptr<MercuryResponse> res) {
            if (res->parts.size() == 0) {
                CSPOT_LOG(debug, "Empty response");
                return;
            }

            cJSON *root = cJSON_Parse((const char *) res->parts[0].data());
            char *token = cJSON_GetObjectItem(root, "accessToken")->valuestring;
            gui->api.set_token(token);
            cJSON_Delete(root);

            CSPOT_LOG(debug, "response: %s", res->parts[0].data());
        };
        mercuryManager->execute(MercuryType::GET, "hm://keymaster/token/authenticated?scope="
                            + std::string(SCOPES) +"&client_id="
                            + std::string(CLIENT_ID_ANDROID) +"&device_id=" + std::string(DEVICE_ID), responseLambda);

        // Add event handler
        spircController->setEventHandler([gui](CSpotEvent &event) {
            switch (event.eventType) {
            case CSpotEventType::TRACK_INFO: {
                TrackInfo track = std::get<TrackInfo>(event.data);
                gui->setTrack(track.name, track.album, track.artist, track.imageUrl);
                break;
            }
            case CSpotEventType::PLAY_PAUSE: {
                gui->setPause(std::get<bool>(event.data));
                break;
            }
            // case CSpotEventType::PLAYBACK_START: {
            //     this->audioBuffer->clearBuffer();
            //     break;
            // }
            // case CSpotEventType::SEEK: {
            //     this->audioBuffer->clearBuffer();
            //     break;
            // }
            default:
                break;
            }
        });

        // control CSpot from gui
        gui->nextCallback = []() {
            return spircController->nextSong();
        };

        gui->prevCallback = []() {
            return spircController->prevSong();
        };

        gui->activateDevice = []() {
            if (!spircController->state->isActive()) {
                spircController->state->setActive(true);
            }
            return spircController->notify();
        };

        gui->playToggleCallback = []() {
            return spircController->playToggle();
        };

        mercuryManager->reconnectedCallback = []() {
            return spircController->subscribe();
        };

        while (gui->isRunning) {
            mercuryManager->updateQueue();
            sceKernelDelayThread(10000);
        }
    }
    // login failed, back to login
    gui->set_screen(gui->login_screen);
    return 0;
}

void start_cspot_thread(GUI *gui) {
    cspot_id = sceKernelCreateThread("cspot", (SceKernelThreadEntry)start_cspot, 0x10000100, 0x10000, 0, 0, NULL);
    sceKernelStartThread(cspot_id, sizeof(void*), &gui);
}

int print_to_menu(const char* fmt, ...);
int vprint_to_menu(const char* fmt, va_list args);

int main(void) {
    bell::setDefaultLogger();
    bell::disableColors();
    bell::function_printf = &print_to_menu;
    bell::function_vprintf = &vprint_to_menu;

    GUI gui;

    init_network();

    watch_id = sceKernelCreateThread("watchdog", (SceKernelThreadEntry) watch_dog, 0x10000100, 0x100, 0, 0, NULL);
    GUI *gui_p = &gui;
    sceKernelStartThread(watch_id, sizeof(void*), &gui_p);

    sceIoMkdir("ux0:data/cspot", 0777);
    sceIoMkdir("ux0:data/cspot/cache", 0777);

    file = std::make_shared<CliFile>();
    configMan = std::make_shared<ConfigJSON>(CONFIG_FILE_NAME, file);

    if (!configMan->load()) {
        CSPOT_LOG(error, "Config error");
    }

    configMan->deviceName = DEVICE_NAME;
    configMan->format = AudioFormat_OGG_VORBIS_320;

    blob = std::make_shared<LoginBlob>();

    gui.init();

    std::string authData;
    file->readFile(CREDENTIALS_FILE_NAME, authData);
    if (authData.length() > 0) {
        blob->loadJson(authData);
        start_cspot_thread(&gui);
        gui.set_screen(gui.playback_screen);
    } else {
        gui.set_screen(gui.login_screen);
    }

    gui.start();

    sceAppMgrReleaseBgmPort();
    term_network();

    sceKernelWaitThreadEnd(watch_id, NULL, NULL);
    sceKernelWaitThreadEnd(cspot_id, NULL, NULL);
    sceKernelDeleteThread(watch_id);
    sceKernelDeleteThread(cspot_id);

    sceKernelExitProcess(0);  // DO NOT REMOVE
    return 0;
}
