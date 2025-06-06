#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define ENTER_FOCUS_MODE_MESSAGE "Entering Focus Mode. All distractions are blocked.\n"
#define FOCUS_ROUND "══════════════════════════════════════════════\n"\
"               Focus Round %d                \n"\
"──────────────────────────────────────────────\n"

#define CHECK_DISTRACTIONS "──────────────────────────────────────────────\n"\
"        Checking pending distractions...\n"\
"──────────────────────────────────────────────\n"\

#define RETURN_TO_FOCUS "──────────────────────────────────────────────\n"\
"             Back to Focus Mode.\n"\
"══════════════════════════════════════════════\n"



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
#define NO_DISTRACTIONS "No distractions reached you this round.\n"
#define FOCUS_MODE_COMPLETED "Focus Mode complete. All distractions are now unblocked.\n"

typedef enum {
    false,
    true
}boolean;


void runFocusMode(const int numOfRounds, const int duration);
char* handleRound(const int duration);
void sigConsumer();
void sendSig(const char userChoice);
void FindSig(int* receivedSignals, int receivedSignalsCount, int* osig);
void blockSignals();
void unblockSignals();

void runFocusMode(const int numOfRounds, const int duration) {
    const char* distractions = NULL;

    printf(ENTER_FOCUS_MODE_MESSAGE); //starting message

    for (int i = 1; i <= numOfRounds; i++) {
        printf(FOCUS_ROUND, i);
        distractions = handleRound(duration);

        printf(CHECK_DISTRACTIONS);

        if (strlen(distractions) > 0) // are there distractions
            printf(distractions);
        else // user quit on the first time
            printf(NO_DISTRACTIONS);

        printf(RETURN_TO_FOCUS);
    }

    printf(FOCUS_MODE_COMPLETED);

}
char* handleRound(const int duration) {
    int receivedSignals[3] = {0, 0, 0};
    char simulator_choice = 0; //choice of user
    sigset_t pending; // the signals waiting

    for (int i = 0; i < duration; i++) { //rounds
        printf(SIMULATE_DISTRACTION); //the options for the user
        scanf(" %c", &simulator_choice);
        sendSig(simulator_choice);

        switch (simulator_choice) {
            case EMAIL_NOTIFICATION:
                if (isReceivedEmail) break;
                isReceivedEmail = true;
                allDistractions = realloc(allDistractions, (length + emailLength+ 1) * sizeof(char)); // +1 for null terminator
                strcpy(allDistractions + length, EMAIL_DISTRACTION);
                length += emailLength;
                break; //EMAIL
            case DELIVERY_REMINDER:
                if (isReceivedDelivery) break;
                isReceivedDelivery = true;
                allDistractions = realloc(allDistractions, (length + deliveryLength + 1) * sizeof(char));
                strcpy(allDistractions + length, DELIVERY_DISTRACTION);
                length += deliveryLength;
                break; //Reminder to pick up delivery
            case DOORBELL_RINGING:
                if (isReceivedDoorbell) break;
                isReceivedDoorbell = true;
                allDistractions = realloc(allDistractions, (length + doorbellLength + 1) * sizeof(char));
                strcpy(allDistractions + length, DOORBELL_DISTRACTION);
                length += doorbellLength;
                break; //Doorbell ringing
            default: return allDistractions; //quit
        }
    }
    return allDistractions;
}

void blockSignals() {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR1);
    sigaddset(&sigs, SIGUSR2);
    sigaddset(&sigs, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &sigs, NULL) == -1) {
        perror("sigprocmask failed");
        exit(EXIT_FAILURE);
    }
}

void unblockSignals() {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR1);
    sigaddset(&sigs, SIGUSR2);
    sigaddset(&sigs, SIGCHLD);

    if (sigprocmask(SIG_UNBLOCK, &sigs, NULL) == -1) {
        perror("sigprocmask failed");
        exit(EXIT_FAILURE);
    }
}


void sendSig(const char userChoice) {
    // sends the signal
    int signal = 0;
    switch (userChoice) {
        case EMAIL_NOTIFICATION:
            signal = SIGUSR1;
            break;
        case DELIVERY_REMINDER:
            signal = SIGUSR2;
            break;
        case DOORBELL_RINGING:
            signal = SIGCHLD;
            break;
        default:
            perror("Unknown signal");
            break;
    }
    raise(signal);
}

void sigConsumer() {

    sigset_t signalsToUnblock;
    sigemptyset(&signalsToUnblock);
    sigaddset(&signalsToUnblock, SIGUSR1);
    sigaddset(&signalsToUnblock, SIGUSR2);
    sigaddset(&signalsToUnblock, SIGCHLD);

    if (sigprocmask(SIG_UNBLOCK, &signalsToUnblock, NULL) == -1) {
        perror("unblocking signals failed");
        exit(EXIT_FAILURE);
    }
}