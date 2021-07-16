#include <TimerOne.h>
#include <LedControl.h>

LedControl led(A3, A5, A4, 1);
const byte CI = 0, BT_SET = 2, BT_UP = 3, BT_DW = 4, SPK = 5;
byte sec = 0;

void setup() {
  led.shutdown(CI, false);
  led.setIntensity(CI, 8);
  for (byte i = 2; i < 5; i++) pinMode(i, INPUT_PULLUP);
  pinMode(SPK, OUTPUT);
  Timer1.initialize(1000000); //1 segundo
  Timer1.attachInterrupt(clk);
}

void loop() {
  ajust();
}

void ajust() {
  int h, m, al_h, al_m, ax_h = 0, ax_m = 0, mod = 3;
  bool pres = false, alarm = false;

  while (1) {
    if (alarm) {
      ax_h = al_h;
      ax_m = al_m;
    } else {
      ax_h = h;
      ax_m = m;
    }
    if (pres) {
      if (!digitalRead(BT_UP) and mod == 0) ax_h += 10;
      else if (!digitalRead(BT_DW) and mod == 0) ax_h -= 10;
      else if (!digitalRead(BT_UP) and mod == 1) ax_h +=  1;
      else if (!digitalRead(BT_DW) and mod == 1) ax_h -=  1;
      else if (!digitalRead(BT_UP) and mod == 2) ax_m += 10;
      else if (!digitalRead(BT_DW) and mod == 2) ax_m -= 10;
      else if (!digitalRead(BT_UP) and mod == 3) ax_m +=  1;
      else if (!digitalRead(BT_DW) and mod == 3) ax_m -=  1;
    }
    if (alarm) {
      al_h = ax_h;
      al_m = ax_m;
      Display(&al_h, &al_m, mod);
      led.setChar(CI, 4, 'A', 0);
      led.setChar(CI, 5, 'L', 0);
    } else {
      h = ax_h;
      m = ax_m;
      Display(&h, &m, mod);
      if (mod < 4) {
        led.setChar(CI, 4, 'H', 0);
        led.setChar(CI, 5, 'd', 0);
      }
    }
    if (!digitalRead(BT_SET) and pres) {
      if (mod-- == 0) {
        alarm = !alarm;
        if (alarm) mod = 3;
        else mod = 4;
      }
    }
    pres = true;

    //Verifica se há botões pressionados
    for (int i = 0; i < 3; i++) if (!digitalRead(i + 2)) pres = false;
    delay(100);
    if (al_m == m and al_h == h and (al_m or al_h) and mod == 4) alarme();
    
    if(m % 15 == 0){
            alarme();
        }else if(m % 30 == 0){
            alarme();
        }else if(h == 0){
            alarme();
        }

  }
}

void Display(int *h, int *m, int mod) {
  static int blk = 0;

  if (mod < 4) {
    if (++blk > 7) blk = 0;
  } else {
    blk = 0;
  }

  if (*h < 0) *h += 10 + (mod == 0) * 20;
  if (*m < 0) *m += 10 + (mod == 2) * 50;
  if (sec > 59) {
    sec = 0;
    *m += 1;
  }
  if (*m > 59) {
    *m = 0;
    *h += 1;
  }
  if (*h > 23) *h = 0;

  led.setDigit(CI, 0, *h / 10, 0);
  led.setDigit(CI, 1, *h % 10, 0);
  led.setDigit(CI, 2, *m / 10, 0);
  led.setDigit(CI, 3, *m % 10, 0);
  led.setDigit(CI, 4, sec / 10, 0);
  led.setDigit(CI, 5, sec % 10, 0);

  if (blk > 3)
    led.setChar(CI, mod, ' ', 0);
}

void clk() {
  sec++;
}

void alarme() {
  if (!(sec % 2)) tone(SPK, 180, 180);
}
