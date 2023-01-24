#include <iostream> // cout
#include <sys/ipc.h> // ftok
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl
#include <unistd.h> // sleep
#include <string.h> // strcpy
#include <curl/curl.h> // curl
using namespace std; // for cout, cin, string, etc

// global array of keys
char keys[8][20]={"numberpad/data", "lockind/data", "buzzind/data", 
"remotecontrol/data", "ledind/data", "fanind/data","button-feed/data",
"doorind/data"}; // numberpad=0, lock=1, buzz=2, remote=3, led=4, fan=5, system=6, door=7
char blynkkeys[5][4]={"0","1","2","3","4"}; // system=0, door=1, fan=2, light=3, close=4
// numberpad=0, lock=1, buzz=2, remote=3, led=4, fan=5, system=6, door=7
char on[10]="1", off[10]="0", reset[10]="*"; // on=1, off=0, reset=*
int lightind=0,fanind=0,state=0; // lightind=0, fanind=0

// define all the functions
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp); // write callback
int getchar(string str); // get char from string
int readada(CURL *curl, char key[20]); // read from adafruit
void writeada(CURL *curl, char key[20], char value[10]); // write to adafruit
string blynkread(CURL *curl, char key[4]); // read from blynk
void blynkwrite(CURL *curl, char key[4], char value[4]); // write to blynk
void sendemail(); // send email
void adddigit(int passcode[3], int num);  // add digit to passcode
void printpasscode(int passcode[3]); // print passcode
int checkpass(CURL *curl1, CURL *curl2, char *str); // check passcode
int checksystem(CURL *curl1); // check system
void intialize(int passcode[3]); // intialize passcode
void opendoor(char *str, CURL *curl2); // open door
void light(char *str, CURL *curl2); // turn on/off light
void fan(char *str, CURL *curl2); // turn on/off fan
void close(char *str, CURL *curl1 , CURL *curl2, int shmid); // close system
void turnoffall(CURL *curl2); // turn off all web dashboard
void turnoffallblynk(CURL *curl1); // turn off all mobile app
int main(){// main function    
    int systemstate = 0,num=0; // systemstate=0, num=0
    CURL *curl1 ,*curl2; // curl1 for read, curl2 for write
    curl1 = curl_easy_init();// initialize curl1
    curl2 = curl_easy_init(); // initialize curl2
    key_t key = ftok("shmfile",65); // key for shared memory
    int shmid = shmget(key,1024,0666|IPC_CREAT); // shared memory id
    char *str = (char*) shmat(shmid,(void*)0,0); // shared memory
    strcpy(str,""); // initialize shared memory
    try{
        cout << "--------------------------------------------\n";
        cout << "\nHello ,This is SMART HOME SYSTEM\nLet's check if the system is on\n ";
        cout << "Checking the system state\n Loading...........\n";
        systemstate = checksystem(curl1); // check system
        if (systemstate==1) { // check system
            state=1;
            cout << "\nWeb Dashboard System is ON\n"; // system is on
            cout << "For security reasons, please enter the passcode (3 digits).........\n";
            if (checkpass(curl1,curl2,str) == 1) { // check passcode
                cout << "Password is correct\n"; // password is correct
                sendemail(); // send email
                strcpy(str,"8"); // send 8 to shared memory
                cout << "Welcome to the system\n"; 
                cout << "You can now control the system through the Remote Control, where\n";
                cout << "1- Open the door\n";//16
                cout << "2- Turn on/off the light\n";// 17
                cout << "3- Turn on/off the fan\n"; //18
                cout << "4- Close the System\n";  //20
                cout << "--------------------------------------------\n";
            while (1) { // while loop
                num = readada(curl1, keys[3]); // read from remote control
                if (num== 16) opendoor(str, curl2); // open door
                else if (num == 17) light(str, curl2); // turn on/off light
                else if (num == 18) fan(str, curl2); // turn on/off fan
                else if (num == 20) close(str, curl1, curl2, shmid); // close all devices
                else if (num == 26){
                writeada(curl2,keys[1], off);
               writeada(curl2,keys[3], off); // reset passcode
                main();
                }
            }
        }
        else{
            cout << "Password is incorrect\nPlease try again and turn on the system\n";
            writeada(curl2, keys[6], off);  // turn off system
        }
    }  
        else if (systemstate==2) { // system is off
            state=0;
            sendemail(); // send email
            string oldlight="0", oldfan="0"; // olddoor=2, oldlight=2, oldfan=2
            string newlight, newfan; // door, light, fan
            cout << "\nMobile App System is ON\n"; // system is on
            cout << "Welcome to the system\n";
            cout << "You can now control the system through the Buttons, where\n";
            cout << "Door Button: Open the door\n";
            cout << "Light Button: Turn on/off the light\n";
            cout << "Fan Button: Turn on/off the fan\n";
            cout << "Close Button: Close the System\n";
            cout << "--------------------------------------------\n";
            strcpy(str,"8"); // send 8 to shared memory
            while (1) { // while loop
                newlight = blynkread(curl1, blynkkeys[3]); // read from mobile app
                newfan = blynkread(curl1, blynkkeys[2]);
                if (blynkread(curl1, blynkkeys[1]) == on ) {
                    opendoor(str, curl2); // open door
                    blynkwrite(curl1, blynkkeys[1], off); // turn off door button
                }
                else if (newlight != oldlight) {
                    light(str, curl2); // turn on/off light
                    oldlight = newlight; // update oldlight
                }
                else if (newfan != oldfan) {
                    fan(str, curl2); // turn on/off fan
                    oldfan = newfan; // update oldfan
                }
                else if (blynkread(curl1, blynkkeys[4]) == on) close(str, curl1, curl2, shmid); // close all devices
            }
        }
    }
    // ctrl + c
    catch (int e) { // catch exception
        cout << "Exiting\n";
        shmdt(str); // detach shared memory
        shmctl(shmid,IPC_RMID,NULL); // remove shared memory
        curl_easy_cleanup(curl1); // cleanup curl1
        curl_easy_cleanup(curl2); // cleanup curl2
        exit(0); // exit
        }
    cout <<"\nSystem is locked\n"; // system is locked
    cout << "Exiting\n";
    shmdt(str); // detach shared memory
    shmctl(shmid,IPC_RMID,NULL); // remove shared memory
    curl_easy_cleanup(curl1); // cleanup curl1
    curl_easy_cleanup(curl2); // cleanup curl2
    exit(0); // exit
    return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){ // write callback
    ((std::string*)userp)->append((char*)contents, size * nmemb); // Intialize string for read buffer
    return size * nmemb; // return size * nmemb
}

