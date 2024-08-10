#include "cmain.h"

extern CSave cSave;//класс записи данных

CMain cMain;

//-Конструктор класса--------------------------------------------------------
CMain::CMain()
{
 //запускаем поток управления устройством
 cReceiveThread.StartThread();
 SaveEnabled=false; 
 for(long n=0;n<TREND_SIZE;n++) TempTrendArray[n]=0;
 TrendSize=0;
}
//-Деструктор класса---------------------------------------------------------
CMain::~CMain()
{  
 //останавливаем поток управления устройством
 cReceiveThread.StopThread();  
}
//-Замещённые функции предка-----------------------------------------------
//-Новые функции класса------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//открытие окна
//----------------------------------------------------------------------------------------------------
bool CMain::OnOpening(void)
{
 return(true);
}
//----------------------------------------------------------------------------------------------------
//закрытие окна
//----------------------------------------------------------------------------------------------------
bool CMain::OnClosing(void)
{
 return(true);	
}
//----------------------------------------------------------------------------------------------------
//перерисовать картинку
//----------------------------------------------------------------------------------------------------
void CMain::Redraw(void)
{
}
//----------------------------------------------------------------------------------------------------
//обработка таймера
//----------------------------------------------------------------------------------------------------
void CMain::OnTimer(void)
{
 float temp;	
 bool enabled=cReceiveThread.GetCurrentTemp(temp);
 //выводим на экран и сохраняем в файл
 char string[255];
 sprintf(string,"T=?");
 if (enabled==true)
 {
  static long counter=0;
  if (counter==0)
  { 	
   //заполняем тренд
   if (TrendSize<TREND_SIZE) TrendSize++;
   //сдвигаем
   for(long n=0;n<TREND_SIZE-1;n++) TempTrendArray[n]=TempTrendArray[n+1];
   TempTrendArray[TREND_SIZE-1]=temp; 

   //обновляем тренд
   float max=temp;
   float min=temp;
   long offset=TREND_SIZE-TrendSize;
   for(long n=0;n<TrendSize;n++)
   {
    float v=TempTrendArray[n+offset];
    if (v>max) max=v;
    if (v<min) min=v;
   }
   //выводим максимум и минимум температуры
   sprintf(string,"%.1f C",max);
   SetWidgetText(ABW_PtLabel_TMax,string);
   sprintf(string,"%.1f C",min);
   SetWidgetText(ABW_PtLabel_TMin,string);
   //перерисовываем тренд
   short data[TREND_SIZE];
   double dt=max-min;
   if (fabs(dt)<ZERO_EPS) dt=1;
   double scale=(double)(TREND_MAX_VALUE)/dt;
   for(long n=0;n<TREND_SIZE;n++)
   {
    float v=TempTrendArray[n];
    if (n<offset) v=TempTrendArray[offset];//данных до этой точки нет, поэтому возьмём первую добавленную точку
    v-=min;
    v*=scale;
    if (v>TREND_MAX_VALUE) v=TREND_MAX_VALUE;
    if (v<0) v=0;
    data[n]=static_cast<short>(v);	
   }
   //выводим тренд
   PtTrendChangeTrendData(ABW_PtTrend_Temp,0,data,0,TREND_SIZE);
  }
  counter++;
  counter%=16;
  //формируем строчку с температурой
  sprintf(string,"T=%.01fC",temp);
  if (SaveEnabled==true) cSave.SaveTemp(TickCounter,temp);
 }
 SetWidgetText(ABW_PtLabel_TValue,string);
 TickCounter+=4;//таймер вызывается с частотой 8 Гц, а нужно сохранять как 32 Гц
}
//----------------------------------------------------------------------------------------------------
//нажата кнопка сохранения данных
//----------------------------------------------------------------------------------------------------
void CMain::OnActivate_Button_Save(void)
{
 if (SaveEnabled==false)
 {
  PtSetResource(ABW_PtButton_Save,Pt_ARG_FILL_COLOR,Pg_GREEN,0);
  SetWidgetText(ABW_PtButton_Save,"Отключить запись");
  SaveEnabled=true;
  TickCounter=0;
  cSave.Start();  
 }
 else
 {
  PtSetResource(ABW_PtButton_Save,Pt_ARG_FILL_COLOR,Pg_GRAY,0);
  SetWidgetText(ABW_PtButton_Save,"Включить запись");
  SaveEnabled=false;
  TickCounter=0;
  cSave.Stop();
 }
}
//----------------------------------------------------------------------------------------------------
//нажата кнопка очистки графика
//----------------------------------------------------------------------------------------------------
void CMain::OnActivate_Button_ClearTrend(void)
{
 TrendSize=0;	
}
//-Функции обработки сообщений класса----------------------------------------

//-Новые функции класса------------------------------------------------------

//-Прочее--------------------------------------------------------------------

