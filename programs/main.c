#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define numberOfPoints 15

typedef enum {false, true} bool;

struct points {
    POINT *points;
    /*
    0: profile picture
    1: Profile Button
    2: old Listing
    3: edit Listing Button
    4: delete Image
    5: Save Button
    6: Save details
    7: Sell Button
    8: Insert Image
    9: Save Image
    10: Next
    11: Others category
    12: New
    13: Mailing & Delivery
    14: Submit */
} allPoints;

struct instructions {
    char **buffer;
    int count;
    int rowSize;

    char* itemName;
    char* itemPrice;
    bool itemDescriptionIsCopy;
} instruction;

POINT getCoordinates() {
    POINT p;
    if (GetCursorPos(&p)) {
        //cursor position now in p.x and p.y
    }
    printf("%d, %d", p.x, p.y);
    return p;
}


void mouseClick(int xcoord, int ycoord) {
    SetCursorPos(xcoord, ycoord);

    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));

    // left mouse button up
    ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

void sendKey(int value) {
    // Create the desired keyboard event

    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = value;
    ip.ki.wScan = 0;
    ip.ki.dwFlags = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    nanosleep((const struct timespec[]){{0, 200000000L}}, NULL);
    // Send the keyboard event to the specified window
    SendInput(1, &ip, sizeof(INPUT));
}

void sendTabs(int numberOfTabs) {
    for (int i = 0; i < numberOfTabs; i++) {
        sendKey(9);
    }
}

void sendCapital(char value) {

    keybd_event(16, 0, 0, 0);

    // Perform test.
    sendKey(value);

    // Release the Control key.
    keybd_event(16, 0, 2, 0);
}

void selectAll() {

    keybd_event(17, 0, 0, 0);

    // Perform test.
    sendKey(65);

    // Release the Control key.
    keybd_event(17, 0, 2, 0);
}

void copy() {

    keybd_event(17, 0, 0, 0);

    // Perform test.
    sendKey(67);

    // Release the Control key.
    keybd_event(17, 0, 2, 0);
}

void paste() {

    keybd_event(17, 0, 0, 0);

    // Perform test.
    sendKey(86);

    // Release the Control key.
    keybd_event(17, 0, 2, 0);
}

void wordToVal(char *word) {
    for (int i = 0; i < strlen(word); i++) {
        if (97 <= (int) word[i] && (int) word[i] <= 122) {
            // Regular character
            sendKey(word[i] - 32);
        } else if (65 <= (int) word[i] && (int) word[i] <= 90) {
            // Capital character
            sendCapital(word[i]);
        } else if (48 <= word[i] && (int) word[i] <= 57) {
            // Number character
            sendKey(word[i]);
        } else if ((32 <= word[i] && word[i] <= 47) || (58 <= word[i] && word[i] <= 64)) {
            // Special character
            if (word[i] == 32) {
                //space
                sendKey(word[i]);
            } else if (word[i] == 33){
                // !
                sendCapital(49);
            }
            else if (word[i] == 38) {
                //&
                sendCapital(55);
            } else if (word[i] == 45) {
                //-
                sendKey(189);
            } else if (word[i] == 46) {
                //full stop
                sendKey(word[i] + 64);
            } else if (word[i] == 58){
                // :
                sendCapital(186);
            }
            else if (word[i] == 63) {
                // ?
                sendCapital(191);
            }
            else if (word[i] == 64) {
                //@
                sendCapital(50);
            } else {
                printf("unknown special character");

            }
        } else {
            printf("unknown value!!");
        }
    }
}


