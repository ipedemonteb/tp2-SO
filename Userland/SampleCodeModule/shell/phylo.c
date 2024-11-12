#include "../include/phylo.h"

#define MAX_PHY 10
#define MIN_PHY 3
#define NO_PID -1
#define SEM_ID 100
#define ADD_VALUE 'a'
#define REMOVE_VALUE 'r'
#define QUIT_VALUE 'q'
#define SEM_PHYLO(x) (SEM_ID + (x) + 1)
#define RIGHT_PHYLO(x) (((x) + 1) % phylo_count)
#define LEFT_PHYLO(x) (((x) + phylo_count - 1) % phylo_count)

typedef enum {
	NONE = 0,
	DRIVING,
	WAITING,
	THINKING
} PHYLO_STATE;

static char state_chars[] = {'0', 'D', '.', '.'};

static char * phylo_names[] = {
	"Alonso", "Verstappen", "Senna", "Colapinto", "Schumacher",
    "Fangio", "Prost", "Lauda", "Clark", "Piquet"
};

static PHYLO_STATE phylo_states[MAX_PHY];
static int16_t phylo_pids[MAX_PHY];
static uint8_t phylo_count = 0;

void print_status() {
    uint8_t write = 0;
    for(int i = 0; i < phylo_count; i++) {
        if(phylo_states[i] != NONE) {
            write = 1;
            printf("%c ", state_chars[phylo_states[i]]);
        }
    }
    if(write) {
        printf("\n");
    }
}

void leave_pedals(uint8_t idx) {
    sem_wait(SEM_ID);
    phylo_states[idx] = THINKING;
    if(phylo_states[LEFT_PHYLO(idx)] == WAITING && phylo_states[LEFT_PHYLO(LEFT_PHYLO(idx))] != DRIVING) {
        phylo_states[LEFT_PHYLO(idx)] = DRIVING;
        sem_post(SEM_PHYLO(LEFT_PHYLO(idx)));
        print_status();
    }

    if(phylo_states[RIGHT_PHYLO(idx)] == WAITING && phylo_states[RIGHT_PHYLO(RIGHT_PHYLO(idx))] != DRIVING) {
        phylo_states[RIGHT_PHYLO(idx)] = DRIVING;
        sem_post(SEM_PHYLO(RIGHT_PHYLO(idx)));
        print_status();
    }
    sem_post(SEM_ID);
}

void take_pedals(uint8_t idx) {
    sem_wait(SEM_ID);
    phylo_states[idx] = WAITING;
    if(phylo_states[LEFT_PHYLO(idx)] != DRIVING && phylo_states[RIGHT_PHYLO(idx)] != DRIVING) {
        phylo_states[idx] = DRIVING;
        sem_post(SEM_PHYLO(idx));
        print_status();
    }
    sem_post(SEM_ID);
    sem_wait(SEM_PHYLO(idx));
}

void phylo_process(uint8_t argc, char * argv[]) {
    uint8_t index_phylo = satoi(argv[0]);
    my_free(argv[0]);
    my_free(argv);
    printf("%s is entering the race from the pitlane.\n", phylo_names[index_phylo]);
    phylo_states[index_phylo] = THINKING;
    while(1) {
        wait(20);
        take_pedals(index_phylo);
        wait(20);
        leave_pedals(index_phylo);
    }
}

int8_t new_phylo(uint8_t idx) {
    sem_wait(SEM_ID);
    if(sem_open(SEM_PHYLO(idx), 0) == -1) {
        return -1;
    }
    
    char ** argv = my_malloc(sizeof(char *) * 2);
    argv[0] = my_malloc(10);
    itos(idx, argv[0]);
    argv[1] = NULL;
    phylo_pids[idx] = create_process(phylo_process, 1, argv, phylo_names[idx], 0);

    if(phylo_pids[idx] != NO_PID) {
        phylo_count++;
    }
    print_status();
    sem_post(SEM_ID); 
    return phylo_pids[idx] != NO_PID ? 0 : -1;
}

void remove_phylo(uint8_t idx) {
    sem_wait(SEM_ID);
    printf("Checkered flag for %s\n", phylo_names[idx]);
    while(phylo_states[LEFT_PHYLO(idx)] == DRIVING && phylo_states[RIGHT_PHYLO(idx)] == DRIVING) {
        sem_post(SEM_ID);
        sem_wait(SEM_PHYLO(idx));
        sem_wait(SEM_ID);
    }
    kill(phylo_pids[idx]);
    wait_pid(phylo_pids[idx], 1);
    sem_close(SEM_PHYLO(idx));
    phylo_pids[idx] = NO_PID;
    phylo_states[idx] = NONE;
    phylo_count--;
    sem_post(SEM_ID);
    return ;
}

void remove_all(int max) {
    for(int i = 0; i < max; i++) {
        remove_phylo(i);
    }
}

int8_t run_phylo() {
    printf("Philosophers problem but with F1 drivers.\nEach driver needs two pedals to be able to drive.\nPress 'a' to add a driver, 'r' to remove one or 'q' to quit.\n");

    if(sem_open(SEM_ID, 1) == -1){
        printf("Couldn't start race.\n");
        return -1;
    }

    for(uint8_t i = 0; i < MAX_PHY; i++) {
        phylo_states[i] = NONE;
        phylo_pids[i] = NO_PID;
    }

    for(int i = 0; i < MIN_PHY; i++) {
        if(new_phylo(i) == -1) {
            printf("Couldn't start race.\n");
            remove_all(i);
            return -1;
        }
    }

    char cmd = 0;
    while((cmd = get_char()) != QUIT_VALUE) {
        printf("\n");
        switch(cmd) {
            case ADD_VALUE:
                if(phylo_count < MAX_PHY) {
                    if(new_phylo(phylo_count) == -1)
                        printf("Couldn't create new driver.\n");
                }
                else {
                    printf("Max number of drivers reached.\n");
                }
                break;
            case REMOVE_VALUE:
                if(phylo_count > MIN_PHY) {
                    remove_phylo(phylo_count - 1);
                }
                else {
                    printf("Min number of drivers reached.\n");
                }
                break;
        }
    }

    remove_all(phylo_count);
    sem_close(SEM_ID);
    printf("\n");
    return 0;
}