#include "creceivethread.h"

SProtectedVariables sProtectedVariables;//защищённые переменные

//----------------------------------------------------------------------------------------------------
//поток приёма данных
//----------------------------------------------------------------------------------------------------
void *ReceiveThread(void *data)
{
 //блокируем любые сигналы для потока
 sigblock(255);
 CReceiveThread	*cReceiveThread_Ptr=reinterpret_cast<CReceiveThread*>(data);
 if (cReceiveThread_Ptr==NULL) return(NULL);
 while(1)
 {
  if (cReceiveThread_Ptr->GetExitState()==true) break;//требуется выйти из потока
  cReceiveThread_Ptr->Processing();
 }
 return(NULL);
}

//----------------------------------------------------------------------------------------------------
//устройство подключено
//----------------------------------------------------------------------------------------------------
void InsertionDevice(usbd_connection *connect,usbd_device_instance_t *instance)
{
 printf("Interfaces of Thermo is connected.\r\n");
 SDevice sDevice;
 sDevice.instance=instance;
 sDevice.device=NULL;
 sDevice.PipeAmount=0;
 long ret=usbd_attach(connect,sDevice.instance,0,&(sDevice.device));
 if (ret!=EOK)//сбой
 {
  printf("\t\tusbd_attach error! Error:%i\r\n",(int)ret);
  return;
 }
 printf("\t\tAttach device is ok.\r\n");
 //настраиваем конфигурацию
 if (usbd_select_config(sDevice.device,1)!=EOK) printf("\t\tusbd_select_config error!\r\n");//первая конфигурация 
 //открываем каналы для конечных точек
 usbd_descriptors_t *desc;
 usbd_desc_node *ifc,*ept;
 //получаем дескриптор интерфейса подключённого устройства
 usbd_interface_descriptor_t *interface;
 interface=usbd_interface_descriptor(sDevice.device,sDevice.instance->config,sDevice.instance->iface,sDevice.instance->alternate,&ifc);
 if (interface!=NULL)//интерфейс получен
 {
  sDevice.InterfaceAddr=interface->bInterfaceNumber;
  printf("\t\tInterface 0x%02x\r\n",sDevice.InterfaceAddr);
  //идём по всем конечным точкам устройста 	
  for(long eix=0;(desc=usbd_parse_descriptors(sDevice.device,ifc,USB_DESC_ENDPOINT,eix,&ept))!=NULL;eix++)
  {
   int addr=desc->endpoint.bEndpointAddress;  	
   if (desc->endpoint.bmAttributes==USB_ATTRIB_CONTROL) printf("\t\t\tEndpoint 0x%02x is Control.\r\n",addr);
   if (desc->endpoint.bmAttributes==USB_ATTRIB_ISOCHRONOUS) printf("\t\t\tEndpoint 0x%02x is Isochronus.\r\n",addr);
   if (desc->endpoint.bmAttributes==USB_ATTRIB_BULK) printf("\t\t\tEndpoint 0x%02x is bulk.\r\n",addr);
   if (desc->endpoint.bmAttributes==USB_ATTRIB_INTERRUPT) printf("\t\t\tEndpoint 0x%02x is interrupt.\r\n",addr);
   //создаём каналы с этими конечными точками
   SDevice::SPipe sPipe;
   sPipe.Addr=addr;   
   if (usbd_open_pipe(sDevice.device,desc,&sPipe.pipe)!=EOK)
   {
    printf("\t\t\tError open pipe for endpoint!\r\n");
    continue;
   }
   else
   {
    //выделяем URB для конечной точки
    sPipe.urb=usbd_alloc_urb(NULL);   	
    if (sPipe.urb==NULL) printf("\t\t\tusbd_alloc_urb error!\r\n");
    if (sDevice.PipeAmount<MAX_PIPE)
    {
     sDevice.sPipe_Array[sDevice.PipeAmount]=sPipe;
     sDevice.PipeAmount++;
    }
    printf("\t\t\tOpen pipe is ok.\r\n");
   }
  }
 } 
 else printf("\t\tGet interface descriptor error!\r\n");
 pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
 if (sProtectedVariables.ConnectedDeviceAmount<MAX_DEVICE)
 {
  sProtectedVariables.sDevice_Array[sProtectedVariables.ConnectedDeviceAmount]=sDevice;//добавляем устройство
  sProtectedVariables.ConnectedDeviceAmount++;
 }
 sProtectedVariables.CurrentConnectedDeviceAmount++;
 pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
}

