/**************************************/
//////////////SNEK//////////vondage2020
/**************************************/
//no additional libraries required

//JOYSTICK
/**************************************/
#define joyX A0   //joystick pins
#define joyY A1
int xValue, yValue; //joystick readings

//converts joystic input into values 0-4
int getMove(int x, int y) {

  int dir = 0;
  if (x <= 2) {dir = 3;} //down
  else if(y <= 2) {dir = 4;} //left
  else if(x >= 8) {dir = 1;} //up
  else if(y >= 8) {dir = 2;} //right
  else {dir = 0;}
  return dir;
}

//Initial Conditions & Variables
/**************************************/
int m = 0; //next move
int p = 2; //previous move
int X = 3; //x position
int Y = 0; //y position

int movecount = 0;
int failcount = 0;
int score = 0;
int tailsize = 1;
int tailMax = 4;

int fl; //food location

//converting from index to pin number
int convertp[] = {2, 7, 3, 8, 6, 4, 5};
int convertd[] = {13, 12, 11, 10};

//save positions for tail illumination
int historyX[] = {0,0,0,0,0};
int historyY[] = {0,0,0,0,0};

//SET-UP
/**************************************/
//the pins of 4-digit 7-segment display attach to pin2-13 respectively
int a = 2; //0
int b = 3; //2
int c = 4; //5
int d = 5; //6
int e = 6; //4
int f = 7; //1
int g = 8; //3
int dp = 9;
int d4 = 10;
int d3 = 11;
int d2 = 12;
int d1 = 13;

void setup() {
  //set datarate
  Serial.begin(9600);
  //set all the pins of the LED display as output
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dp, OUTPUT);
}

//LOOP FUNCTIONS (win, lose, custom...)
/**************************************/
void winnerLoop() {

  clearLEDs();
  //9's used to terminate arrays
  int U[] = {0,2,5,6,9};
  int W[] = {0,2,3,5,6,9};
  int I[] = {3,9};
  int N[] = {0,2,3,4,6,9};
  
  //i dislike coding pointers
  int* UWIN[] = {U, W, I, N};

  for (int j=0; j<4; j++) {
    for (int i=3; i>=0; i--) {
    
      //if (j!=0) 
      for (int k=0; UWIN[j][k]!=9; k++) {
        digitalWrite(convertd[i], HIGH);
        digitalWrite(convertp[UWIN[j][k]], LOW);
        delay(50);
      }
      delay(100);
      digitalWrite(convertd[i], LOW);
    }
    clearLEDs();
  }
}

void loserLoop() {

  failcount ++;
  if (failcount%10==0) score = score - score/20;
  
  clearLEDs();
  //9's used to terminate arrays
  int F[] = {0,1,3,4,9};
  int A[] = {0,1,2,3,4,5,9};
  int I[] = {1,4,9};
  int L[] = {1,4,6,9};
  int* FAIL[] = {F, A, I, L};
  
  for (int i=0; i<4; i++) {
    for (int j=0; FAIL[i][j]!=9; j++) {
      digitalWrite(convertd[i], HIGH);
      digitalWrite(convertp[FAIL[i][j]], LOW);
      delay(1);
      digitalWrite(convertp[FAIL[i][j]], HIGH);
    }
    digitalWrite(convertd[i], LOW);
  }
  clearLEDs();
}
              
void snekLoop() {
  int snekL[] = {3,2,0,1,3,5,6,4,3};
  int tail = 3;
  for (int i=0; i<4; i++) {
    for (int j=0; j<9; j++) {
      //delay(500);
      X = snekL[j];
      Y = i;
      clearLEDs();
      updateHistory();
      simpleMove(tail);
    }
  }
  //reset initial conditions
  reset(0);
}

