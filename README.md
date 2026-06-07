# CoreOS: Distributed Transaction Manager (DTM)

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Kali](https://img.shields.io/badge/Kali_Linux-557C94?style=for-the-badge&logo=kali-linux&logoColor=white)

**CoreOS DTM** is a low-level Operating System simulation project written in C. It models how modern operating systems manage concurrent tasks efficiently and safely by handling process creation, CPU scheduling, resource allocation, and inter-process synchronization using the Linux kernel's core POSIX libraries.

**Author:** Abdullah Falak  
**Program:** BSCS (5th Semester) Operating Systems Project  
**Environment:** Kali Linux / POSIX-compliant UNIX  

---

## 📖 Project Scenario
Imagine a banking server receiving hundreds of transaction requests simultaneously. If two requests try to deduct money from the same account at the exact same millisecond, data corruption occurs. This project simulates the underlying Operating System mechanisms required to prevent that. It takes a batch of transactions, schedules them optimally, checks if running them will crash the system (Deadlock), and then executes them securely using isolated memory locks.

---

## 🧠 Core Operating System Concepts Implemented
This project seamlessly integrates 10 fundamental OS concepts into a single cohesive application:

1. **Fork:** Master process spawns independent Worker child processes.
2. **Wait:** Master halts execution until all Worker processes complete.
3. **Sleep:** Simulates CPU execution (burst) time.
4. **CPU Scheduling Algorithm:** Implements **Shortest Job First (SJF)**.
5. **Mutex:** Binary semaphore locking the shared memory ledger.
6. **Semaphore:** Counting semaphore limiting concurrent database connections.
7. **Banker's Logic:** Pre-assesses resource allocation for safety.
8. **Deadlock Avoidance:** Denies execution if an unsafe state is detected.
9. **Msg Signal:** Catches `SIGINT` (Ctrl+C) for safe IPC cleanup.
10. **IPC (Inter-Process Communication):** Utilizes POSIX Shared Memory (`shmget`, `shmat`).

---

## 📂 Repository Structure
```text
├── main.c                                  # Main C source code file
├── CoreOS_DTM_Project_Report.docx          # Detailed academic project documentation
└── README.md                               # Project overview and execution guide

```
## ⚙️ Prerequisites
To run this project, you need a Linux environment with the GNU Compiler Collection (GCC) installed.
```bash
gcc --version

```
## 🚀 Create, Compile & Execute
**1. Create the Source File** Open your terminal and use the nano text editor to create the C file:
```bash
nano main.c

```
*(Paste the project code into this file. Save by pressing Ctrl + O, hit Enter to confirm, and exit with Ctrl + X.)*
**2. Compile the Code** You must link the POSIX threads library (-pthread) when compiling because the project utilizes semaphores and mutexes.
```bash
gcc main.c -o core_project -pthread

```
**3. Execute the Project** ```bash
./core_project
```

---

## 📈 Future Enhancements
While this project satisfies the core requirements for an OS simulation, future iterations could include:
* Transitioning from multi-processing (`fork`) to multi-threading (`pthreads`) for lower memory overhead.
* Implementing Dynamic Memory Allocation for handling an infinite queue of incoming jobs.
* Adding a logging mechanism to write all transaction outcomes to a permanent `.txt` file.

```