void carousellCalibration() {
    // We make space for the points
    allPoints.points = malloc(numberOfPoints * sizeof(POINT));

    ShellExecuteA(0, 0, "chrome.exe", "https://sg.carousell.com/", 0, SW_SHOWMAXIMIZED);
    sleep(5);

    MessageBox(0, "Is your account logged in yet? If it is, leave your cursor on the left of the screen. If not, leave it on the right", "Step 1", 1);
    sleep(3);
    POINT p = getCoordinates();
    if (p.x > 770) {
        MessageBox(0, "We are going to log you in!", "Step 1B", 1);
        sendTabs(2);
        sendKey(13);
        sleep(3);
        if (instruction.buffer[5][0] == '!') {
            sendTabs(4);
            sendKey(13);
            sleep(3);
            sendTabs(3);
            sendKey(13);
            sleep(5);
        } else {
            sendTabs(1);
            sendKey(13);
        }
    }

    MessageBox(0, "Hover your cursor over your profile picture", "Step 2", 1);
    sleep(3);
    allPoints.points[0] = getCoordinates();
    mouseClick(allPoints.points[0].x, allPoints.points[0].y);
    sleep(1);

    MessageBox(0, "Hover your cursor over the Profile button", "Step 3", 1);
    sleep(3);
    allPoints.points[1] = getCoordinates();
    mouseClick(allPoints.points[1].x, allPoints.points[1].y);
    sleep(3);

    MessageBox(0, "Hover your cursor over the old listing", "Step 4", 1);
    sleep(3);
    allPoints.points[2] = getCoordinates();
    mouseClick(allPoints.points[2].x, allPoints.points[2].y);
    sleep(3);

    MessageBox(0, "Hover your cursor over the edit listing button", "Step 5", 1);
    sleep(3);
    allPoints.points[3] = getCoordinates();
    mouseClick(allPoints.points[3].x, allPoints.points[3].y);
    sleep(3);

    MessageBox(0, "Hover your cursor over the delete image button", "Step 6", 1);
    sleep(3);
    allPoints.points[4] = getCoordinates();
    mouseClick(allPoints.points[4].x, allPoints.points[4].y);
    sleep(3);

    mouseClick(allPoints.points[4].x, allPoints.points[4].y);
    sleep(3);
    wordToVal("moved.jpg");
    sendKey(13);
    sleep(3);

    MessageBox(0, "Hover your cursor over the save button", "Step 7", 1);
    sleep(3);
    allPoints.points[5] = getCoordinates();
    mouseClick(allPoints.points[5].x, allPoints.points[5].y);
    sleep(1);

    sendTabs(3);
    sendKey(8);
    wordToVal("listing moved");

    sendTabs(3);
    selectAll();
    copy();

    sendKey(8);
    wordToVal("Listing moved");
    sendTabs(1);
    sendKey(34);


    MessageBox(0, "Hover your cursor over the Save Details button", "Step 8", 1);
    sleep(3);
    allPoints.points[6] = getCoordinates();
    //mouseClick(allPoints.points[6].x, allPoints.points[6].y);
    sleep(1);


    MessageBox(0, "Hover your cursor over the Sell button", "Step 9", 1);
    sleep(3);
    allPoints.points[7] = getCoordinates();
    mouseClick(allPoints.points[7].x, allPoints.points[7].y);
    sleep(3);

    MessageBox(0, "Hover your cursor over the Insert Image button", "Step 10", 1);
    sleep(3);
    allPoints.points[8]= getCoordinates();
    mouseClick(allPoints.points[8].x, allPoints.points[8].y);
    sleep(3);

    wordToVal("insta.jpg");
    sleep(1);
    sendKey(13);

    MessageBox(0, "Hover your cursor over Save button", "Step 11", 1);
    sleep(3);
    allPoints.points[9] = getCoordinates();
    mouseClick(allPoints.points[9].x, allPoints.points[9].y);

    MessageBox(0, "Hover your cursor over the Next: Choose a category button", "Step 12", 1);
    sleep(3);
    allPoints.points[10] = getCoordinates();
    mouseClick(allPoints.points[10].x, allPoints.points[10].y);

    sleep(3);
    sendTabs(2);
    wordToVal("Electronics & Gadgets - Others");

    MessageBox(0, "Hover your cursor over the Electronics & Gadgets - Others category", "Step 13", 1);
    sleep(3);
    allPoints.points[11] = getCoordinates();
    mouseClick(allPoints.points[11].x, allPoints.points[11].y);

    sleep(3);
    sendTabs(2);
    // Name of item
    wordToVal(instruction.itemName);
    sleep(1);
    sendTabs(1);
    wordToVal(instruction.itemPrice);
    sendTabs(1);

    MessageBox(0, "Hover your cursor over New button", "Step 14", 1);
    sleep(3);
    allPoints.points[12] = getCoordinates();
    mouseClick(allPoints.points[12].x, allPoints.points[12].y);

    sendTabs(1);
    selectAll();
    if (instruction.itemDescriptionIsCopy == true){
        paste();
    }
    else {
        // We now reference the other file
        FILE *file;
        file = fopen("description.txt", "r");
        sleep(3);
        char ch;

        while ((ch = fgetc(file)) != EOF) {
            if (ch == '\n') {
                sendKey(13);
            }
            else {
                char* letter = malloc(1 * sizeof(char));
                letter[0] = ch;
                wordToVal(letter);
                free(letter);
            }
        }
    }
    sendKey(32);
    sendKey(8);

    sendTabs(3);
    MessageBox(0, "Hover your cursor over the Mailing & Delivery button", "Step 15", 1);
    sleep(3);
    allPoints.points[13] = getCoordinates();
    mouseClick(allPoints.points[13].x, allPoints.points[13].y);
    sendKey(34);
    sendKey(40);

    MessageBox(0, "Hover your cursor over the List It button", "Step 16", 1);
    sleep(3);
    allPoints.points[14] = getCoordinates();

    MessageBox(0, "Nice! Give us some time to finish calibrating", "Working", 1);
    FILE *file;
    file = fopen("data.txt", "w");
    for (int i = 0; i < instruction.count; i++){
        if (i == 3){
            for (int i = 0; i < numberOfPoints; i++){
                if (i == numberOfPoints-1){
                    fprintf(file, "%d,%d,\n", (int) allPoints.points[i].x, (int) allPoints.points[i].y);
                }
                else {
                    fprintf(file, "%d,%d, ", (int) allPoints.points[i].x, (int) allPoints.points[i].y);
                }
            }
        }
        else if (i == 4 || i == 5){
            for (int j = 0; j < 61; j++){
                if (instruction.buffer[i][j] != '\n') {
                    fprintf(file, "%c", instruction.buffer[i][j]);
                } else {
                    fprintf(file, "\n");
                    break;
                }
            }
        }
        else {
            for (int j = 0; j < instruction.rowSize; j++) {
                if (instruction.buffer[i][j] != '\n') {
                    fprintf(file, "%c", instruction.buffer[i][j]);
                } else {
                    fprintf(file, "\n");
                    break;
                }
            }
        }
    }
    MessageBox(0, "Finished! You can now use automation in the future", "Done", 1);
}

