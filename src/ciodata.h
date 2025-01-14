#ifndef CIODATA_H
#define CIODATA_H

//****************************************************************************************************
//класс для работы с сохранением файлов данных
//****************************************************************************************************

//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

//****************************************************************************************************
//настройки компиляции
//****************************************************************************************************
#pragma pack(1)

//****************************************************************************************************
//макроопределения
//****************************************************************************************************

//бесконечно малое эпсилон
#define CIO_EPS 0.0000000001


//****************************************************************************************************
//перечисления
//****************************************************************************************************

//типы данных
enum TYPE
{
 TypeCHAR,//char
 TypeBYTE,//unsigned char
 TypeSHORT,//short
 TypeWORD,//unsigned short
 TypeLONG,//long
 TypeDWORD,//unsigned long
 TypeFLOAT,//float
 TypeDOUBLE//double
};

//****************************************************************************************************
//класс для работы с сохранением файлов данных
//****************************************************************************************************

class CIOData
{
 public:
  struct SData
  {
   unsigned char *Buffer;//буфер для данных
   unsigned long Size;//размер буфера для данных
   unsigned long Amount;//количество данных в буфере
   char *FileName;//имя файла для записи
   bool SaveCache;//флаг необходимости полной записи данных при очередной подготовке к записи  
  } sData;//текущие данные
  pthread_mutex_t mutex_ID;//мьютекс для доступа к данным
  
  unsigned char SizeOfElement;//размер элемента
  long LastTime;//последнее значение времени
  double Delta;//разность, при превышениии которой можно сохранять данные
  double Scale;//масштабный коэффициент
  unsigned char Type;//тип данных
  bool FirstData;//это первый запуск
  bool SaveAll;//сохранять всё
  double OldValue;//старое значение
  long LastAddTime;//последнее значение времени, которое было сохранено в файле
  
 public:
  //конструктор
  CIOData(void);
  //деструктор
  ~CIOData(void);  
  bool Init(char *filename,char type,double delta,double scale,unsigned long tickofsave);//инициализация
  bool AddData(double value,long time);//добавить данные и сохранить, если пришло время
  bool AddDataNonDelta(double value,long time);//добавить данные и сохранить (без прореживания), если пришло время
  bool AddUnit(unsigned char *ptr,unsigned char size);//добавить элемент в массив
  bool SaveCacheOn(void);//установить необходимость полной записи буфера при следующей подготовки данных для записи
  bool Save(void);//произвести запись  
  bool Release(void);//освободить данные
 protected:
  void Lock_sData(void);//захватить мьютекс доступа к sData
  void UnLock_sData(void);//освободить мьютекс доступа к sData
};

#endif 