void showScore() {

  if (score>9999) {
    winnerLoop();
    score = 9999;
  } else if (score <= 0) score = 0;

  clearLEDs();

  int s = score;
  int scores[] = {0,0,0,0};
  
  int zero[] = {0,1,2,4,5,6,9};
  int one[] = {2,5,9};
  int two[] = {0,2,3,4,6,9};
  int three[] = {0,1,3,4,6,9};
  int four[] = {1,2,3,5,9};
  int five[] = {0,1,3,5,6,9};
  int six[] = {0,1,3,4,5,6,9};
  int seven[] = {0,2,5,9};
  int eight[] = {0,1,2,3,4,5,6,9};
  int nine[] = {0,1,2,3,5,6,9};

  int* NUMBERS[] = {zero, one, two, three, four, five, six, seven, eight, nine};
  
  //get score digit by digit (stops when s==0)
  while(s) {
    for (int i=3; i>=0; i--) {
      scores[i] = s%10;
      s/=10; 
    }
  }

  long startT = millis();
  long endT = startT;
  
  while ((endT-startT) < 5000) {
  
    for (int i=0; i<4; i++) {
      digitalWrite(convertd[i], HIGH);
      for (int j=0; NUMBERS[scores[i]][j]!=9; j++) {
          digitalWrite(convertp[NUMBERS[scores[i]][j]], LOW);
          delay(1);
          digitalWrite(convertp[NUMBERS[scores[i]][j]], HIGH);  
      }
      digitalWrite(convertd[i], LOW);
    }
    clearLEDs();
    endT = millis();
  }  
}

//MAIN LOOP
/**************************************/
//Prepatory logic
bool firstloop = true;
bool safeMode = true;
bool control = false;

/***************************************/
void loop() {

  //Run the intro loop
  if (firstloop) { reset(0); snekLoop(); }
  firstloop = false;

  //Motion can either be randomly generated or controlled by joystick  
  if (control) {
    
    //Default move is last, and if no real move, move is last move
    m = p;
    m = getMove(analogRead(joyX)/102, analogRead(joyY)/102);
    //Serial.print(direct(analogRead(joyX)/102, analogRead(joyY)/102));
    if (m == 0) m = p;
  }
  else {
    m = random(1,5);
    safeMode = true; //safe mode means you can't hit the wall or yourself
  }
  
  //Run main functions
  pickMove();
  delay(1);

  //check for pause
  photopause(analogRead(A7));
  
  //Counting moves for food location (fl) spawn
  movecount++;
  if (movecount==10) {
    movecount = 0;
    fl = random(0,5);  
  } else fl = 5; //fl=5 represents no food spawn

  Serial.print(score);
  Serial.print("\n");
  //endMAIN
}


//MAIN FUNCTIONS
/**************************************/

void pickMove() {
  
  //check if move is valid
  if (!valid()) {
    if (safeMode==false) loserLoop();
    else Serial.print("BAD MOVE\n");
    return;
  }

  //if so, move, and if move fails, don't show
  if (!Move()) { return; }
  
  //update changes
  updateHistory();
  newMove();

  //update old move if actual move
  if (m != 0) p = m;
  
}

//Used for testing and initial loop
void simpleMove(int tail) {
  long startT = millis();
  long endT = startT;
  
  while ((endT - startT) < 100) {
    for (int i=tail-1; i>=0; i--) {
      clearLEDs();
      digitalWrite(convertd[historyY[i]], HIGH);
      digitalWrite(convertp[historyX[i]], LOW);
    }
    endT = millis();
  }  
}

//move with food considerations
void newMove() {

  //loop flicker time (multiplexing)
  long startT = millis();
  long endT = startT;
  while ((endT-startT) < 500) {
    
    spawnFood();

    //light up segments from present to past
    for (int i=tailsize-1; i>=0; i--) {
      clearLEDs();
      digitalWrite(convertd[historyY[i]], HIGH);
      digitalWrite(convertp[historyX[i]], LOW);
      delay(1);
    }
    endT = millis();
  }
}

//resets if food is caught, continues otherwise
void spawnFood() {
  clearLEDs();
  //a food location of 5 represents no food for now
  if (fl==5) return;
  digitalWrite(convertd[fl], HIGH);
  if (historyX[0]==6 && historyX[1]==6) {
    digitalWrite(dp, HIGH);
    reset(1);
  }
  digitalWrite(dp, LOW);
  return;
}


//MOVEMENT VALIDATION
/**************************************/

bool valid() {

  //if (m==0) return true;
  if (((X==0)||(X==1)||(X==2)) && (m==1)) return false; //no up
  if (((X==6)||(X==4)||(X==5)) && (m==3)) return false; //no down
  if (((Y==0) && ((X!=2)||(X!=5))) && (m==4)) return false; //no left
  if (((Y==3) && ((X!=1)||(X!=4))) && (m==2)) return false; //no right
  //no backtracking
  if ((m==1) && (p==3)) return false;
  if ((m==2) && (p==4)) return false;
  if ((m==3) && (p==1)) return false;
  if ((m==4) && (p==2)) return false;
  return true;
}