void carousellAutomation() {
    // First we get the saved points from the buffer
    char **tempBuffer = malloc(numberOfPoints * sizeof(char *));
    for (int i = 0; i < numberOfPoints; i++) {
        tempBuffer[i] = malloc(12 * sizeof(char));
        for (int j = 0; j < 12; j++) {
            tempBuffer[i][j] = ' ';
        }
    }

    char *firstToken = strtok(instruction.buffer[3], " ");
    strcpy(tempBuffer[0], firstToken);
    printf("%s   ", tempBuffer[0]);

    for (int i = 1; i < numberOfPoints; i++) {
        char *token = strtok(NULL, " ");
        strcpy(tempBuffer[i], token);
        printf("%s   ", tempBuffer[i]);
    }

    allPoints.points = malloc(numberOfPoints * sizeof(POINT));
    printf("\n");
    for (int i = 0; i < numberOfPoints; i++) {
        char *charX = strtok(tempBuffer[i], ",");
        char *charY = strtok(NULL, ",");
        allPoints.points[i].x = atoi(charX);
        allPoints.points[i].y = atoi(charY);
        printf("%d-%d   ", (int) allPoints.points[i].x, (int) allPoints.points[i].y);
    }


    // Now the points are in the desired positions, we can start to sell!
    ShellExecuteA(0, 0, "chrome.exe", "https://sg.carousell.com/", 0, SW_SHOWMAXIMIZED);
    sleep(5);

    // Enter the profile page
    mouseClick(allPoints.points[0].x, allPoints.points[0].y);
    sleep(1);
    mouseClick(allPoints.points[1].x, allPoints.points[1].y);
    sleep(4);

    // Click the old listing
    mouseClick(allPoints.points[2].x, allPoints.points[2].y);
    sleep(5);

    // Click the edit listing
    mouseClick(allPoints.points[3].x, allPoints.points[3].y);
    sleep(5);

    // Click the delete and change image
    mouseClick(allPoints.points[4].x, allPoints.points[4].y);
    sleep(2);
    mouseClick(allPoints.points[4].x, allPoints.points[4].y);
    sleep(2);
    wordToVal("moved.jpg");
    sendKey(13);
    sleep(2);

    // Click save image
    mouseClick(allPoints.points[5].x, allPoints.points[5].y);
    sleep(2);

    sendTabs(3);
    sendKey(8);
    wordToVal("listing moved");

    sendTabs(3);
    selectAll();
    copy();

    sendKey(8);
    wordToVal("Listing moved");
    sendTabs(1);
    sendKey(34);
    sleep(2);
    // Click Save changes to listing
    mouseClick(allPoints.points[6].x, allPoints.points[6].y);
    sleep(5);

    // Click Sell
    mouseClick(allPoints.points[7].x, allPoints.points[7].y);
    sleep(5);

    // Click Insert Image
    mouseClick(allPoints.points[8].x, allPoints.points[8].y);
    sleep(2);
    wordToVal("insta.jpg");
    sendKey(13);
    sleep(2);

    // Click Save Image
    mouseClick(allPoints.points[9].x, allPoints.points[9].y);
    sleep(3);

    // Click Next
    mouseClick(allPoints.points[10].x, allPoints.points[10].y);
    sleep(3);
    sendTabs(2);
    wordToVal("Electronics & Gadgets - Others");

    // Click Electronics & Gadgets - Others
    mouseClick(allPoints.points[11].x, allPoints.points[11].y);
    sleep(3);
    sendTabs(2);
    wordToVal(instruction.itemName);
    sleep(1);
    sendTabs(1);
    wordToVal(instruction.itemPrice);
    sendTabs(1);
    sleep(1);

    // Click New
    mouseClick(allPoints.points[12].x, allPoints.points[12].y);
    sleep(3);
    sendTabs(1);

    selectAll();
    if (instruction.itemDescriptionIsCopy == true){
        paste();
    }
    else {
        // We now reference the other file
        FILE *file;
        file = fopen("description.txt", "r");
        sleep(3);
        char ch;

        while ((ch = fgetc(file)) != EOF) {
            if (ch == '\n') {
                sendKey(13);
            }
            else {
                char* letter = malloc(1 * sizeof(char));
                letter[0] = ch;
                wordToVal(letter);
                free(letter);
            }
        }
    }
    sendKey(32);
    sendKey(8);

    sendTabs(3);
    sleep(1);
    // Click Mailing & Delivery
    mouseClick(allPoints.points[13].x, allPoints.points[13].y);
    sendKey(34);
    sendKey(40);
    sleep(2);
    // Click Submit
    mouseClick(allPoints.points[14].x, allPoints.points[14].y);
    MessageBox(0, "You should be done!", "Finished", 1);
}