void writeada(CURL *curl, char key[20], char value[10]) { // write to adafruit
    char url[100]="https://io.adafruit.com/api/v2/eyadgad/feeds/"; // url
    strcat(url,key); // concatenate url and key
    char val[20] = "value="; // value
    strcat(val,value); // concatenate value and value
    struct curl_slist *headers = NULL; // headers
    string readBuffer; // read buffer
    headers = curl_slist_append(headers, "X-AIO-Key: aio_LIyb46hksA3M0pDkXfDAto2myMtG"); // append headers
    usleep(200000); // sleep
    curl_easy_setopt(curl, CURLOPT_URL, url); // set url
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // set headers      
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, val); // set value
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"); // set post
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set write callback
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // set read buffer
    curl_easy_perform(curl); // perform
}

int readada(CURL *curl, char key[20]) { // read from adafruit
    usleep(200000); // sleep
    string str ;   // string to read from adafruit
    char url[100]="https://io.adafruit.com/api/v2/eyadgad/feeds/"; // url for adafruit
    curl_easy_setopt(curl, CURLOPT_URL, strcat(url, key)); // set url
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // set follow location to 1
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set write callback
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str); // set read buffer
    curl_easy_perform(curl);    // perform
    //cout << str << endl;
    return getchar(str);    // return the value
}

string blynkread(CURL *curl, char key[4]) {
    char url[100]="https://blynk.cloud/external/api/get?token=aah-HzqSQJepghpcSP4rVIwlWvu07kH0&v";
    strcat(url,key);
    string out;
    usleep(100000); 
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    curl_easy_perform(curl);
    return out;
}

