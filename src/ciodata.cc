#include "ciodata.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//конструктор и деструктор класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CIOData::CIOData(void)
{
 pthread_mutex_init(&mutex_ID,NULL);//создаём мьютекс для доступа	

 sData.Buffer=NULL;
 sData.Size=0;
 sData.Amount=0;
 sData.FileName=NULL; 
 sData.SaveCache=false;	
 
 SizeOfElement=0;
 Delta=1;
 Type=0;
 Scale=1;
 FirstData=true;
 LastTime=0;
 LastAddTime=-1;
 SaveAll=false;
 OldValue=0;
}
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CIOData::~CIOData(void)
{
 Release();	
 pthread_mutex_destroy(&mutex_ID);//удаляем мьютекс 	 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//закрытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//захватить мьютекс доступа к sData//----------------------------------------------------------------------------------------------------
void CIOData::Lock_sData(void)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
}
//----------------------------------------------------------------------------------------------------
//освободить мьютекс доступа к sData
//----------------------------------------------------------------------------------------------------
void CIOData::UnLock_sData(void)
{
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//открытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//инициализация
//----------------------------------------------------------------------------------------------------

bool CIOData::Init(char *filename,char type,double delta,double scale,unsigned long tickofsave)
{
 Release(); 
 if (filename==NULL) return(false);
 if (strlen(filename)==0) return(false);
 
 Lock_sData();
 sData.SaveCache=false;
 sData.FileName=new char[(strlen(filename)+100)*2];
 strcpy(sData.FileName,"");
 char s_scale[255];
 strcpy(s_scale,"");
 if (scale!=1) sprintf(s_scale,"_%g",scale);
 SizeOfElement=0;
 if (type==TypeCHAR) 
 {
  sprintf(sData.FileName,"%s%s.c",filename,s_scale);
  SizeOfElement=sizeof(signed char);
 }
 if (type==TypeBYTE)
 {
  sprintf(sData.FileName,"%s%s.uc",filename,s_scale);
  SizeOfElement=sizeof(unsigned char);  
 }
 if (type==TypeSHORT) 
 {
  sprintf(sData.FileName,"%s%s.s",filename,s_scale);
  SizeOfElement=sizeof(signed short);  
 }
 if (type==TypeWORD) 
 {
  sprintf(sData.FileName,"%s%s.us",filename,s_scale);
  SizeOfElement=sizeof(unsigned short);    
 }
 if (type==TypeLONG) 
 {
  sprintf(sData.FileName,"%s%s.l",filename,s_scale);
  SizeOfElement=sizeof(signed long);    
 }
 if (type==TypeDWORD)
 {
  sprintf(sData.FileName,"%s%s.ul",filename,s_scale);
  SizeOfElement=sizeof(unsigned long);      
 }
 if (type==TypeFLOAT)
 {
  sprintf(sData.FileName,"%s%s.f",filename,s_scale);
  SizeOfElement=sizeof(float);
 }
 if (type==TypeDOUBLE)
 {
  sprintf(sData.FileName,"%s%s.d",filename,s_scale);
  SizeOfElement=sizeof(double);
 }
 if (SizeOfElement==0)
 {
  delete(sData.FileName);
  sData.FileName=NULL;
  UnLock_sData();
  return(false);	
 }
 sData.Size=(SizeOfElement+sizeof(long))*1000;
 sData.Buffer=new unsigned char [sData.Size];
 sData.Amount=0;
 UnLock_sData();

 Delta=delta;
 Type=type;
 Scale=scale;
 FirstData=true;
 LastTime=0;
 SaveAll=false;
 return(true);
}

bool CIOData::AddData(double value,long time)
{	
 if (FirstData==true) OldValue=value;
 LastTime=time;
 if (SaveAll==true)
 {
  if (fabs(value-OldValue)<=CIO_EPS && FirstData==false) return(true);
 }	
 else
 {
  if (fabs(value-OldValue)<=Delta && FirstData==false) return(true);
 }
 if (LastAddTime==time) return(true);//такую точку мы уже добавляли
 LastAddTime=time;
 OldValue=value;
 FirstData=false;
 if (Type==TypeCHAR)
 {
  struct SMask
  {
   unsigned long Time;
   signed char Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<signed char>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeBYTE)
 {
  struct SMask
  {
   unsigned long Time;  	
   unsigned char Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<unsigned char>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeSHORT)
 {
  struct SMask
  {
   unsigned long Time;  	
   signed short Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<signed short>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeWORD)
 {
  struct SMask
  {
   unsigned long Time;  	
   unsigned short Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<unsigned short>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeLONG)
 {
  struct SMask
  {
   unsigned long Time;  	
   signed long Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<signed long>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeDWORD)
 {
  struct SMask
  {
   unsigned long Time;  	
   unsigned long Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<unsigned long>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeFLOAT)
 {
  struct SMask
  {
   unsigned long Time;  	
   float Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<float>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 }
 if (Type==TypeDOUBLE)
 {
  struct SMask
  {
   unsigned long Time;  	
   double Value;  	  
  } sMask;
  sMask.Time=time;
  sMask.Value=static_cast<double>(value*Scale);
  AddUnit(reinterpret_cast<unsigned char*>(&sMask),sizeof(SMask));
 } 
 return(true);
}
bool CIOData::AddUnit(unsigned char *ptr,unsigned char size)
{
 Lock_sData();	
 if (sData.FileName==NULL || sData.Buffer==NULL || sData.Size==0)
 {
  UnLock_sData();	  
  return(false);
 }
 if (sData.Size<=sData.Amount+size)//нужен новый буфер
 {
  unsigned char *buffer=new unsigned char[(sData.Size+size)*2];
  memcpy(buffer,sData.Buffer,sData.Size);
  sData.Size=(sData.Size+size)*2;
  delete(sData.Buffer);
  sData.Buffer=buffer;
 }
 //записываем данные в буфер
 memcpy(sData.Buffer+sData.Amount,ptr,size);
 sData.Amount+=size;
 UnLock_sData(); 
 return(true); 
}

bool CIOData::AddDataNonDelta(double value,long time)
{
 SaveAll=true;
 AddData(value,time);//добавляем её в список
 SaveAll=false;
 return(true);
}

//установить необходимость полной записи буфера при следующей подготовки данных для записи
bool CIOData::SaveCacheOn(void)
{
 Lock_sData();
 sData.SaveCache=true;
 UnLock_sData();
 return(true);
}
//произвести запись  
bool CIOData::Save(void)
{
 unsigned long max_size=1024*16;
 //делаем копию имени файла и данных, удаляя данные из буфера
 Lock_sData();
 if (sData.Amount==0 || (sData.SaveCache==false && sData.Amount<max_size) || sData.FileName==NULL || sData.Buffer==NULL)//записывать не нужно
 {
  UnLock_sData();
  return(true);
 }
 unsigned char *buffer=new unsigned char[sData.Amount];
 memcpy(buffer,sData.Buffer,sData.Amount);
 unsigned long amount=sData.Amount;
 char *filename=new char[strlen(sData.FileName)*2+2];
 strcpy(filename,sData.FileName); 
 sData.Amount=0;
 sData.SaveCache=false;
 UnLock_sData();

 //сохраняем данные
 FILE *file=fopen(filename,"ab");
 if (file!=NULL)
 {
  fwrite(buffer,amount,1,file); 	
  fclose(file);
 }  
 /*
 int FileID=open(filename,O_APPEND|O_WRONLY);
 if (FileID<0) FileID=open(filename,O_CREAT|O_WRONLY);
 if (FileID>=0)
 {
  //сохраняем данные
  write(FileID,buffer,amount);
  close(FileID);
 }*/
 delete[](buffer);
 delete[](filename);
 return(true);
}

bool CIOData::Release(void)
{
 if (FirstData==false)//если одна точка у нас уже есть, запишем вторую
 {
  //добавим последнюю точку в список
  FirstData=true;
  AddDataNonDelta(OldValue,LastTime);//добавляем её в список
  SaveCacheOn();//устанавливаем флаг полной записи данных
  Save();
 }
 Lock_sData();
 if (sData.FileName!=NULL) delete[](sData.FileName);
 sData.FileName=NULL;
 if (sData.Buffer!=NULL) delete[](sData.Buffer);
 sData.Buffer=NULL;
 sData.Size=0;
 sData.Amount=0;
 sData.SaveCache=false;
 UnLock_sData();
 return(true);
}