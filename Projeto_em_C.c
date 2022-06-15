float new_valor=0;
ISR(INT1_vect){
// aumenta 10% cada vez que o PD2 for apertado

new_valor += (25.5); // soma 10% do valor de 255

}
void ADC_init () {



// Configura Vref para VCC = 5V
ADMUX |= (1 << REFS0);



/* ADC ativado e preescaler de 128
        16MHz / 128 = 125kHz
        ADEN = ADC Enable, ativa o ADC
        ADPSx = ADC Prescaler Select Bits
        1 1 1 = clock / 128 */



ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}



// Função para leitura do pino do ADC
int ADC_read (unsigned char pino) {



// Seleciona o pino de entrada
ADMUX |= (pino & 0b00000111);



// Pega a média das amostras
int valor = 0;
int valor_aux;



// Realiza 8 leituras
for (unsigned char i = 0; i < 8; i++) {



// Inicia a conversão
ADCSRA |= (1 << ADSC);



// Aguarda o fim da conversão
while (!ADCSRA & (1 << ADIF));



// Pega o valor da conversão
valor_aux = ADCL;
valor_aux += ADCH << 8;



// Soma o valor da amostra
valor += valor_aux;
}



// Retorna a média de 8 leituras
return valor >> 3;
}
int main(){

//////////////////
DDRD |= 0b01000000; // PD6 saída PWM (LED vermelho)
PORTD |= 0b00001000; // PD2 em pull-up
EIMSK |= (0b00000010); // Habilita a interrupção no PD2
EICRA |= (0b00001101); // Transição de subida
TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00); // Modo não-invertido e modo Fast-PWM
TCCR0B |= (1<<CS02)|(1<<CS00);
sei();



///////////////////////////
ADC_init (); // Inicializa o ADC
Serial.begin (9600); // Inicializa a porta serial
////////////////////////////
float valorTemp=0;
while(1){
OCR0A = new_valor;
int valor = ADC_read (PC0);
valorTemp =valor*(50.0/1023.0)+20;
Serial.print ("Temperatura: ");// Imprime o valor
Serial.println (valorTemp);
}
}