void blynkwrite(CURL *curl, char key[4], char value[4]) {
    char url[100]="https://blynk.cloud/external/api/update?token=aah-HzqSQJepghpcSP4rVIwlWvu07kH0&v";
    strcat(url,key);
    strcat(url,"=");
    strcat(url,value);
    usleep(100000); 
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_perform(curl);
}

int getchar(string str) { // get the value from the string
      for (int i = 0; i < str.length(); i++) // loop through the string 
        if (str[i] == 'v' && str[i+1] == 'a' && str[i+2] == 'l' ){ // if the string contains "val"
            if (str[i+8] == '*' || str[i+8] == '#') return 11; // if equals "*" or "#" then return 11
            // if equals "'" then return 10
            if (str[i+9] == '"') return int(str[i+8])-48; // else return the value
            else return (int(str[i+8])-48)*10 + int(str[i+9])-48; // else return the value
    
        }  
}

void sendemail(){ // send email
    string sender = "bratsproject@gmail.com"; // sender email
    string receiver = "eyad.gad@outlook.com"; // receiver email
    string subject = "Smart Home"; // subject
    string message = "Hello Eyad\nThis is an alert from your smart home system. Someone entered the passcode to your home. Please check your home security system."; // message
    string gapp ="szpyxkryfsmfzref";  // google app password
    string ignoreoutput = " > /dev/null 2>&1"; // ignore output
    string command = "curl --url 'smtps://smtp.gmail.com:465' --ssl-reqd --mail-from '" + sender + "' --mail-rcpt '" + receiver + "' --upload-file - --user '" + sender + ":" + gapp + "' " + ignoreoutput; // command     
    FILE *mail = popen(command.c_str(), "w"); // open command
    fprintf(mail,"To: %s \n", receiver.c_str()); // print to receiver    
    fprintf(mail,"From: %s \n", sender.c_str()); // print from sender
    fprintf(mail,"Subject: %s \n", subject.c_str()); // print subject
    fprintf(mail,"\n"); // blank line to divide headers from body, see RFC5322
    fprintf(mail,"%s \n", message.c_str()); // print message
    pclose(mail); // close command
}

void adddigit(int passcode[3], int num) { // add digit to passcode
    // if num is in passcode, do nothing
    for (int i = 0; i < 3; i++) { // loop through passcode
        if (passcode[i] == num) return; // if num is in passcode, return
    }
    // if num is not in passcode, add it to the first -1 in passcode 
    for (int i = 0; i < 3; i++) { // loop through passcode
        if (passcode[i] == -1) { // if passcode[i] is -1
            passcode[i] = num; // add num to passcode[i]
            printpasscode(passcode);  // print passcode
            return; // return
        }
    }


}

void printpasscode(int passcode[3]) { // print passcode
    for (int i = 0; i < 3; i++) cout  << "  "<< passcode[i] ; // print passcode
    cout << endl;
}

int checkpass(CURL *curl1, CURL *curl2, char *str) { // check passcode
    int passcode[3]={-1,-1,-1}; // initialize passcode
    int trials=0; // initialize trials
    //writeada(curl2,keys[0], reset);
    while(1){ // loop
        int num = readada(curl1, keys[0]); // read from adafruit
        //cout << "num is " << num << endl;
        if (num ==11) intialize(passcode); // if num is 11, initialize passcode
        else if (passcode[2]!=-1){ // if passcode is full
            writeada(curl2,keys[0], reset); // reset passcode
            if (passcode[0]==1 && passcode[1]==2 && passcode[2]==4) { // if passcode is correct
               writeada(curl2,keys[1], on); // turn on lock
               return 1; // return 1
            }
            else {
                cout << "Password is incorrect" << endl; // print incorrect
                trials++; // increment trials
                if (trials==2) { // if trials is 3
                    writeada(curl2,keys[2], on); // turn on alarm
                    strcpy(str,"9"); // set str to 9
                    usleep(3000000); // wait 3 seconds
                    writeada(curl2,keys[2], off); // turn off alarm
                    strcpy(str,"0"); // set str to 0
                    return 0; // return 0
                }
            }
        }
        else adddigit(passcode,num); // if passcode is not full, add digit to passcode
    }
    return 0;
}