//----------------------------------------------------------------------------------------------------
//устройство отключено
//----------------------------------------------------------------------------------------------------
void RemovalDevice(usbd_connection *connect,usbd_device_instance_t *instance)
{
 printf("Interfaces of Thermo is disconnected.\r\n");
 pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
 if (sProtectedVariables.CurrentConnectedDeviceAmount>0) sProtectedVariables.CurrentConnectedDeviceAmount--;
 pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
}
//----------------------------------------------------------------------------------------------------
//принимаются данные с конечной точки 0x81
//----------------------------------------------------------------------------------------------------
void DataReceive_81(usbd_urb *urb,usbd_pipe *pipe,void *data)
{ 	
 _uint32 status;
 _uint32 length;
 int error=usbd_urb_status(urb,&status,&length);
 if (error!=EOK) return;
 pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
 printf("Received 0x81: ");
 unsigned char *ptr=(unsigned char*)(data); 
 for(unsigned long n=0;n<length;n++) printf("%02x ",(unsigned int)ptr[n]);
 printf("\r\n"); 
 //sProtectedVariables.cThermoControl.SetData((unsigned char*)data,length)
 pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
}

//----------------------------------------------------------------------------------------------------
//принимаются данные с конечной точки 0x82
//----------------------------------------------------------------------------------------------------
void DataReceive_82(usbd_urb *urb,usbd_pipe *pipe,void *data)
{ 	
 _uint32 status;
 _uint32 length;
 int error=usbd_urb_status(urb,&status,&length);
 if (error!=EOK) return;
 pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
 sProtectedVariables.cThermoControl.SetData((unsigned char*)data,length);
 pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
 printf("Received 0x82: ");
 unsigned char *ptr=(unsigned char*)(data); 
 for(unsigned long n=0;n<length;n++) printf("%02x ",ptr[n]);
 printf("\r\n"); 
}

