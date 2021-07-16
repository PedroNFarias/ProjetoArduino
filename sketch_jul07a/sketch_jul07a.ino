//Programa : Relogio com Arduino, DS1307 e Display 7 Seg

#include "Wire.h"
#include "LedControl.h"
#define DS1307_ADDRESS 0x68
#define RELOGIO 0
#define AJUSTE 1
#define BLINK 100

LedControl led(A3, A1, A2, 1);

const byte DisplayCi = 0, ButtonSet = 2, ButtonUp = 3, ButtonDown = 4, Speaker = 5;
int modo = RELOGIO; //variável de modo
//modo 0 = relógio
//modo 1 = setup manual de hora
int segundos, minutos, horas; //variáveis do tempo
int blk; //variável para fazer o pisca pisca
byte zero = 0x00; //zero em byte
unsigned long timer; //excluir depois

void setup()
{
    Wire.begin();
    //Liga o display
    led.shutdown(DisplayCi, false);
    //Ajusta o brilho
    led.setIntensity(DisplayCi, 8);
    //Inicia os pinos digitais
    for (byte i = 2; i < 5; i++)
        pinMode(i, INPUT_PULLUP);
    //Inicia o pino do speaker
    pinMode(Speaker, OUTPUT);
    //A linha abaixo pode ser retirada apos setar a data e hora
    SelecionaDataeHora();

    timer = millis(); //excluir depois
}

void loop()
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);

    //Lê as variáveis do RTC
    segundos = ConverteparaDecimal(Wire.read());
    minutos = ConverteparaDecimal(Wire.read());
    horas = ConverteparaDecimal(Wire.read() & 0b111111);

    //Coloca as variáveis num painel de 7 segmentos
    desenhaLED();
    //Função de ajuste de hora
    ajustarHora();
}

void SelecionaDataeHora() //Seta a data e a hora do DS1307
{
    segundos = 10; //Valores de 0 a 59
    minutos = 32;  //Valores de 0 a 59
    horas = 19;    //Valores de 0 a 23
    enviaDados();
    desenhaLED();
}

byte ajustarHora()
{
    int digito;

    if (!digitalRead(ButtonSet) && modo == RELOGIO){
        modo = AJUSTE;
        digito = 5;
    }

    while(modo != RELOGIO){
      
        
        //////////////////////////////////////////////////
        //Ifs de controle de ajuste
        if (digito == 5){
            if (!digitalRead(ButtonUp)){
                segundos += 1;
            }else if(!digitalRead(ButtonDown)){
                segundos -= 1;
            }
        }else if(digito == 4){
            if (!digitalRead(ButtonUp)){
                segundos += 10;
            }else if(!digitalRead(ButtonDown)){
                segundos -= 10;
            }
        }else if(digito == 3){
            if (!digitalRead(ButtonUp)){
                minutos += 1;
            }else if(!digitalRead(ButtonDown)){
                minutos -= 1;
            }
        }else if(digito == 2){
            if (!digitalRead(ButtonUp)){
                minutos += 10;
            }else if(!digitalRead(ButtonDown)){
                minutos -= 10;
            }
        }else if(digito == 1){
            if (!digitalRead(ButtonUp)){
                horas += 1;
            }else if(!digitalRead(ButtonDown)){
                horas -= 1;
            }
        }else if(digito == 0){
            if (!digitalRead(ButtonUp)){
                horas += 10;
            }else if(!digitalRead(ButtonDown)){
                horas -= 10;
            }
        }
        ///////////////////////////////////////////////////
        
        //Troca o digito com o botão SET
        if(!digitalRead(ButtonSet)){
            digito--;
        }
        
        //volta pro modo relógio
        if(digito < 0){
            modo = RELOGIO;
        }

        //ajeita as variáveis
        if(horas >= 24){
            horas -= 24;
        }else if(horas < 0){
            horas = 24;
        }

        if(minutos >= 60){
            minutos -= 60;
        }else if(minutos < 0){
            minutos = 59;
        }

        if(segundos >= 60){
            segundos -= 60;
        }else if(segundos < 0){
            segundos = 59;
        }

        //colocas nos display 
        desenhaLED();
        pisca(digito);
        //envia dados para o CI
        enviaDados();
        //delay
        delay(500);

    }

}

void desenhaLED(){
    led.setDigit(DisplayCi, 0, horas / 10, 0);
    led.setDigit(DisplayCi, 1, horas % 10, 0);
    led.setDigit(DisplayCi, 2, minutos / 10, 0);
    led.setDigit(DisplayCi, 3, minutos % 10, 0);
    led.setDigit(DisplayCi, 4, segundos / 10, 0);
    led.setDigit(DisplayCi, 5, segundos % 10, 0);
}

void enviaDados(){
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero); //Stop no CI para que o mesmo possa receber os dados
    //As linhas abaixo escrevem no CI os valores de
    //data e hora que foram colocados nas variaveis acima
    Wire.write(ConverteParaBCD(segundos));
    Wire.write(ConverteParaBCD(minutos));
    Wire.write(ConverteParaBCD(horas));
    Wire.write(zero);
    Wire.endTransmission();
}

void pisca(int digito){
    if (blk > BLINK){ 
        blk = 0;
    }
    if (blk > BLINK/2){
      led.setChar(DisplayCi, digito, ' ', 0);
    }
    blk++;
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
