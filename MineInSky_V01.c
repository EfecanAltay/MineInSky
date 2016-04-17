char px=6,py=1 ;                     //Ucaðýmýzýn Koordinatlarý
char map[2][11];                     //Gelen mayýnlar içintutulan map
char adim=0;                        //Gelen Mayýnlarýn Oluþma Zamaný için gerekli adým Sayýsý

char gameStatus = 0;  // 0:StartScreen ,1:GameScreen ,2:GameOverScreen;

//LCD pin Baðlantýlarý-----------------
sbit LCD_RS at GPIOE_ODR.B8;
sbit LCD_EN at GPIOE_ODR.B10;
sbit LCD_D4 at GPIOE_ODR.B4;
sbit LCD_D5 at GPIOE_ODR.B5;
sbit LCD_D6 at GPIOE_ODR.B6;
sbit LCD_D7 at GPIOE_ODR.B7;
//--------------------------------------

unsigned int periyot;   // Oyunla alakasý yok fakat Pwm Çalýþýldý

//Oyun için Gerekli Textler--------------
char gameOvertxt[] = "Game Over";
char Trytxt[] = "Tekrar Dene ?";
char yesTxt[] ="Evet";
char noTxt[] ="Hayir";

char WelcomeTxt[] = "MineInSky V.01";

char StartGameTxt[] = "Start";
char About[] = "About";

const char character[] = {0,4,20,30,30,20,4,0};
char txtSkor[] = "Skor";
//----------------------------------------

