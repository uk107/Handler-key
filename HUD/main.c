#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Observer 인터페이스
typedef struct HUDObserver HUDObserver;

struct HUDObserver {
    void (*update)(HUDObserver *self, const char *settings); // 함수 포인터
};

// Subject 인터페이스
typedef struct HUDSettings HUDSettings;

struct HUDSettings {
    HUDObserver **observers;    // Observer 배열
    int num_observers;          // 현재 Observer 수
    int capacity;               // Observer 배열의 용량
    void (*attach)(HUDSettings *self, HUDObserver *observer);
    void (*detach)(HUDSettings *self, HUDObserver *observer);
    void (*notify)(HUDSettings *self);
    void (*set_setting)(HUDSettings *self, const char *key, const char *value);
    char settings[256];         // 설정 데이터 저장
};

// Observer 업데이트 함수
void hud_observer_update(HUDObserver *self, const char *settings) {
    printf("HUD Observer received update: %s\n", settings);
}

// HUDSettings 함수 구현
void attach(HUDSettings *self, HUDObserver *observer) {
    if (self->num_observers >= self->capacity) {
        self->capacity *= 2;
        self->observers = realloc(self->observers, self->capacity * sizeof(HUDObserver *));
    }
    self->observers[self->num_observers++] = observer;
}

void detach(HUDSettings *self, HUDObserver *observer) {
    for (int i = 0; i < self->num_observers; i++) {
        if (self->observers[i] == observer) {
            for (int j = i; j < self->num_observers - 1; j++) {
                self->observers[j] = self->observers[j + 1];
            }
            self->num_observers--;
            break;
        }
    }
}

void notify(HUDSettings *self) {
    for (int i = 0; i < self->num_observers; i++) {
        self->observers[i]->update(self->observers[i], self->settings);
    }
}

void set_setting(HUDSettings *self, const char *key, const char *value) {
    snprintf(self->settings, sizeof(self->settings), "%s: %s", key, value);
    self->notify(self);
}

// HUDSettings 및 HUDObserver 생성 함수
HUDSettings *create_hud_settings() {
    HUDSettings *hud_settings = malloc(sizeof(HUDSettings));
    hud_settings->observers = malloc(10 * sizeof(HUDObserver *));
    hud_settings->num_observers = 0;
    hud_settings->capacity = 10;
    hud_settings->attach = attach;
    hud_settings->detach = detach;
    hud_settings->notify = notify;
    hud_settings->set_setting = set_setting;
    return hud_settings;
}

void destroy_hud_settings(HUDSettings *hud_settings) {
    free(hud_settings->observers);
    free(hud_settings);
}

HUDObserver *create_hud_observer(void (*update)(HUDObserver *self, const char *settings)) {
    HUDObserver *observer = malloc(sizeof(HUDObserver));
    observer->update = update;
    return observer;
}

void destroy_hud_observer(HUDObserver *observer) {
    free(observer);
}

// 메인 함수
int main() {
    // HUDSettings와 HUDObserver 생성
    HUDSettings *hud_settings = create_hud_settings();
    HUDObserver *observer1 = create_hud_observer(hud_observer_update);
    HUDObserver *observer2 = create_hud_observer(hud_observer_update);

    // Observer 등록
    printf("Test Case 1: Adding Observers\n");
    hud_settings->attach(hud_settings, observer1);
    hud_settings->attach(hud_settings, observer2);

    // 설정 변경
    printf("\nTest Case 2: Setting 'Brightness' to '75%%'\n");
    hud_settings->set_setting(hud_settings, "Brightness", "75%");
    printf("\nTest Case 3: Setting 'Contrast' to '85%%'\n");
    hud_settings->set_setting(hud_settings, "Contrast", "85%");

    // Observer 제거
    printf("\nTest Case 4: Removing Observer 1\n");
    hud_settings->detach(hud_settings, observer1);

    // 설정 변경
    printf("\nTest Case 5: Setting 'Brightness' to '50%%'\n");
    hud_settings->set_setting(hud_settings, "Brightness", "50%");

    // 메모리 해제
    destroy_hud_observer(observer1);
    destroy_hud_observer(observer2);
    destroy_hud_settings(hud_settings);

    return 0;
}
