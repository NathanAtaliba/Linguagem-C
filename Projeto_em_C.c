float new_valor = 0;
////////////////////////////////////
//INTERRUPCAO
ISR(INT1_vect){
// aumenta 10% cada vez que o PD2 for apertado

new_valor += (25.5); // soma 10% do valor de 255

}
//////////////////////////////////////////////////////////////////////////////////////////////////

char RX_buffer[32]; // Variáveis para entrada e saída
char RX_index = 0; // Variáveis para entrada e saída
char old_rx_hs[32];// Buffer para estado anterior do RX

// A inicialização do UART consiste em definir a taxa de transmissão,
// definir o formato de quadro, e ativar o Transmissor ou o receptor.

void UART_init(int baud) // Função de configuração do UART
{
    int MYUBRR = 16000000 / 16 / baud - 1; // Calcula a taxa de transmissão
    UBRR0H = (unsigned char)(MYUBRR >> 8); // Definindo a taxa de transmissão
    UBRR0L = (unsigned char)(MYUBRR);
    UCSR0C = (0 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01); // Definindo o formato de quadro, 8 bits e 1 stop bit
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Ativa o Transmissor, receptor e define a interrupção
}

void UART_send(char *TX_buffer) // Função para envio de dados via UART
{
    while (*TX_buffer != 0) // Enquanto tiver caracteres para enviar
    {
        UDR0 = *TX_buffer;  // Prepara o buffer para o envio
        while (!(UCSR0A & (1 << UDRE0))){};// Espera o envio ser completado
        TX_buffer++; // Avança o ponteiro do buffer
    }
}
void limpa_RX_buffer(void) // Limpa o buffer de entrada e saída
{
    unsigned char dummy;
    while (UCSR0A & (1 << RXC0))// Enquanto houver dados no buffer
    {
        dummy = UDR0; // Lê o dado
    }
    RX_index = 0; // Reseta o índice
    for (int i = 0; i < 32; i++) // Limpa todos os dados do buffer
    {
        old_rx_hs[i] = RX_buffer[i];
        RX_buffer[i] = 0;
    }
}

ISR(USART_RX_vect)
{
    // Salva o dado recebido
    RX_buffer[RX_index] = UDR0;
    RX_buffer[RX_index +1] = 0;
    // Adiciona mais 1 na contagem
    RX_index++;
}

/////////////////////////////////////////////////////////////////////////////////
void ADC_init () {

ADMUX |= (1 << REFS0); // Configura Vref para VCC = 5V

ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int ADC_read (unsigned char pino) {// Função para leitura do pino do ADC
ADMUX |= (pino & 0b00000111); // Seleciona o pino de entrada
int valor = 0;// Pega a média das amostras
int valor_aux;
for (unsigned char i = 0; i < 8; i++) { // Realiza 8 leituras
ADCSRA |= (1 << ADSC); // Inicia a conversão
while (!ADCSRA & (1 << ADIF));// Aguarda o fim da conversão
valor_aux = ADCL;// Pega o valor da conversão
valor_aux += ADCH << 8;
valor += valor_aux; // Soma o valor da amostra
}
return valor >> 3;// Retorna a média de 8 leituras
}

int main (){
//////////////////
DDRD |= 0b01100000; // PD6 saída PWM (LED vermelho)// PD5 saida GPIO(LED Verde)
PORTD |= 0b00001000; // PD2 em pull-up
EIMSK |= (0b00000010); // Habilita a interrupção no PD2
EICRA |= (0b00001101); // Transição de subida
TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00); // Modo não-invertido e modo Fast-PWM
TCCR0B |= (1<<CS02)|(1<<CS00);
sei();
bool SystemStatus=0 ;
UART_init(9600);
///////////////////////////
ADC_init (); // Inicializa o ADC
//Serial.begin (9600); // Inicializa a porta serial
float valorTemp=0;
//////////////////////////// 
while(1){
  int i = RX_index;
 char vetor[32];
if(RX_buffer[i-3]== 'o' && RX_buffer[i-2]== 'f' && RX_buffer[i-1]== 'f'){ 
UART_send(RX_buffer);
PORTD|=(0<<PD5);
SystemStatus=0;
limpa_RX_buffer();
}else
if((RX_buffer[i-2]== 'o' && RX_buffer[i-1]== 'n')){
UART_send(RX_buffer);
PORTD|=(1<<PD5);
SystemStatus=1;
limpa_RX_buffer();
}
if(SystemStatus==1){
OCR0A = new_valor;
int valor = ADC_read (PC0);
valorTemp =valor*(50.0/1023.0)+20;
UART_send("Temperatura: ");// Imprime o valor
itoa(valorTemp,vetor,10);
UART_send(vetor);
UART_send(" °C");
_delay_ms(1000);
}
else{
 ADCSRA |= ( 0<< ADEN) ; // DESLIGA CONVERSOR AD
 EIMSK |= (0b00000000); //DESLIGA INTERRUPÇAO
 PORTD |= (0<<PD6)|(0<<PD5); //DESLIGA LEDS DE PWM E INDICAÇÃO
}
  }
  
}