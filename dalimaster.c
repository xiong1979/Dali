#include <stdint.h>

/// һ��dali����
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

/// ����dali����֡���е�ַ+���� ���
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
    Dali_Send(GetForwardFrame());   /// ����㷢������  
    break;
    
  case E_DaliMaster_Sent:          /// �������
    if (Transmission[subCurTrans].NeedResponse)   /// ��ҪslaveӦ��
    {
      STATE_TO(DaliMaster_Wait);     
    }
    else     
    {
      STATE_TO(DaliMaster_ReSend);     /// ת���ط�
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
      DaliMaster_StartTimer(100);  /// ��Ҫ�ط����ȴ�100 �����ط�
    }
    else
    {
      STATE_TO(DaliMaster_Silence);   /// ����Ҫ�ط�
    }
    break;
    
  case E_DaliMaster_TimeOut:
    Transmission[subCurTrans].ubNeedRepeat = FALSE;  // �ȴ�100����ʱ�䵽��׼���ط��� �����Ҫ�ط��ı�־
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
    Dali_Recv();       /// ��������
    break;
    
  case E_DaliMaster_Received:
    
    STATE_TO(DaliMaster_Resend);   /// ������ɣ� ת��Resend�������Ƿ���Ҫ�ط�
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
    DaliMaster_StartTimer(10);  /// �����¸���Ҫ10���뾲Ĭ��
    break;
    
  case E_DaliMaster_TimeOut:
    STATE_TO(DaliMaster_Idle);   /// ��Ĭ�ڽ��գ�ת������״̬
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