//Here the movements are actually made (updating X and Y)
bool Move() {

  //top
  if (X==0) {
    if (p==2) { //going right
      if (m==2) {Y++; return true;} //right
      if (m==3) {X=2; return true;} //down
    }
    if (p==4) { //going left
      if (m==3) {X=1; return true;} //down
      if (m==4) {Y--; return true;} //left
    }
  }

 //upper left
  if (X==1) {
    if (p==1) { //going up
      if (m==2) {X=0; return true;} //right
      if (m==4) {X=0; Y--; return true;} //left
    }
    if (p==3) { //going down
      if (m==2) {X=3; return true;} //right
      if (m==3) {X=4; return true;} //down
      if (m==4) {X=3; Y--; return true;} //left
    }
  }

  //upper right
  if (X==2) {
    if (p==1) { //going up
      if (m==2) {X=0; Y++; return true;} //right
      if (m==4) {X=0; return true;} //left
    }
    if (p==3) { //going down
      if (m==2) {X=3; Y++; return true;} //right
      if (m==3) {X=5; return true;} //down
      if (m==4) {X=3; return true;} //left
    }
  }

  //middle
  if (X==3) {
    if (p==2) { //going right
      if (m==1) {X=2; return true;} //up
      if (m==2) {Y++; return true;} //right
      if (m==3) {X=5; return true;} //down
    }
    if (p==4) { //going left
      if (m==1) {X=1; return true;} //up
      if (m==4) {Y--; return true;} //left
      if (m==3) {X=4; return true;} //down
    }
  }

  //lower left
  if (X==4) {
    if (p==1) { //going up
      if (m==1) {X=1; return true;} //up
      if (m==2) {X=3; return true;} //right
      if (m==4) {X=3; Y--; return true;} //left
    }
    if (p==3) { //going down
      if (m==2) {X=6; return true;} //right
      if (m==4) {X=6; Y--; return true;} //left
    }
  }

  //lower right
  if (X==5) {
    if (p==1) { //going up
      if (m==1) {X=2; return true;} //up
      if (m==2) {X=3; Y++; return true;} //right
      if (m==4) {X=3; return true;} //left
    }
    if (p==3) { //going down
      if (m==2) {X=6; Y++; return true;} //right
      if (m==4) {X=6; return true;} //left
    }
  }

  //bottom
  if (X==6) {
    if (p==2) { //going right
      if (m==1) {X=5; return true;} //up
      if (m==2) {Y++; return true;} //right
    }
    if (p==4) { //going left
      if (m==1) {X=4; return true;} //up
      if (m==4) {Y--; return true;} //left
    }
  }

  return false;
}


//Utility Functions
/**************************************/

void photopause(float x) {
  //wait 5 seconds if paused (adjust brightness accordingly)
  if (x < 75) delay(5000);
  //Serial.print(x); Serial.print("\n");
}

void updateHistory() {
  for (int i=tailMax-1; i>0; i--) {
    historyX[i] = historyX[i-1];
    historyY[i] = historyY[i-1];  
  }
  historyX[0] = X;
  historyY[0] = Y;
}

void reset(int choice) {
  clearLEDs();
  if (choice==0 || choice==1) {
    movecount = 0;
    if (choice==1) snekLoop();
    m = 0;
    X = 3;
    Y = 0;
    for (int i=0; i<tailMax; i++) {
      historyX[i] = 0;
      historyY[i] = 0;  
    }
  }
  
  //pure reset
  if (choice==0) { failcount = 0; return; }
  
  //point gaining
  else if (choice==1) {
    score += 10*tailsize;
    failcount = 0;
    if (tailsize < tailMax) { tailsize++; }
    //wow, you've really done it! :)
    else { winnerLoop(); showScore(); }
  }
  //you have hit a wall or yourself, time to face it
  else { loserLoop(); showScore(); }
}

void clearLEDs() {
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  digitalWrite(d4, LOW);
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  digitalWrite(dp, HIGH);
}


//END