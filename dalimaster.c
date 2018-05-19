#include <stdint.h>

/// 一次dali事务
typedef struct __dalimastertrans
{
  uint8_t   ubAddr;
  uint8_t   ubCommand;
  uint8_t   ubNeedResponse;
  uint8_t   ubResponse;
  uint8_t   ubNeedRepeat;
}tsDaliTransmission;

#define TRANSMISSION_NUM    10
static tsDaliTransmission Transmission[TRANSMISSION_NUM];
static uint8_t   subCurTrans = 0;

static StateMachine DaliMasterState;

static void DaliMaster_Idle(tEvent *e);
static void DaliMaster_Send(tEvent *e);
static void DaliMaster_Wait(tEvent *e);
#define DaliMaster_DispEvent(e)     DispatchEvent(DaliMasterState, (e))

static void DaliMaster_StartTimer(uint32_t Time);
static void DaliMaster_StopTimer();


////
void DaliMaster_SendCmd(uint8_t Address, uint8_t Command, uint8_t NeedReponse, uint8_t NeedRepeat)
{
  Transmission[subCurTrans].ubAddr          = Address;
  Transmission[subCurTrans].ubCommand       = Command;
  Transmission[subCurTrans].ubNeedResponse  = NeedResponse;
  Transmission[subCurTrans].ubNeedRepeat    = NeedRepeat;
  
  subCurTrans ++;
}

///
void DaliMaster_Idle(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_DaliMaster_Send:
    STATE_TO(DaliMaster_Send);
    break;
    
  case E_EXIT:
    break;
  }
}

/// 构造dali发送帧，有地址+数据 组成
uint16_t GetForwardFrame(void)
{

    return (((FRAME)Transmission[subCurTrans].ubAddr) << 8) + Transmission[subCurTrans].ubCommand;
}

/// 
void DaliMaster_Send(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:  
    Dali_Send(GetForwardFrame());   /// 向传输层发送数据  
    break;
    
  case E_DaliMaster_Sent:          /// 发送完成
    if (Transmission[subCurTrans].NeedResponse)   /// 需要slave应答
    {
      STATE_TO(DaliMaster_Wait);     
    }
    else     
    {
      STATE_TO(DaliMaster_ReSend);     /// 转达重发
    }
    break;
    
  case E_EXIT:
    break;
  }
}

void DaliMaster_ReSend(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    if (Transmission[subCurTrans].ubNeedRepeat)   
    {
      DaliMaster_StartTimer(100);  /// 需要重发，等待100 毫秒重发
    }
    else
    {
      STATE_TO(DaliMaster_Silence);   /// 不需要重发
    }
    break;
    
  case E_DaliMaster_TimeOut:
    Transmission[subCurTrans].ubNeedRepeat = FALSE;  // 等待100毫秒时间到，准备重发， 清除需要重发的标志
    STATE_TO(DaliMaster_Send);
    break;
    
  case E_EXIT:
    DaliMaster_StopTimer();
    break;
  }
}
void DaliMaster_Wait(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY: 
    Dali_Recv();       /// 启动接收
    break;
    
  case E_DaliMaster_Received:
    
    STATE_TO(DaliMaster_Resend);   /// 接收完成， 转到Resend，看看是否需要重发
    break;
    
  case E_DaliMaster_NoReceived:
    
    break;
    
  case E_EXIT:
    break;
  }
}

void DaliMaster_Silence(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    DaliMaster_StartTimer(10);  /// 发送下个需要10毫秒静默期
    break;
    
  case E_DaliMaster_TimeOut:
    STATE_TO(DaliMaster_Idle);   /// 静默期接收，转到空闲状态
    break;
    
  case E_EXIT:
    DaliMaster_StopTimer();
    break;
  }
}

void DaliMaster_GetData(tsBackwardFrame *Frame)
{
  tEvent e;
  if (Frame->Valid)
  {
    Transmission[subCurTrans].ubNeedRepeat = Frame;
    
    e->Id = E_DaliMaster_Received;
  }
  else
  {
    e->Id = E_DaliMaster_NoReceived;  
  }
  
  DaliMaster_DispEvent(e);    
}





