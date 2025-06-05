#include <stdio.h>
#define ENTER_FOCUS_MODE_MESSAGE "Entering Focus Mode. All distractions are blocked.\n"\
"══════════════════════════════════════════════\n"\
"               Focus Round 1                \n"\
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


void runFocusMode(const int numOfRounds, const int duration) {
    char simulator_choice = 0; //choice of user
    printf(ENTER_FOCUS_MODE_MESSAGE);

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