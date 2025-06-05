#include <stdio.h>
#include <stdlib.h>

#define ENTER_FOCUS_MODE_MESSAGE "Entering Focus Mode. All distractions are blocked.\n"
#define FOCUS_ROUND "══════════════════════════════════════════════\n"\
"               Focus Round %d                \n"\
"──────────────────────────────────────────────\n"

#define CHECK_DISTRACTIONS "──────────────────────────────────────────────\n"\
"        Checking pending distractions...\n"\
"──────────────────────────────────────────────\n"\


#define SIMULATE_DISTRACTION "\nSimulate a distraction:\n"\
"  1 = Email notification\n"\
"  2 = Reminder to pick up delivery\n"\
"  3 = Doorbell Ringing\n"\
"  q = Quit\n"

#define EMAIL_NOTIFICATION '1'
#define DELIVERY_REMINDER '2'
#define DOORBELL_RINGING '3'
#define EMAIL_DISTRACTION "- Email notification is waiting.\n"\
"[Outcome:] The TA announced: Everyone get 100 on the exercise!\n"
#define DELIVERY_DISTRACTION " - You have a reminder to pick up your delivery.\n"\
"[Outcome:] You picked it up just in time.\n"
#define DOORBELL_DISTRACTION " - The doorbell is ringing.\n"\
"[Outcome:] Food delivery is here.\n"

void runFocusMode(const int numOfRounds, const int duration);
char* handleRound(const int duration);

void runFocusMode(const int numOfRounds, const int duration) {
    const char* distractions = NULL;

    printf(ENTER_FOCUS_MODE_MESSAGE);
    for (int i = 1; i <= numOfRounds; i++) {
        printf(FOCUS_ROUND, i);
        distractions = handleRound(duration);
        printf(CHECK_DISTRACTIONS);
        printf(distractions);

    }

}
char* handleRound(const int duration) {
    int length = 0;
    int emailLength = strlen(EMAIL_DISTRACTION);
    int deliveryLength = strlen(DELIVERY_DISTRACTION);
    int doorbellLength = strlen(DOORBELL_DISTRACTION);
    char* allDistractions = malloc(1);
    allDistractions[0] = '\0';

    char simulator_choice = 0; //choice of user
    for (int i = 0; i < duration; i++) { //rounds
        printf(SIMULATE_DISTRACTION); //the options for the user
        scanf(" %c", &simulator_choice);
        switch (simulator_choice) {
            case EMAIL_NOTIFICATION:
                allDistractions = realloc(allDistractions, (length + emailLength+ 1) * sizeof(char)); // +1 for null terminator
                strcpy(allDistractions + length, EMAIL_DISTRACTION);
                length += emailLength;
                break; //EMAIL
            case DELIVERY_REMINDER:
                allDistractions = realloc(allDistractions, (length + deliveryLength + 1) * sizeof(char));
                strcpy(allDistractions + length, DELIVERY_DISTRACTION);
                length += deliveryLength;
                break; //Reminder to pick up delivery
            case DOORBELL_RINGING:
                allDistractions = realloc(allDistractions, (length + doorbellLength + 1) * sizeof(char));
                strcpy(allDistractions + length, DOORBELL_DISTRACTION);
                length += doorbellLength;
                break; //Doorbell ringing
            default: return allDistractions; //quit
        }
    }
    return allDistractions;


}