int main(int argc, char *argv[]) {
    // First we read the file

    FILE *file;
    file = fopen("data.txt", "r");
    char ch;
    int count = 0;
    int currentChars = 0;
    int rowSize = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++;
            if (currentChars > rowSize) {
                rowSize = currentChars;
            }
            currentChars = 0;
        }
        currentChars++;
    }
    printf("Number of Lines: %d ,rowSize: %d \n", count, rowSize);
    assert(count == 6);

    // We initialize a buffer
    instruction.count = count;
    instruction.rowSize = rowSize;
    instruction.buffer = malloc(count * sizeof(char *));
    for (int i = 0; i < count; i++) {
        instruction.buffer[i] = malloc(rowSize * sizeof(char));
        for (int j = 0; j < rowSize; j++) {
            instruction.buffer[i][j] = ' ';
        }
    }


    // We put the instructions into a buffer
    rewind(file);


    instruction.buffer[count][rowSize];
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < rowSize; j++) {
            instruction.buffer[i][j] = ' '; // We first pad the whole buffer with spaces
        }
        char ch;
        int index = 0;
        while ((ch = fgetc(file)) != '\n') {
            instruction.buffer[i][index] = ch;
            index++;
        }
        instruction.buffer[i][index] = '\n';
    }

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < rowSize; j++) {
            printf("%c", instruction.buffer[i][j]);
        }
        printf("\nend of instruction \n");
    }

    // Getting some details
    strtok(instruction.buffer[4], ":");
    char* itemName = strtok(NULL, "\n");
    printf("%s", itemName);
    instruction.itemName = malloc(strlen(itemName) * sizeof(char));
    strcpy(instruction.itemName, itemName);

    strtok(instruction.buffer[5], ":");
    char* itemPrice = strtok(NULL, "\n");
    printf("\n%s", itemPrice);
    instruction.itemPrice = malloc(strlen(itemPrice) * sizeof(char));
    strcpy(instruction.itemPrice, itemPrice);

    free(instruction.buffer[4]);
    free(instruction.buffer[5]);
    instruction.buffer[4] = malloc(61 * sizeof(char));
    instruction.buffer[5] = malloc(61 * sizeof(char));

    strcpy(instruction.buffer[4], "Item Name:");
    strcat(instruction.buffer[4], instruction.itemName);
    strcat(instruction.buffer[4], "\n");
    strcpy(instruction.buffer[5], "Item Price:");
    strcat(instruction.buffer[5], instruction.itemPrice);
    strcat(instruction.buffer[5], "\n");

    printf("%s",instruction.buffer[4]);
    printf("%s",instruction.buffer[5]);
    //printf("%s", instruction.itemName);
    //printf("%s", instruction.itemPrice);

    instruction.itemDescriptionIsCopy = true;

    // Find if user wants to calibrate or automate
    printf("%c", instruction.buffer[1][0]);
    if (instruction.buffer[1][0] == '!') {
        printf("We are going to calibrate your account!");
        carousellCalibration();
    } else if (instruction.buffer[1][0] != '!' && instruction.buffer[3][0] == 'N') {
        MessageBox(0, "We cannot start automating without calibration!", "Error", 1);
        return 1;
    } else {
        printf("Automation process started");
        carousellAutomation();
    }

    // Exit normally
    return 0;
}