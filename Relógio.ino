//Programa : Relogio com Arduino, DS1307 e Display 7 Seg

#include "Wire.h"
#include <LedControl.h>

#define DS1307_ADDRESS 0x68
//controle do LED
LedControl led(A3, A5, A4, 1);

//variáveis do Display, botões e speaker
const byte displayCi = 0, ButtonSet = 2, ButtonUp = 3, ButtonDown = 4, Speaker = 5;

int valor = 0;

byte zero = 0x00;
unsigned long timer;
void setup()
{
    Wire.begin();
    //A linha abaixo pode ser retirada apos setar a data e hora
    SelecionaDataeHora();
    timer = millis();
    led.shutdown(CI, false);
    led.setIntensity(CI, 8);
    for (byte i = 2; i < 5; i++)
        pinMode(i, INPUT_PULLUP);
    pinMode(Speaker, OUTPUT);
}

void loop()
{
    char tempString[10]; //Used for sprintf
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);
    int segundos = ConverteparaDecimal(Wire.read());
    int minutos = ConverteparaDecimal(Wire.read());
    int horas = ConverteparaDecimal(Wire.read() & 0b111111);

}

void SelecionaDataeHora() //Seta a data e a hora do DS1307
{
    byte segundos = 10; //Valores de 0 a 59
    byte minutos = 32;  //Valores de 0 a 59
    byte horas = 19;    //Valores de 0 a 23
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero); //Stop no CI para que o mesmo possa receber os dados

    //As linhas abaixo escrevem no CI os valores de
    //data e hora que foram colocados nas variaveis acima
    Wire.write(ConverteParaBCD(segundos)); //Pega o tempo do módulo RTC
    Wire.write(ConverteParaBCD(minutos));
    Wire.write(ConverteParaBCD(horas));
    Wire.write(zero);
    Wire.endTransmission();

    //desenha a hora na tela/Display
    led.setDigit(CI, 0, horas / 10, 0);
    led.setDigit(CI, 1, horas % 10, 0);
    led.setDigit(CI, 2, minutos / 10, 0);
    led.setDigit(CI, 3, minutos % 10, 0);
    led.setDigit(CI, 4, segundos / 10, 0);
    led.setDigit(CI, 5, segundos % 10, 0);
}

byte ConverteParaBCD(byte val)
{
    //Converte o número de decimal para BCD
    return ((val / 10 * 16) + (val % 10));
}

byte ConverteparaDecimal(byte val)
{
    //Converte de BCD para decimal
    return ((val / 16 * 10) + (val % 16));
}