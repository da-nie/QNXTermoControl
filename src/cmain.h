#ifndef CMAIN_H
#define CMAIN_H

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/neutrino.h>
#include <inttypes.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "gui.h"
#include "creceivethread.h"
#include "csave.h"

//количество точек в графике тренда
#define TREND_SIZE 500
//максимум тренда (от 0)
#define TREND_MAX_VALUE 10000

//бесконечное малое
#define ZERO_EPS 0.00000001

class CMain
{
 protected:
  //-Переменные класса-------------------------------------------------------
  CReceiveThread cReceiveThread;//класс получения данных от термодатчика
  bool SaveEnabled;//включена ли запись данных
  unsigned long TickCounter;//счётчик тактов
  float TempTrendArray[TREND_SIZE];//данные тренда  
  long TrendSize;//количество точек в тренде
  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CMain();
  //-Деструктор класса-------------------------------------------------------
  ~CMain();
  //-Переменные класса-------------------------------------------------------
  //-Замещённые функции предка-----------------------------------------------
  //-Новые функции класса----------------------------------------------------
 public:  
  bool OnOpening(void);//открытие окна
  bool OnClosing(void);//закрытие окна
  void Redraw(void);//отрисовать картинку
  void OnTimer(void);//обработка таймера    
  void OnActivate_Button_Save(void);//нажата кнопка сохранения данных
  void OnActivate_Button_ClearTrend(void);//нажата кнопка очистки графика
  //-Функции обработки сообщений класса--------------------------------------
  //-Новые функции класса----------------------------------------------------
  //-Прочее------------------------------------------------------------------
 protected:
};

#endif