#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>

// ---------------------------------------------------------
// 10. IPC (Inter-Process Communication): Shared Memory Struct
// ---------------------------------------------------------
typedef struct {
    int ledger_balance;
    sem_t mutex;     // 5. Mutex (Binary semaphore for exclusive access)
    sem_t db_slots;  // 6. Semaphore (Counting semaphore for available connections)
} SharedData;

SharedData *shared_data;
int shmid;

// ---------------------------------------------------------
// 9. Msg Signal: Emergency/Cleanup Handler
// ---------------------------------------------------------
void handle_signal(int sig) {
    printf("\n[Signal] System interrupt (%d) received. Cleaning up IPC resources...\n", sig);
    // Destroy semaphores and free shared memory
    sem_destroy(&shared_data->mutex);
    sem_destroy(&shared_data->db_slots);
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

// Job Structure for Scheduling and Banker's Algorithm
typedef struct {
    int id;
    int burst_time; // Time it takes to execute
    int max_res;    // Maximum resources needed
    int alloc_res;  // Resources currently allocated to it
} Job;

// ---------------------------------------------------------
// 4. CPU Scheduling Algorithm: Shortest Job First (SJF)
// ---------------------------------------------------------
void sjf_schedule(Job jobs[], int n) {
    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - i - 1; j++) {
            if(jobs[j].burst_time > jobs[j+1].burst_time) {
                Job temp = jobs[j];
                jobs[j] = jobs[j+1];
                jobs[j+1] = temp;
            }
        }
    }
}

// ---------------------------------------------------------
// 7. Banker's Logic & 8. Deadlock Avoidance
// ---------------------------------------------------------
bool is_safe_state(Job jobs[], int n, int available_res) {
    int work = available_res;
    bool finish[10] = {false};
    int completed = 0;
    
    while(completed < n) {
        bool found = false;
        for(int i = 0; i < n; i++) {
            int need = jobs[i].max_res - jobs[i].alloc_res;
            if(!finish[i] && need <= work) {
                work += jobs[i].alloc_res; // Process finishes and returns allocated resources
                finish[i] = true;
                found = true;
                completed++;
            }
        }
        if(!found) {
            return false; // Deadlock would occur (Unsafe State)
        }
    }
    return true; // Safe State
}

int main() {
    // Register the signal handler
    signal(SIGINT, handle_signal);

    // Initialize Shared Memory (IPC)
    shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    shared_data = (SharedData *)shmat(shmid, NULL, 0);
    shared_data->ledger_balance = 5000; // Starting balance

    // Initialize Synchronization Tools
    // 1 indicates the semaphore is shared between processes
    sem_init(&shared_data->mutex, 1, 1);     // Mutex lock (1 = unlocked)
    sem_init(&shared_data->db_slots, 1, 2);  // Semaphore (Allow 2 concurrent connections)

    // Define pending jobs (Transactions)
    int num_jobs = 3;
    Job jobs[3] = {
        {1, 4, 3, 1}, // Job 1: 4s burst, needs 3 total res, holds 1
        {2, 1, 2, 1}, // Job 2: 1s burst, needs 2 total res, holds 1
        {3, 2, 2, 0}  // Job 3: 2s burst, needs 2 total res, holds 0
    };
    int system_available_resources = 2; // Simulated system resources

    printf("\n=== CoreOS Distributed Transaction Manager ===\n");

    // Execute Concept 4: CPU Scheduling
    printf("\n[Scheduler] Organizing jobs using Shortest Job First (SJF)...\n");
    sjf_schedule(jobs, num_jobs);

    // Execute Concepts 7 & 8: Banker's Algorithm and Deadlock Check
    printf("[Banker] Checking system state for Deadlock avoidance...\n");
    if(is_safe_state(jobs, num_jobs, system_available_resources)) {
        printf("[Banker] System is in a SAFE STATE. No deadlocks detected. Proceeding...\n\n");
    } else {
        printf("[Banker] UNSAFE STATE! Potential Deadlock detected. Aborting execution.\n");
        handle_signal(SIGINT);
    }

    // Process Execution
    for(int i = 0; i < num_jobs; i++) {
        // 1. Fork Process
        pid_t pid = fork();
        
        if(pid < 0) {
            perror("Fork failed");
            exit(1);
        } 
        else if(pid == 0) {
            // --- CHILD PROCESS ---
            
            // 6. Semaphore: Wait for an available database slot
            sem_wait(&shared_data->db_slots); 
            
            // 5. Mutex: Lock the ledger so no one else can write to it
            sem_wait(&shared_data->mutex);    
            
            printf("[Worker %d] Starting Job ID %d...\n", getpid(), jobs[i].id);
            int current_balance = shared_data->ledger_balance;
            
            // 3. Sleep: Simulate processing time based on burst time
            sleep(jobs[i].burst_time); 
            
            // Process the transaction
            shared_data->ledger_balance = current_balance - 100;
            printf("[Worker %d] Finished Job ID %d. New Ledger Balance: $%d\n", getpid(), jobs[i].id, shared_data->ledger_balance);

            // Unlock Mutex and release Semaphore
            sem_post(&shared_data->mutex);
            sem_post(&shared_data->db_slots);
            
            // Detach shared memory for the child and exit
            shmdt(shared_data);
            exit(0); 
        }
    }

    // --- PARENT PROCESS ---
    // 2. Wait: Parent waits for all child processes to finish execution
    for(int i = 0; i < num_jobs; i++) {
        wait(NULL);
    }
    
    printf("\n[Master] All processes completed successfully. Triggering safe shutdown...\n");
    // Trigger the signal manually to clean up resources
    handle_signal(SIGINT); 

    return 0;
}