//----------------------------------------------------------------------------------------------------
//вывод сообщения об ошибке USBD
//----------------------------------------------------------------------------------------------------
void PrintUSBDStatus(long status)
{
 if (status&USBD_STATUS_INPROG) printf("USBD_STATUS_INPROG\r\n"); 
 if (status&USBD_STATUS_CMP) printf("USBD_STATUS_CMP\r\n");  
 if (status&USBD_STATUS_CMP_ERR) printf("USBD_STATUS_CMP_ERR\r\n");  
 if (status&USBD_STATUS_TIMEOUT) printf("USBD_STATUS_TIMEOUT\r\n");  
 if (status&USBD_STATUS_ABORTED) printf("USBD_STATUS_ABORTED\r\n"); 
 if (status&USBD_STATUS_CRC_ERR) printf("USBD_STATUS_CRC_ERR\r\n"); 
 if (status&USBD_STATUS_BITSTUFFING) printf("USBD_STATUS_BITSTUFFING\r\n"); 
 if (status&USBD_STATUS_TOGGLE_MISMATCH) printf("USBD_STATUS_TOGGLE_MISMATCH\r\n"); 
 if (status&USBD_STATUS_STALL) printf("USBD_STATUS_STALL\r\n"); 
 if (status&USBD_STATUS_DEV_NOANSWER) printf("USBD_STATUS_DEV_NOANSWER\r\n"); 
 if (status&USBD_STATUS_PID_FAILURE) printf("USBD_STATUS_PID_FAILURE\r\n"); 
 if (status&USBD_STATUS_BAD_PID) printf("USBD_STATUS_BAD_PID\r\n"); 
 if (status&USBD_STATUS_DATA_OVERRUN) printf("USBD_STATUS_DATA_OVERRUN\r\n"); 
 if (status&USBD_STATUS_DATA_UNDERRUN) printf("USBD_STATUS_DATA_UNDERRUN\r\n"); 
 if (status&USBD_STATUS_BUFFER_OVERRUN) printf("USBD_STATUS_BUFFER_OVERRUN\r\n"); 
 if (status&USBD_STATUS_BUFFER_UNDERRUN) printf("USBD_STATUS_BUFFER_UNDERRUN\r\n"); 
 if (status&USBD_STATUS_NOT_ACCESSED) printf("USBD_STATUS_NOT_ACCESSED\r\n");     
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//конструктор и деструктор класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CReceiveThread::CReceiveThread()
{
 pthread_mutex_init(&mutex_ID,NULL);//создаём мьютекс для доступа 
 pthread_ID=-1;
 ExitThread=false;
}
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CReceiveThread::~CReceiveThread()
{
 StopThread();
 pthread_mutex_destroy(&mutex_ID);//удаляем мьютекс
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//закрытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetupVendor(usbd_urb *urb,usbd_pipe *pipe,unsigned short request,unsigned short rtype,unsigned short value,unsigned short index,void *addr,unsigned long length)
{
 _uint32 status;
 _uint32 ret_length; 
 int error=usbd_setup_vendor(urb,URB_DIR_OUT|URB_SHORT_XFER_OK,rtype,request,value,index,addr,length);
 if (error!=EOK) printf("usbd_setup_vendor error %i!\r\n",error);
 error=usbd_io(urb,pipe,NULL,addr,500);
 if (error!=EOK) printf("usbd_io error %i!\r\n",error);
 error=usbd_urb_status(urb,&status,&ret_length);
 if (error!=EOK) printf("usbd_urb_status error %i!\r\n",error);
 else
 {
  printf("usbd_urb_status return: %ld and length: %ld\r\n",(long)status,(long)ret_length);
  PrintUSBDStatus((long)status); 
 }
}

//----------------------------------------------------------------------------------------------------
//обработка
//----------------------------------------------------------------------------------------------------
void CReceiveThread::Processing(void)
{
 usbd_connection *connection=NULL;
 //найденные конечные точки
 usbd_pipe *pipe_0[2];//канал для точки 0
 usbd_urb *urb_0[2];//выделенный URB точки 0

 usbd_pipe *pipe_81;//канал для точки 81
 usbd_urb *urb_81;//выделенный URB точки 81

 usbd_pipe *pipe_82;//канал для точки 82
 usbd_urb *urb_82;//выделенный URB точки 82
 
 //создаём мютекс защищённых глобальных переменных
 pthread_mutex_init(&sProtectedVariables.mutex_ID,NULL);//создаём мьютекс для доступа	
 
 sProtectedVariables.ConnectedDeviceAmount=0;//количество подключённых устройств
 sProtectedVariables.CurrentConnectedDeviceAmount=0;//количество устройств в настоящий момент времени
 	
 int error;	
 usbd_device_ident_t interest= 
 {
  USB_DEVICE_VENDOR_ID,
  USB_DEVICE_PRODUCT_ID,
  USBD_CONNECT_WILDCARD,
  USBD_CONNECT_WILDCARD,
  USBD_CONNECT_WILDCARD,
 };

 usbd_funcs_t funcs= 
 {
  _USBDI_NFUNCS,
  &InsertionDevice,
  &RemovalDevice,
  NULL
 };

 usbd_connect_parm_t cparms= 
 {
  NULL,
  USB_VERSION,
  USBD_VERSION,
  0,
  NULL, 
  0,
  0,
  &interest,
  &funcs
 };	

 //соединяемся со стеком USB	
 error=usbd_connect(&cparms,&connection);	
 if (error!=EOK)
 { 	
  printf("usbd_connect error %i!\r\n",error);
  if (error==EPROGMISMATCH) printf("EPROGMISMATCH\r\n");
  if (error==ENOMEM) printf("ENOMEM\r\n");
  if (error==ESRCH) printf("ESRCH\r\n");
  if (error==EACCES) printf("EACCES\r\n");
  if (error==EAGAIN) printf("EAGAIN\r\n");
  //уничтожаем мютекс
  pthread_mutex_destroy(&sProtectedVariables.mutex_ID);//удаляем мьютекс 
  return;  
 }
 void *data=usbd_alloc(URB_BUFFER_SIZE);
 unsigned char *c_data=(unsigned char*)(data);
 if (data==NULL)
 {
  printf("usbd_alloc error!\r\n");
  //отсоединяемся от стека USB
  usbd_disconnect(connection);
  //уничтожаем мютекс
  pthread_mutex_destroy(&sProtectedVariables.mutex_ID);//удаляем мьютекс 
  return;  
 }
 while(1)
 {
  pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
  bool done=ExitThread;
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
  if (done==true) break;//выходим из потока
  pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
  //ждём готовности интерфейсов
  if (sProtectedVariables.CurrentConnectedDeviceAmount!=2)
  {
   pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс  	
   delay(10);
   continue;
  }

  //подключены все устройства термодатчика
  //определим каналы точек
  pipe_0[0]=NULL;
  urb_0[0]=NULL;

  pipe_0[1]=NULL;
  urb_0[1]=NULL;
    
  pipe_81=NULL;
  urb_81=NULL;

  pipe_82=NULL;
  urb_82=NULL;

  for(long n=0;n<sProtectedVariables.ConnectedDeviceAmount;n++)
  {
   SDevice *sDevice_Ptr=&(sProtectedVariables.sDevice_Array[n]);   
   for(long m=0;m<sDevice_Ptr->PipeAmount;m++)
   {
    SDevice::SPipe *sPipe_Ptr=&(sDevice_Ptr->sPipe_Array[m]);
    if (sPipe_Ptr->Addr==0x81)
    {
     pipe_81=sPipe_Ptr->pipe;
     urb_81=sPipe_Ptr->urb;
     printf("Found endpoint 0x81.\r\n");
    }
    if (sPipe_Ptr->Addr==0x82)
    {
     pipe_82=sPipe_Ptr->pipe;
     urb_82=sPipe_Ptr->urb;
     printf("Found endpoint 0x82.\r\n");
    }
    if (sPipe_Ptr->Addr==0x00)
    {
     int i=sDevice_Ptr->InterfaceAddr;	
     pipe_0[i]=sPipe_Ptr->pipe;
     urb_0[i]=sPipe_Ptr->urb;     
     printf("Found endpoint 0x00 of interface:0x%02x.\r\n",i);
    }
   }
  }  

  pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
  printf("Thermo is connected.\r\n");
  //термодатчик подключён
  delay(500);
  printf("Step 1\r\n");
    
  c_data[0]=0x01;
  c_data[1]=0x01;
  SetupVendor(urb_0[0],pipe_0[0],0x21,0x09,0x0201,0x00,data,8);
  
  delay(500);
  printf("Step 2\r\n");  

  c_data[0]=0x01;
  c_data[1]=0x80;
  c_data[2]=0x33;
  c_data[3]=0x01;
  c_data[4]=0x00;
  c_data[5]=0x00;
  c_data[6]=0x00;
  c_data[7]=0x00;
  SetupVendor(urb_0[0],pipe_0[0],0x21,0x09,0x0200,0x01,data,8);
  
  error=usbd_setup_interrupt(urb_81,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x81 error %i\r\n",error);
  error=usbd_io(urb_81,pipe_81,DataReceive_81,data,500);
  
  error=usbd_setup_interrupt(urb_82,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x82 error %i\r\n",error);
  error=usbd_io(urb_82,pipe_82,DataReceive_82,data,500);
  
  delay(500);
  printf("Step 3\r\n");

  c_data[0]=0x01;
  c_data[1]=0x82;
  c_data[2]=0x77;
  c_data[3]=0x01;
  c_data[4]=0x00;
  c_data[5]=0x00;
  c_data[6]=0x00;
  c_data[7]=0x00;  
  SetupVendor(urb_0[0],pipe_0[0],0x21,0x09,0x0200,0x01,data,8);
  
  error=usbd_setup_interrupt(urb_81,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x81 error %i\r\n",error);
  error=usbd_io(urb_81,pipe_81,DataReceive_81,data,500);
  
  error=usbd_setup_interrupt(urb_82,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x82 error %i\r\n",error);
  error=usbd_io(urb_82,pipe_82,DataReceive_82,data,500);
  
  delay(500);
  printf("Step 4\r\n");

  c_data[0]=0x01;
  c_data[1]=0x86;
  c_data[2]=0xff;
  c_data[3]=0x01;
  c_data[4]=0x00;
  c_data[5]=0x00;
  c_data[6]=0x00;
  c_data[7]=0x00;  
  SetupVendor(urb_0[0],pipe_0[0],0x21,0x09,0x0200,0x01,data,8);
  
  error=usbd_setup_interrupt(urb_81,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x81 error %i\r\n",error);
  error=usbd_io(urb_81,pipe_81,DataReceive_81,data,500);
  
  error=usbd_setup_interrupt(urb_82,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
  if (error!=EOK) printf("usbd_setup_interrupt 0x82 error %i\r\n",error);
  error=usbd_io(urb_82,pipe_82,DataReceive_82,data,500);
  
  delay(500);
  printf("Step 5\r\n");

  //принимаем данные
  while(1)
  {     	
   delay(1000);
   pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
   sProtectedVariables.cThermoControl.NoData();//объявляем, что данных нет
   pthread_mutex_lock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
   
   printf("Question!\r\n");
   c_data[0]=0x01;
   c_data[1]=0x80;
   c_data[2]=0x33;
   c_data[3]=0x01;
   c_data[4]=0x00;
   c_data[5]=0x00;
   c_data[6]=0x00;
   c_data[7]=0x00;
   
   SetupVendor(urb_0[0],pipe_0[0],0x21,0x09,0x0200,0x01,data,8);
 
   printf("Wait answer...\r\n");
   pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
   if (sProtectedVariables.CurrentConnectedDeviceAmount!=2)//устройство отключено
   {       	
    pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
    break;
   }
   pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
   
   error=usbd_setup_interrupt(urb_81,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
   if (error!=EOK) printf("usbd_setup_interrupt 0x81 error %i\r\n",error);
   error=usbd_io(urb_81,pipe_81,DataReceive_81,data,500);
   if (error==ENODEV) break;   

   error=usbd_setup_interrupt(urb_82,URB_DIR_IN|URB_SHORT_XFER_OK,data,URB_BUFFER_SIZE);
   if (error!=EOK) printf("usbd_setup_interrupt 0x82 error %i\r\n",error);
   error=usbd_io(urb_82,pipe_82,DataReceive_82,data,500);
   if (error==ENODEV) break;

   pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
   done=ExitThread;
   pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
   if (done==true) break;//выходим из потока
  } 
  //ждём отключения устройства
  while(1)
  {
   delay(10);
   pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
   if (sProtectedVariables.CurrentConnectedDeviceAmount!=0)
   {
    pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
    continue;
   }
   pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
   break;
  }
  //освобождаем все занятые ресурсы
  printf("Release connected devices.\r\n");
  pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мьютекс
  for(long n=0;n<sProtectedVariables.ConnectedDeviceAmount;n++)
  {
   printf("\tIndex: 0x%02x\r\n",(int)n);
   SDevice *sDevice_Ptr=&(sProtectedVariables.sDevice_Array[n]);
   //удаляем данные для конечных точек  
   for(long m=0;m<sDevice_Ptr->PipeAmount;m++)
   {
    usbd_close_pipe(sDevice_Ptr->sPipe_Array[m].pipe);//закрываем канал
    //удаляем память URB
    if (sDevice_Ptr->sPipe_Array[m].urb!=NULL) usbd_free_urb(sDevice_Ptr->sPipe_Array[m].urb);
   } 
   sDevice_Ptr->PipeAmount=0;
   //отключаем устройство	
   usbd_detach(sDevice_Ptr->device);
  }
  sProtectedVariables.ConnectedDeviceAmount=0;
  pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мьютекс
  //повторяем заново  
  delay(100);
 }
 usbd_free(data);
 //отсоединяемся от стека USB
 usbd_disconnect(connection);
 //уничтожаем мютекс
 pthread_mutex_destroy(&sProtectedVariables.mutex_ID);//удаляем мьютекс 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//открытые функции класса
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//----------------------------------------------------------------------------------------------------
//запустить поток приёма данных
//----------------------------------------------------------------------------------------------------
bool CReceiveThread::StartThread(void)
{
 //останавливаем поток
 StopThread();
 ExitThread=false;//выходить из потока не нужно
 //запускаем поток
 pthread_attr_t pt_attr;
 pthread_attr_init(&pt_attr);
 pthread_attr_setdetachstate(&pt_attr,PTHREAD_CREATE_DETACHED);
 pthread_attr_setinheritsched(&pt_attr,PTHREAD_EXPLICIT_SCHED);
 pthread_attr_setschedpolicy(&pt_attr,SCHED_RR); 
 pt_attr.param.sched_priority=RECEIVE_THREAD_PRIORITY; 
 pthread_create(&pthread_ID,&pt_attr,ReceiveThread,this);//создаём поток приёма данных
 return(true);
}
//----------------------------------------------------------------------------------------------------
//остановить поток контроля CAN
//----------------------------------------------------------------------------------------------------
bool CReceiveThread::StopThread(void)
{
 if (pthread_ID!=-1)
 {
  pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
  ExitThread=true;//требуется выход из потока
  pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
  pthread_join(pthread_ID,NULL);//;ждём завершения потока
 }
 pthread_ID=-1;
 return(true);
}

//----------------------------------------------------------------------------------------------------
//возвратить, требуется ли выход из потока
//----------------------------------------------------------------------------------------------------
bool CReceiveThread::GetExitState(void)
{
 pthread_mutex_lock(&mutex_ID);//блокируем мьютекс
 bool exitthread=ExitThread;
 pthread_mutex_unlock(&mutex_ID);//разблокируем мьютекс
 return(exitthread);
}
//----------------------------------------------------------------------------------------------------
//получить текущую температуру
//----------------------------------------------------------------------------------------------------
bool CReceiveThread::GetCurrentTemp(float &temp)
{
 pthread_mutex_lock(&sProtectedVariables.mutex_ID);//блокируем мютекс
 bool ret=sProtectedVariables.cThermoControl.GetCurrentTemp(temp);
 pthread_mutex_unlock(&sProtectedVariables.mutex_ID);//разблокируем мютекс
 return(ret);
}