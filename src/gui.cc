#include "gui.h"

//----------------------------------------------------------------------------------------------------
//добавить строку в список
//----------------------------------------------------------------------------------------------------
void AddStringInList(PtWidget_t *pt_widget,char *string)
{
 long size=0;	
 //считаем размер строки
 while(string[size]!=0) size++; 	 
 //делаем копию строки
 char *str=new char[size*2+1];
 size=0;
 while(string[size]!=0)
 {
  str[size]=string[size];
  size++;
 }  	 
 str[size]=0;	
 char *sptr=str;	
 long pos=0; 
 //делим строку по частям
 while(1)
 {
  if (str[pos]==0 || str[pos]==10)//строка разбивается на части
  {
   bool done=false;	
   if (str[pos]==0) done=true;
   str[pos]=0;
   const char *cstring=sptr;
   //добавляем строку в список
   PtListAddItems(pt_widget,&cstring,1,0);
   if (done==true) break;//строка закончилась
   sptr=str+pos+1;
  }
  pos++;
 }
 delete[](str);
}
//----------------------------------------------------------------------------------------------------
//получить количество элементов в списке
//----------------------------------------------------------------------------------------------------
unsigned short GetListSize(PtWidget_t *pt_widget)
{
 PtArg_t args[1];
 short *num=NULL;
 char **items=NULL;
 PtSetArg(&args[0],Pt_ARG_ITEMS,&items,&num);	
 PtGetResources(pt_widget,1,args);
 return(static_cast<unsigned short>(*num));
}
//----------------------------------------------------------------------------------------------------
//получить позицию верхнего отображаемого элемента в списке
//----------------------------------------------------------------------------------------------------
unsigned short GetTopItemPosInList(PtWidget_t *pt_widget)
{
 unsigned short *ptr;	
 PtGetResource(pt_widget,Pt_ARG_TOP_ITEM_POS,&ptr,0);
 return(*ptr);
}
//----------------------------------------------------------------------------------------------------
//задать позицию верхнего отображаемого элемента в списке
//----------------------------------------------------------------------------------------------------
void SetTopItemPosInList(PtWidget_t *pt_widget,unsigned short pos)
{
 PtSetResource(pt_widget,Pt_ARG_TOP_ITEM_POS,pos,0);	
}
//----------------------------------------------------------------------------------------------------
//выделить элемент списка
//----------------------------------------------------------------------------------------------------
void SelectListElement(PtWidget_t *pt_widget,unsigned short index)
{
 PtListSelectPos(pt_widget,index+1);
}
//----------------------------------------------------------------------------------------------------
//отмпенить выделение элемента списка
//----------------------------------------------------------------------------------------------------
void UnselectListElement(PtWidget_t *pt_widget,unsigned short index)
{
 PtListUnselectPos(pt_widget,index+1);
}
//----------------------------------------------------------------------------------------------------
//узнать, выделен ли элемент списка
//----------------------------------------------------------------------------------------------------
bool IsSelectedListElement(PtWidget_t *pt_widget,char *name)
{
 unsigned short pos=PtListItemPos(pt_widget,name);
 if (pos==0) return(false);//элемент не найден
 PtArg_t args[1];
 short *num=NULL;
 unsigned short *items=NULL;
 PtSetArg(&args[0],Pt_ARG_SELECTION_INDEXES,&items,&num);
 PtGetResources(pt_widget,1,args);
 for(long n=0;n<(*num);n++)
 {
  if (items[n]==pos) return(true);	
 }
 return(false); 	
}
//----------------------------------------------------------------------------------------------------
//очистить список
//----------------------------------------------------------------------------------------------------
void ClearList(PtWidget_t *pt_widget)
{
 PtListDeleteAllItems(pt_widget);//очищаем список
}
//----------------------------------------------------------------------------------------------------
//установить/снять блокировку
//----------------------------------------------------------------------------------------------------
void SetBlocked(PtWidget_t *pt_widget,bool blocking)
{
 unsigned long *ptr;
 PtGetResource(pt_widget,Pt_ARG_FLAGS,&ptr,0);//считаем состояние
 unsigned long flag=*(ptr);
 if (blocking==true) flag|=Pt_BLOCKED;
 else
 {
  if (flag&Pt_BLOCKED) flag^=Pt_BLOCKED;
 }
 PtSetResource(pt_widget,Pt_ARG_FLAGS,flag,Pt_BLOCKED);//установим состояние кнопки
}
//----------------------------------------------------------------------------------------------------
//получить номер выбранной строки в ComboBox
//----------------------------------------------------------------------------------------------------
unsigned short GetSelectedItemInComboBox(PtWidget_t *pt_widget)
{
 unsigned short *ptr;
 PtGetResource(pt_widget,Pt_ARG_CBOX_SEL_ITEM,&ptr,0);//считаем выбранную строку
 return(*ptr);
}
//----------------------------------------------------------------------------------------------------
//очистить выпадающий список
//----------------------------------------------------------------------------------------------------
void ClearComboBox(PtWidget_t *pt_widget)
{
 PtListDeleteAllItems(pt_widget);//очищаем список
}
//----------------------------------------------------------------------------------------------------
//добавить строку в выпадающий список
//----------------------------------------------------------------------------------------------------
void AddStringInComboBox(PtWidget_t *pt_widget,char *string)
{
 const char *cstring=string;
 //добавляем строку в список
 PtListAddItems(pt_widget,&cstring,1,0);
}
//----------------------------------------------------------------------------------------------------
//установить выбранную строку в выпадающем списке
//----------------------------------------------------------------------------------------------------
void SetSelectedItemInComboBox(PtWidget_t *pt_widget,unsigned short index)
{
 PtSetResource(pt_widget,Pt_ARG_CBOX_SEL_ITEM,index,0);//установим выбранную строку как 1
}
//----------------------------------------------------------------------------------------------------
//записать в числовое поле значение
//----------------------------------------------------------------------------------------------------
void SetNumbericFloatValue(PtWidget_t *pt_widget,double value)
{
 PtSetResource(pt_widget,Pt_ARG_NUMERIC_VALUE,&value,0);
}
//----------------------------------------------------------------------------------------------------
//считать из числового поля значение
//----------------------------------------------------------------------------------------------------
double GetNumbericFloatValue(PtWidget_t *pt_widget)
{
 double *d_ptr;
 PtGetResource(pt_widget,Pt_ARG_NUMERIC_VALUE,&d_ptr,0);//считаем значение
 return(*d_ptr);	
}
//----------------------------------------------------------------------------------------------------
//задать текст компонента
//----------------------------------------------------------------------------------------------------
void SetWidgetText(PtWidget_t *pt_widget,char *text)
{
 PtSetResource(pt_widget,Pt_ARG_TEXT_STRING,text,0);
}
//----------------------------------------------------------------------------------------------------
//считать текст виджета
//----------------------------------------------------------------------------------------------------
char* GetWidgetText(PtWidget_t *pt_widget)
{
 char *text;
 PtGetResource(pt_widget,Pt_ARG_TEXT_STRING,&text,0);
 return(text);
}
//----------------------------------------------------------------------------------------------------
//получить состояние кнопки OnOff
//----------------------------------------------------------------------------------------------------
bool GetButtonOnOffState(PtWidget_t *pt_widget)
{
 //узнаем состояние кнопки
 unsigned short *ptr;
 PtGetResource(pt_widget,Pt_ARG_ONOFF_STATE,&ptr,0);
 if ((*ptr)!=0) return(true);
 return(false);
}

