// IPC using shared memory
// Path: msg/client.cpp
#include <iostream> // cout, endl, etc.
#include <sys/ipc.h> // ftok(), shmget(), shmat(), shmdt(), shmctl()
#include <sys/shm.h> // shmget(), shmat(), shmdt(), shmctl()
#include <unistd.h> // sleep()
#include <string.h> // strlen()
// Jetson Nano GPIO
#include <JetsonGPIO.h>
using namespace std; // cout, endl, etc.
using namespace GPIO; // gpioExport(), gpioUnexport(), etc.



using namespace std; // cout, endl, etc.
int main() { // main function
    // set mode to BOARD
    setmode(BOARD);
    int light=19, fan=18, door=16,system=13,systemoff=15, buzz=12; // GPIO pin numbers
    setup(light, OUT, LOW); // set the light pin to output and low
    setup(fan, OUT, LOW); // set the fan pin to output and low
    setup(door, OUT, LOW); // set the door pin to output and low
    setup(system, OUT, LOW); // set the system pin to output and low
    setup(buzz, OUT, LOW); // set the buzzer pin to output and low
    setup(systemoff, OUT, HIGH); // set the systemoff pin to output and low


    key_t key = ftok("shmfile",65); // ftok to generate unique key
    int shmid = shmget(key,1024,0666|IPC_CREAT); // shmget returns an identifier in shmid
    char *str = (char*) shmat(shmid,(void*)0,0); // shmat to attach to shared memory
    char oldstr='0'; // oldstr to store the previous value of str
    while (true) { // infinite loop
        if (str[0] == '1' && oldstr != '1') { // if str[0] is 1 and oldstr is not 1
            cout << "Opening the door" << endl;
            output(door, HIGH); // set the door pin to high
            oldstr = '1'; // set oldstr to 1            
        }
        else if (str[0] == '2' && oldstr != '2') { // if str[0] is 2 and oldstr is not 2
            cout << "Turning on the light" << endl; // print turning on the light
            output(light, HIGH); // set the light pin to high
            oldstr = '2'; // set oldstr to 2
        }
        else if (str[0] == '3' && oldstr != '3') { // if str[0] is 3 and oldstr is not 3
            cout << "Turning off the light" << endl; // print turning off the light
            output(light, LOW); // set the light pin to low
            oldstr = '3'; // set oldstr to 3
        }
        else if (str[0] == '4' && oldstr != '4') { // if str[0] is 4 and oldstr is not 4
            cout << "Turning on the fan" << endl; // print turning on the fan
            output(fan, HIGH); // set the fan pin to high
            oldstr = '4'; // set oldstr to 4
        }
        else if (str[0] == '5' && oldstr != '5') { // if str[0] is 5 and oldstr is not 5
            cout << "Turning off the fan" << endl; // print turning off the fan
            output(fan, LOW); // set the fan pin to low
            oldstr = '5'; // set oldstr to 5
        }
        else if (str[0] == '6' && oldstr != '6') { // if str[0] is 6 and oldstr is not 6
            cout << "Closing the system" << endl; // print turning off the system
            output(system, LOW); // set the system pin to low
            output(systemoff, HIGH); // set the systemoff pin to high
            oldstr = '6'; // set oldstr to 6
            break; // break the loop
        }
        else if (str[0] == '7' && oldstr != '7') { // if str[0] is 8 and oldstr is not 8
            cout << "Closing the door" << endl; // print closing the door
            output(door, LOW); // set the door pin to low
            oldstr = '7'; // set oldstr to 8
        }
        else if (str[0] == '8' && oldstr != '8') { // if str[0] is 7 and oldstr is not 7
            cout << "Turning on the system" << endl; // print turning on the system
            output(system, HIGH); // set the system pin to high
            output(systemoff, LOW); // set the systemoff pin to low
            oldstr = '8'; // set oldstr to 7
        }
        else if (str[0] == '9' && oldstr != '9') { // if str[0] is 9 and oldstr is not 9
            cout << "Turning on the buzzer" << endl; // print turning on the buzzer
            output(buzz, HIGH); // set the buzzer pin to high
            oldstr = '9'; // set oldstr to 9
        }
        else if (str[0] == '0' && oldstr != '0') { // if str[0] is 0 and oldstr is not 0
            cout << "Turning off the buzzer" << endl; // print turning off the buzzer
            output(buzz, LOW); // set the buzzer pin to low
            oldstr = '0'; // set oldstr to 0
            break; // break the loop
        }

    }
        
    cleanup(); // cleanup the GPIO pins 
    shmdt(str); // detach from shared memory   
    return 0;

}

