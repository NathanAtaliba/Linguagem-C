int main(){
Serial.begin(9600);
ADMUX=0b00000011;
ADCSRA |=(1<<ADPS1)|(ADPS0);  //PRESCALER =8
ADCSRA |=(1<<ADEN); //LIGA O CONVERSOR AD
ADCSRA |=(1<<ADSC);  //INICIA A CONVERSAO

while(1){
   
while(ADCSRA&(1<<ADSC)); //AGUARDA O FIM DA CONVERSAO
Serial.println(ADC); //MOSTRA O VALOR CONVERTIDO NA SERIAL
adcsra |= (1<<adsc); //INICIA UMA NOVA CONVERSAO

}}