int checksystem(CURL *curl1){ // check system
    for (int i = 0; i < 5; i++) { // loop 5 times
        if (readada(curl1, keys[6]) == 1) return 1; // if web system is on, return 1
        else if (blynkread(curl1, blynkkeys[0]) == "1") return 2; // if mobile system is on, return 2
        usleep(1000000); // wait 1 second
    }
    return 0;
}

void intialize(int passcode[3]) { // initialize passcode
    for (int i = 0; i < 3; i++)// loop through passcode  
    passcode[i] = -1;  // set passcode[i] to -1

}

void opendoor(char *str, CURL *curl2) { // open door
    if(state)writeada(curl2,keys[3], off); // reset remote
    cout <<"Opening the door..........\n"; // print opening door
    if(state)writeada(curl2, keys[7], on); // turn on door
    strcpy(str,"1"); // send 1 t0 shared memory
    usleep(3000000); // wait 3 seconds
    if(state)writeada(curl2, keys[7], off); // turn off door
    strcpy(str,"7"); // send 7 to shared memory
}

void light(char *str, CURL *curl2) { // turn on light
    if(state)writeada(curl2,keys[3], off); // reset remote
    if (lightind == 0) { // if light is off
        cout << "Turning on the light..........\n"; // print turning on light
        if(state)writeada(curl2,keys[4], on); // turn on light
        strcpy(str,"2"); // send 2 to shared memory
        lightind = 1; // set lightind to 1
    }
    else { // if light is on
        cout << "Turning off the light..........\n"; // print turning off light
        if(state)writeada(curl2,keys[4], off); // turn off light
        strcpy(str,"3"); // send 3 to shared memory
        lightind = 0; // set lightind to 0
    }
}

void fan(char *str, CURL *curl2) { // turn on fan
    if(state)writeada(curl2,keys[3], off); // reset remote
    if (fanind == 0) { // if fan is off
        cout << "Turning on the fan..........\n"; // print turning on fan
        strcpy(str,"4"); // send 4 to shared memory
        if(state)writeada(curl2,keys[5], on); // turn on fan
        fanind = 1; // set fanind to 1
    }
    else { // if fan is on
        cout << "Turning off the fan..........\n"; // print turning off fan
        strcpy(str,"5"); // send 5 to shared memory
        writeada(curl2,keys[5], off); // turn off fan
        fanind = 0; // set fanind to 0
    }
}
void close(char *str, CURL *curl1, CURL *curl2, int shmid) { // close all devices
    cout << "CLosing all the devices..........\n"; // print closing all devices
    strcpy(str,"6"); // send 6 to shared memory
    if(state)turnoffall(curl2); // turn off all devices
    else turnoffallblynk(curl1); // turn off all devices
    cout << "Exiting\n"; // print exiting
    cout << "--------------------------------------------\n"; 
    shmdt(str); // detach shared memory
    shmctl(shmid,IPC_RMID,NULL); // remove shared memory
    curl_easy_cleanup(curl1); // cleanup curl1
    curl_easy_cleanup(curl2); // cleanup curl2
    exit(0); // exit
}

void turnoffall(CURL *curl2) { // turn off all devices in adafruit
    writeada(curl2,keys[1], off); // turn off lock
    writeada(curl2,keys[2], off); // turn off buzz
    writeada(curl2,keys[3], off); // reset remote
    writeada(curl2,keys[4], off); // turn off light
    writeada(curl2,keys[5], off); // turn off fan
    writeada(curl2,keys[6], off); // turn off system
}
void turnoffallblynk(CURL *curl1) { // turn off all devices in blynk
    blynkwrite(curl1, blynkkeys[1], off); // turn off door button
    blynkwrite(curl1, blynkkeys[2], off); // turn off light button
    blynkwrite(curl1, blynkkeys[3], off); // turn off fan button
    blynkwrite(curl1, blynkkeys[4], off); // turn off close button
    blynkwrite(curl1, blynkkeys[0], off); // turn off system button
}
