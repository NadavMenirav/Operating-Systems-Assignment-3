#include <stdio.h>
#define ENTER_FOCUS_MODE_MESSAGE "Entering Focus Mode. All distractions are blocked.\n"
#define FOCUS_ROUND "══════════════════════════════════════════════\n"\
"               Focus Round %d                \n"\
"──────────────────────────────────────────────\n"


#define SIMULATE_DISTRACTION "\nSimulate a distraction:\n"\
"  1 = Email notification\n"\
"  2 = Reminder to pick up delivery\n"\
"  3 = Doorbell Ringing\n"\
"  q = Quit\n"

#define EMAIL_NOTIFICATION '1'
#define DELIVERY_REMINDER '2'
#define DOORBELL_RINGING '3'

void runFocusMode(const int numOfRounds, const int duration);
void handleRound(const int duration);

void runFocusMode(const int numOfRounds, const int duration) {
    printf(ENTER_FOCUS_MODE_MESSAGE);
    for (int i = 1; i <= numOfRounds; i++) {
        printf(FOCUS_ROUND, i);
        handleRound(duration);
    }

}
void handleRound(const int duration) {
    char simulator_choice = 0; //choice of user
    for (int i = 0; i < duration; i++) { //rounds
        printf(SIMULATE_DISTRACTION); //the options for the user
        scanf(" %c", &simulator_choice);
        switch (simulator_choice) {
            case EMAIL_NOTIFICATION: break; //EMAIL
            case DELIVERY_REMINDER: break; //Reminder to pick up delivery
            case DOORBELL_RINGING: break; //Doorbell ringing
            default: break; //quit
        }
    }
}