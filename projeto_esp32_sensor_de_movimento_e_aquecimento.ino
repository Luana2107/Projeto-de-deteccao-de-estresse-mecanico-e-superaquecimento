#include <Wire.h> //ativa a comunicação I2C, que é um comunicação de dois fios SDA (os dados vão e veem em mão dupla e SCL é o clock que define a batida dos dados nesse vai e vem)
#include <analogWrite.h>


// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;


// Select SDA and SCL pins for I2C communication
const uint8_t Pin_scl = 22; //selecionamos o pino 22, contudo, vc pode alterar
const uint8_t Pin_sda = 21; //selecionamos o pino 21
const uint8_t Pin_buzzer = 23;  //selecionamos o pino 23, mude para o 25 caso o seu buzzer seja passivo (de acordo com as explicações do professor)




// sensitivity scale factor respective to full scale setting provided in datasheet
const uint16_t AccelScaleFactor = 16384;  //define um fator de escala para melhorar a sensibilidade do sensor. Esses números são fixos
const uint16_t GyroScaleFactor = 131;     //define um fator de escala para melhorar a sensibilidade do sensor. Esses  números são fixos


// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;  //o fabricante define esses endereços, não precisamos alterar
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;  //o fabricante define esses endereços, não precisamos alterar


int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ, VOLTA=0;
double Ax_ref, Ay_ref, Az_ref, T_ref, Gx_ref, Gy_ref, Gz_ref;
double Ax_atual, Ay_atual, Az_atual, T_atual, Gx_atual, Gy_atual, Gz_atual;
double LimiteMec = 0.8, LimiteTemp = 5;




void setup() {
  Serial.begin(115200);
  Wire.begin(Pin_sda, Pin_scl);
  MPU6050_Init(); //aqui estamos ativando a plaquinha MPU6050
  pinMode(Pin_buzzer, OUTPUT);
  delay(5000); //linha de teste
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  T_ref = (double)Temperature/340+36.53; //ajustando o valor da temperatura em ºC
} //aqui encerra o void setup


void loop() {
 
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);


  if(VOLTA==0) //vai entrar aqui logo na primeira volta do loop
  {
    //divide each with their sensitivity scale factor
    Ax_ref = (double)AccelX/AccelScaleFactor;
    Ay_ref = (double)AccelY/AccelScaleFactor;
    Az_ref = (double)AccelZ/AccelScaleFactor;
    Gx_ref = (double)GyroX/GyroScaleFactor;
    Gy_ref = (double)GyroY/GyroScaleFactor;
    Gz_ref = (double)GyroZ/GyroScaleFactor;
    Serial.print("Ax: "); Serial.print(Ax_ref);
    Serial.print(" Ay: "); Serial.print(Ay_ref);
    Serial.print(" Az: "); Serial.print(Az_ref);
    Serial.print(" Gx: "); Serial.print(Gx_ref);
    Serial.print(" Gy: "); Serial.print(Gy_ref);
    Serial.print(" Gz: "); Serial.println(Gz_ref);
    VOLTA=1;
  }
  else
  {
    Ax_atual = (double)AccelX/AccelScaleFactor;
    Ay_atual = (double)AccelY/AccelScaleFactor;
    Az_atual = (double)AccelZ/AccelScaleFactor;
    T_atual = (double)Temperature/340+36.53; //fórmula da temperatura
    Gx_atual = (double)GyroX/GyroScaleFactor;
    Gy_atual = (double)GyroY/GyroScaleFactor;
    Gz_atual = (double)GyroZ/GyroScaleFactor;
    Serial.print("Ax: "); Serial.print(Ax_atual);
    Serial.print(" Ay: "); Serial.print(Ay_atual);
    Serial.print(" Az: "); Serial.print(Az_atual);
    Serial.print(" T: "); Serial.print(T_atual);
    Serial.print(" Gx: "); Serial.print(Gx_atual);
    Serial.print(" Gy: "); Serial.print(Gy_atual);
    Serial.print(" Gz: "); Serial.println(Gz_atual);
    VOLTA=0;
  }
  if((Ax_atual - Ax_ref) >= LimiteMec)
  {
    Serial.print("\n\nESTRESSE MEC NICO DETECTADO");
    BUZZER1();
    Serial.print("\nAtual: ");Serial.print(Ax_atual);
    Serial.print("\nRef: ");Serial.print(Ax_ref);
    Serial.print("\n");
  }


  if(abs(T_atual - T_ref) >= LimiteTemp)
  {
    Serial.print("\n\nATENÇÃO: SUPER-AQUECIMENTO\n\n");
    BUZZER2();
    Serial.print("\nAtual: ");Serial.print(T_atual);
    Serial.print("\nRef: ");Serial.print(T_ref);
    Serial.print("\n");
  }


  delay(50);
}


void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}


// lê todos os 14 registros do MPU6050
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}


void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}


void BUZZER1(){
  for (int i=0; i < 3;i++){
    //digitalWrite(Pin_buzzer, HIGH);
    //delay(100);
    //digitalWrite(Pin_buzzer, LOW);
    //delay(100);
 
    int freq = 700; //escolha um valor entre 0 e 1023 para mudar a freq do apito
    int pwm = map(freq, 0, 1024, 0, 255); //define valores pwm (0 a 255)
    analogWrite(Pin_buzzer, pwm);
    delay(100);
  }
  analogWrite(Pin_buzzer, 0);  
}


void BUZZER2(){
  for (int i=0; i < 9;i++){
    //digitalWrite(Pin_buzzer, HIGH);
    //delay(100);
    //digitalWrite(Pin_buzzer, LOW);
    //delay(100);


    int freq = 700; //escolha um valor entre 0 e 1023 para mudar a freq do apito
    int pwm = map(freq, 0, 1024, 0, 255); //define valores pwm (0 a 255)
    analogWrite(Pin_buzzer, pwm);
    delay(100);
  }
  analogWrite(Pin_buzzer, 0);  
}
