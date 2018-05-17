#include <stdint.h>

typedef struct __dalimaster
{
  uint8_t   ubAddr;
  uint8_t   ubCommand;
  uint8_t   ubNeedResponse;
  uint8_t   ubNeedRepeat;
}tsDaliTransmission;

#define TRANSMISSION_NUM    10
tsDaliTransmission Transmission[TRANSMISSION_NUM];
uint8_t   subCurTrans = 0;

static StateMachine DaliMaster;

void DaliMaster_Idle(tEvent *e);
void DaliMaster_Send(tEvent *e);
void DaliMaster_Wait(tEvent *e);


void DaliMaster_StartTimer(uint32_t Time);
void DaliMaster_StopTimer();

void DaliMaster_Idle(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_DaliMaster_Send:
    STATE_TO(DaliSend_Start);
    break;
    
  case E_EXIT:
    break;
  }
}

void GetForwardFrame(void)
{

    (((FRAME)Transmission[subCurTrans].Addr) << 8) + Transmission[subCurTrans].Command;
}

void DaliMaster_Send(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:  
    Dali_Send(GetForwardFrame());    
    break;
    
  case E_DaliMaster_Sent:
    if (Transmission[subCurTrans].NeedResponse)
    {
      STATE_TO(DaliMaster_Wait);
    }
    else
    {
      STATE_TO(DaliMaster_ReSend);
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
      DaliMaster_StartTimer(100);  /// 100 毫秒重发
    }
    else
    {
      STATE_TO(DaliMaster_Silence);   
    }
    break;
    
  case E_DaliMaster_TimeOut:
    Transmission[subCurTrans].ubNeedRepeat = FALSE;  // 已经重发了
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
    Dali_Recv();
    break;
    
  case E_DaliMaster_Received:
    STATE_TO(DaliMaster_Silence);
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
    STATE_TO(DaliMaster_Idle);
    break;
    
  case E_EXIT:
    DaliMaster_StopTimer();
    break;
  }
}


