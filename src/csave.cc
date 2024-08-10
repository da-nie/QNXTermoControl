#include "csave.h"

CSave cSave;//класс записи данных

void *Save_thread(void *data);//поток записи

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//конструктор и деструктор класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CSave::CSave()
{
 vector_CIODataPtr.clear();
 Enabled=false;	
 FirstTick=0;
 FirstTickEnabled=false; 
 pthread_mutex_init(&mutex_ID,NULL);//создаём мьютекс для доступа	 
};
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CSave::~CSave()
{
 Stop();
 pthread_mutex_destroy(&mutex_ID);//удаляем мьютекс 	 
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//закрытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//создание имени файла из имени каталога, имени устройства и номера устройства
//----------------------------------------------------------------------------------------------------
char* CSave::MakeFileName(char *patch,char *filename,char *device_name,long device_index)
{
 if (device_index>0) sprintf(MakeFileNameString,"%s/%s%ld%s",patch,device_name,device_index,filename);
 if (device_index==0) sprintf(MakeFileNameString,"%s/%s%s",patch,device_name,filename);
 if (device_index<0) sprintf(MakeFileNameString,"%s/%s",patch,filename);
 return(MakeFileNameString);
}
//----------------------------------------------------------------------------------------------------
//запись файлов
//----------------------------------------------------------------------------------------------------
void CSave::Save(CIOData *cIOData_Ptr,unsigned long t,long double value)
{
 if (cIOData_Ptr==NULL) return;
 if (Enabled==false) return;
 if (FirstTickEnabled==false) 
 {
  FirstTick=t;
  FirstTickEnabled=true;
 }
 if (t<FirstTick) return;//время меньше времени начала записи
 t-=FirstTick;
 cIOData_Ptr->AddData(value,t);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//открытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//возвратить, требуется ли выход из потока
//----------------------------------------------------------------------------------------------------
bool CSave::GetExitState(void)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
 bool exitthread=ExitThread;
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
 return(exitthread);	
}
//----------------------------------------------------------------------------------------------------
//начать запись с заданным именем каталога
//----------------------------------------------------------------------------------------------------
bool CSave::Start(char *path)
{
 Stop();//останавливаем запись, если она была
 //создаём файл настроек
 char filename[1024];
 sprintf(filename,"%s/settings.ini",path);
 FILE *file=fopen(filename,"wb");
 if (file!=NULL)
 {
  fprintf(file,"[settings]\r\n");
  fprintf(file,"FREQUENCY = 32\r\n");
  fclose(file);
 }
 unsigned long tickofsave=0;
 unsigned long div=100;
 unsigned long offset=100;
 //инициализируем все классы записи
 vector_CIODataPtr.clear(); 
 
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
 vector_CIODataPtr.push_back(&cIOData_Temp);
 cIOData_Temp.Init(MakeFileName(path,".Температура,C","Temp",1),TypeFLOAT,0.1,1,tickofsave%div+offset);tickofsave++;
 
 Enabled=true;
 FirstTick=0;
 FirstTickEnabled=false; 
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
 
 //запускаем поток записи
 ExitThread=false;//выходить из потока не нужно 
 //запускаем поток
 pthread_attr_t pt_attr;
 pthread_attr_init(&pt_attr);
 pthread_attr_setdetachstate(&pt_attr,PTHREAD_CREATE_JOINABLE);
 pthread_attr_setinheritsched(&pt_attr,PTHREAD_EXPLICIT_SCHED);
 pthread_attr_setschedpolicy(&pt_attr,SCHED_RR);
 pt_attr.param.sched_priority=SAVE_THREAD_PRIORITY;
 pthread_create(&pthread_ID,&pt_attr,Save_thread,this);//создаём поток записи
 return(true);//запись включена
}
//----------------------------------------------------------------------------------------------------
//начать запись
//----------------------------------------------------------------------------------------------------
bool CSave::Start(void)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс	
 if (Enabled==true)//запись уже включена
 {
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
  return(true);
 }
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс

 //создаём директории	
 mkdir("/Research",S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH |S_IXOTH);
 mkdir("/Research/thermo_data",S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH |S_IXOTH); 
 char path[2048];
 char s_year[256];
 char s_year2[256];
 time_t time_main=time(NULL);
 tm *tm_main=localtime(&time_main);
 sprintf(s_year,"%i",tm_main->tm_year+1900);
 sprintf(s_year2,"%s",s_year+strlen(s_year)-2);
 sprintf(path,"/Research/thermo_data/%02i.%04i",tm_main->tm_mon+1,tm_main->tm_year+1900);
 
 mkdir(path,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH |S_IXOTH);
 long index=0;
 while(1)
 { 
  sprintf(path,"/Research/thermo_data/%02i.%04i/%02i_%02i_%s_%03ld",tm_main->tm_mon+1,tm_main->tm_year+1900,tm_main->tm_mday,tm_main->tm_mon+1,s_year2,index);
  if (mkdir(path,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH |S_IXOTH)==0) break;//создали директорию
  index++;
  if (index==1000) break;
 }
 if (index==1000) return(false);//запись не включена - достигнут предел вложенности
 return(Start(path));
}
//----------------------------------------------------------------------------------------------------
//завершить запись
//----------------------------------------------------------------------------------------------------
bool CSave::Stop(void)
{
 //останавливаем поток записи	
 if (pthread_ID!=-1)
 {
  pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
  ExitThread=true;//требуется выход из потока
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
  pthread_join(pthread_ID,NULL);//;ждём завершения потока
 }
 pthread_ID=-1;	
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
 if (Enabled==false) 
 {
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
  return(true);
 }
 long size=vector_CIODataPtr.size();
 for(long n=0;n<size;n++) vector_CIODataPtr[n]->Release();
 vector_CIODataPtr.clear();
 Enabled=false;
 FirstTick=0;
 FirstTickEnabled=false; 
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
 return(true);//запись отключена	
}
//----------------------------------------------------------------------------------------------------
//сохранить данные кэша
//----------------------------------------------------------------------------------------------------
bool CSave::SaveCache(void)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс 
 if (Enabled==false) 
 {
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
  return(true);
 }
 long size=vector_CIODataPtr.size();
 for(long n=0;n<size;n++) vector_CIODataPtr[n]->SaveCacheOn();
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
 return(true);	
}
//----------------------------------------------------------------------------------------------------
//сохранить температуру
//----------------------------------------------------------------------------------------------------
bool CSave::SaveTemp(unsigned long tick,double temp)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс 
 if (Enabled==false)
 {
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
  return(true);
 }
 //сохраняем данные
 Save(&cIOData_Temp,tick,temp);
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс 
 return(true);
}
//----------------------------------------------------------------------------------------------------
//произвести сохранение данных, если оно требуется (ВЫЗЫВАЕТСЯ ТОЛЬКО ПОТОКОМ ЗАПИСИ!) 
//----------------------------------------------------------------------------------------------------
bool CSave::SaveProcessing(void)
{
 //записываем без блокировки на мьютексе
 long size=vector_CIODataPtr.size();
 for(long n=0;n<size;n++) vector_CIODataPtr[n]->Save();
 return(true);	
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Потоки
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//----------------------------------------------------------------------------------------------------
//поток записи
//----------------------------------------------------------------------------------------------------
void *Save_thread(void *data)
{
 //блокируем любые сигналы для потока	
 sigblock(255);	
 ThreadCtl(_NTO_TCTL_IO,NULL);
 CSave	*cSave_Ptr=reinterpret_cast<CSave*>(data);
 if (cSave_Ptr==NULL) return(NULL);
 //основной цикл потока
 unsigned long counter=SAVE_CACHE_COUNTER;
 while(1)
 {
  if (cSave_Ptr->GetExitState()==true) break;
  delay(10);
  cSave_Ptr->SaveProcessing();
  counter--;
  if (counter==0)
  {
   cSave_Ptr->SaveCache();
   counter=SAVE_CACHE_COUNTER;
  }
 }  
 return(NULL);
}