void Plane(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(72);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(character[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 1);
}

const char character2[] = {0,4,21,14,14,21,4,0};

void Mayin(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(80);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(character2[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 2);
}
const char character3[] = {0,4,2,31,31,2,4,0};

void Cursor(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(88);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(character3[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 3);
}

void WriteSkor(){
 unsigned char score[100];
     IntToStr(skor,score);
     Lcd_Out(1,1,txtSkor);
     Lcd_Out(1,38,score);
     Lcd_Chr(1,5,'|');
     Lcd_Chr(2,5,'|');
}

void RandomMayin(){
     int r = rand();
     if(skor <= 10 && adim > 5){
         map[r%2][9] = 1 ;
         adim = 0 ;
     }else if(skor > 10 && skor <= 20 && adim > 3){
         map[r%2][9] = 1 ;
         adim = 0 ;
     }
     else if(skor > 20 && adim > 2){
         map[r%2][9] = 1 ;
         adim = 0;
     }
     adim++ ;
}
void shiftMap(){
    char i = 0,j=0;
    for(i=0; i <2;i++){
       for(j=0;j<10;j++){
          map[i][j] = map[i][j+1] ;
          }
       }
}
void WriteMayin(){
    char i = 0,j=0;
      for(i=0; i <2;i++){
       for(j=0;j<11;j++){
          if(map[i][j] == 1){
           Mayin (i+1,j+6);
          }
         }
      }
}
//Uçaðýmýzýn Mayýna Çarpmýþmý Kontrol Ettiðimiz fonk--
void PlaneCollision(){
     if(map[0][0] == 1 || map[1][0] == 1){
                  if(map[py-1][0] == 1){
                  gameStatus = 2 ;
                  }
                  else{
                  skor++;
                  }
     }
}
//----------------------------------------------------
//Lcd,Pwm,SPI Ýçin Gerekli Kurulumlar
void kurulum(){
 GPIO_Digital_Output(&GPIOE_ODR,_GPIO_PINMASK_3);
 GPIO_Digital_Input(&GPIOA_IDR,_GPIO_PINMASK_0);
 GPIO_Digital_Output(&GPIOD_ODR,_GPIO_PINMASK_12 | _GPIO_PINMASK_13 | _GPIO_PINMASK_15);
 
    GPIO_Digital_Output(&GPIOE_ODR,_GPIO_PINMASK_9);
   GPIOE_ODR.B9 = 0 ;

   Lcd_Init();
   Lcd_Cmd(_LCD_CLEAR);

   Lcd_Cmd(_LCD_CURSOR_OFF);

   

 periyot = PWM_TIM4_Init(50000);
 
 PWM_TIM4_Set_Duty((periyot/100)*0, _PWM_NON_INVERTED, _PWM_CHANNEL3);
 PWM_TIM4_Start(_PWM_CHANNEL3, &_GPIO_MODULE_TIM4_CH3_PD14);
 
 ChipSelect = 1;
 SPI1_Init();
 ChipSelect = 0;
 SPI1_write(0x0F|0x80);
 okunan = SPI1_Read(0);
 ChipSelect = 1;
 if(okunan = 0x3B)
 {
  ChipSelect = 0;
  SPI1_write(0x20);
  SPI1_write(0x47);
  ChipSelect = 1;
 }
}
//----------------------------------------------------------------
//SPI ile veri yoluna eðim sensörunden veri cekiyoruz-------------
void chipOku(){

 ChipSelect = 0;
 SPI1_write(0x27|0x80);
 okunan = SPI1_read(0);
 ChipSelect = 1;
 if(okunan&2){
 ChipSelect = 0;
 SPI1_write(0x2b|0x80);
 yi = SPI1_read(0);
 ChipSelect = 1;
 }
}
//-----------------------------------------------------------------
//Oyunun Sýfýrlanmasý----------------------
void GameInit(){
 char i,j;
     skor = 0 ;
      for(i=0; i <2;i++){
       for(j=0;j<11;j++){
          map[i][j] = 0;
         }
      }
}
//-----------------------------------------
//Oyunun Ýnputlarýnýn Çekilmesi------------
void input(){
  chipOku();
  GPIOD_ODR.B12 = 0 ;
  if(GPIOA_IDR.B0){GPIOD_ODR.B12 = 1;}
  else{GPIOD_ODR.B12 = 0;}
  if(yi >= 0 ){GPIOD_ODR.B13 = 1; PWM_TIM4_Set_Duty((periyot/100)*90, _PWM_NON_INVERTED, _PWM_CHANNEL3); py=1;}
  else{GPIOD_ODR.B13 = 0;}
  if(yi <= 0 ){GPIOD_ODR.B15 = 1; PWM_TIM4_Set_Duty((periyot/100)*10, _PWM_NON_INVERTED, _PWM_CHANNEL3); py=2;}
  else{GPIOD_ODR.B15 = 0;}
}
//-----------------------------------
//Oyun Giriþ Ekranýn Yasýtýlmasý----------------
void GameStart(){
char secim,i ;
int a = 0;

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,16,WelcomeTxt);
    for( i = 0 ; i < 15 ;i++){
    Lcd_Cmd(_LCD_SHIFT_LEFT);
    Delay_ms(500);
    }
    Delay_ms(2000);
    
    while(btn == 0){
     Lcd_Cmd(_LCD_CLEAR);
     input();
     if(py-1){
     secim = 0 ;
     }else{
     secim = 1 ;
     }
     if(a<2 && secim== 0)a++;
     else if(a>0 && secim== 1)a--;
     
     Lcd_Out(1,1+a,WelcomeTxt);
     Lcd_Out(2,4,StartGameTxt);
     Lcd_Out(2,11,About);
     if(secim)Cursor(2,3);
     else Cursor(2,10);
     Delay_ms(300);
     }
     if(secim) {
     gameStatus=1;
     GameInit();
     }else{gameStatus=3;}
}
//---------------------------------------------
//GameAbout için Yazdýðým Fonk-----------------
void yaz( char * a ,char * b){
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,1,a);
     Lcd_Out(2,1,b);
     Delay_ms(1000);
}
//--------------------------------------------
//About Ekranýnýn Yansýtýlmasý----------------
void GameAbout(){
     yaz("MineInSky V.01","");
     Delay_ms(4000);
     yaz(""," Bilecik");
     yaz("Bilecik","Universitesi");
     yaz("Universitesi","Ogrencisi");
     yaz("Ogrencisi","Efecan Altay");
     yaz("Efecan Altay","Tarafindan");
     yaz("Tarafindan","Tasarlanan");
     yaz("Tasarlanan","Odev Maksadila");
     yaz("Odev Maksadila","Yapilmistir");
     yaz("Yapilmistir","");
     yaz("","Tum Emegi");
     yaz("Tum Emegi","Gecenlere");
     yaz("Gecenlere" , "O da Sadece Ben");
     yaz("O da Sadece Ben" , "Oluyorum :D");
     yaz("Oluyorum :D","ve Hakan Hocama");
     yaz("ve Hakan Hocama,","Tum Arkadaslara");
     yaz("Tum Arkadaslara","Saygilar....");
     yaz("Saygilar....","");
     yaz("","");
     Delay_ms(500);
     yaz("Wrise Game","Efecan Altay");
     yaz("","");
     yaz("Wrise Game","Efecan Altay");
     yaz("","");
     yaz("Wrise Game","Efecan Altay");
     Delay_ms(5000);
     gameStatus = 0;
}
//-------------------------------------------------
//Oyun Ekranýnýn Yansýtýlmasý----------------------
void GameUpdate(char px,char py){
      input();
      Lcd_Cmd(_LCD_CLEAR);
      WriteSkor();
      WriteMayin();
      Plane(py,px);
      shiftMap();
      RandomMayin();
      PlaneCollision();
      Delay_ms(300);
      //Mayin(2,1);
}
//------------------------------------------
//GameOver Ekranýnýn Yansýtýlmasý-----------
void GameOver(){
char i;
char secim=0;
unsigned char score[100];
     IntToStr(skor,score);
     
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,16,gameOvertxt);
     for(i = 0 ; i < 11 ;i++){
     Lcd_Cmd(_LCD_SHIFT_LEFT);
     Delay_ms(500);
     }
     Delay_ms(3000);
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,8,txtSkor);
     Lcd_Out(2,6,score);
     Delay_ms(4000);
     
     Lcd_Cmd(_LCD_CLEAR);
     while(btn == 0){
     input();
     if(py-1){
     secim = 0 ;
     }else{
     secim = 1 ;
     }
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Out(1,3,Trytxt);
     Lcd_Out(2,4,yesTxt);
     Lcd_Out(2,11,noTxt);
     if(secim)Cursor(2,3);
     else Cursor(2,10);
     Delay_ms(300);
     }
     if(secim) {
     gameStatus=1;
     GameInit();
     }else{
     gameStatus=0;
     }
}
//-----------------------------------
//Ana Fonk--------------------------
void main() {
  kurulum();
  do{
  //GameStatus seceneklerine göre Oyun Ekraný Deðiþiyor
  switch(gameStatus){
  case 0:
  GameStart();
       break;
  case 1:
  GameUpdate(px,py);
       break;
  case 2:
  GameOver();
       break;
  case 3:
  GameAbout();
       break;
  }

  Delay_ms(100);
  }while(1);
}
//-----------------------------------