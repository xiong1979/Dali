
#include <stdint.h>

typedef uint32_t FRAME;

typedef struct __forwardframe
{
  FRAME       Data;
  uint8_t     ToSend;
  uint8_t     Sent;
}tsForwardFrame;

typedef struct __backwardframe
{
  uint16_t  Data;
  uint8_t   ToReceive;
  uint8_t   Received;
}tsBackwardFrame;

typedef enum
{
  DALI_INSEND,
  DALI_INRECV,
};

typedef enum tEvent;
typedef int StateMachine;
static StateMachine DaliSend;
static StateMachine DalieRecv;

#define STATE_TO(State)   

void DaliSend_Idle(tEvent *e);
void DaliSend_Start(tEvent *e);
void DaliSend_SendFirstHalf(tEvent *e);
void DaliSend_SendSecondHalf(tEvent *e);
void DaliSend_SendEnd(tEvent *e);

void DaliRecv_Idle(tEvent *e);
void DaliRecv_Start(tEvent *e);
void DaliRecv_RecvFirstHalf(tEvent *e);
void DaliRecv_RecvSecondHalf(tEvent *e);


#define TIME_TE        416  // 416.67 us
#define DALI_PIN_OUT_HIGH()
#define DALI_PIN_OUT_LOW()
#define DALI_PIN_OUT_TOGGLE()
#define DALI_PIN_IN()

#define MostBit       ( 0x8000)
void DaliSend_StartTime(uint32_t  Time);
void DaliSend_StopTime(void);

tsForwardFrame      thisForwardFrame;
tsBackwardFrame     thisBackWardFrame;
uint8_t       subDaliStatus;

void Dali_Send(FRAME Frame)
{
    thisForwardFrame.Data = Frame;
    
}

void Dali_Recv(void)
{
  // 启动定时器，时间22TE，如果超过，说明dali input 没有来，dali错误
  // 启动 dali input中断， 在中断里，设置定时器中断时间要50 TE, 大于22TE
  
}

void DaliSend_Idle(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:    
    break;
    
  case E_Dali_Send:
    STATE_TO(DaliSend_Start);
    break;
    
  case E_EXIT:
    break;
  }
}
void DaliSend_Start(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    STATE_TO(DaliSend_SendFirstHalf);
    break;
    
  case E_EXIT:
    break;
  }
}
void DaliSend_SendFirstHalf(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    if (MostBit & ForwardFrame.Data)
    {
      DALI_PIN_OUT_LOW();
    }
    else
    {
      DALI_PIN_OUT_HIGH();
    }
    DaliSend_StartTime(TIME_TE);
    break;
    
  case E_Dali_TimeOut:
    STATE_TO(DaliSend_SendSecondHalf);
    break;
    
  case E_EXIT:
    DaliSend_StopTime();
    break;
  }
}
void DaliSend_SendSecondHalf(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    DALI_PIN_OUT_TOGGLE();
    
     DaliSend_StartTime(TIME_TE);
    break;
    
  case E_Dali_TimeOut:
    ForwardFrame.Sent ++;
    ForwardFrame.Data <<= 1;
    if (ForwardFrame.ToSend !=  ForwardFrame.Sent)
    {
      STATE_TO(DaliSend_SendFirstHalf);
    }
    else
    {
      STATE_TO(DaliSend_SendEnd);
    }
    break;
    
  case E_EXIT:
    DaliSend_StopTime();
    break;
  }
}

void DaliSend_SendEnd(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    DALI_PIN_OUT_HIGH();
    
     DaliSend_StartTime(TIME_TE * 4);
    break;
    
  case E_Dali_TimeOut:
    STATE_TO(DALI_IDLE);
    break;
    
  case E_EXIT:
    DaliSend_StopTime();
    break;
  }
}


void DaliRecv_Idle(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_Dali_Recv:
    STATE_TO(DaliRecv_Start);
    break;
    
  case E_EXIT:
    break;
  }
}
void DaliRecv_Start(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_EXIT:
    break;
  }
}
void DaliRecv_RecvFirstHalf(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_EXIT:
    break;
  }
}
void DaliRecv_RecvSecondHalf(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_EXIT:
    break;
  }
}

typedef enum
{
  DALI_RECV_ERROR,
  DALI_RECV_OK,
};

uint8_t GetOneBit(uint8_t Data)
{
  if (Data == 0x01)   ///  上升沿， 为 1
  {
    thisBackwardFrame.Data << = 1;
    thisBackwardFrame。Data |= 1;
  }
  else (Data == 0x02)
  {
    thisBackwardFrame.Data << = 1;
  }
  else
  {
    return DALI_RECV_ERROR;
  }      
  
  return DALI_RECV_OK;
}

interrupt void DaliTimer(void)
{
  // 在发送模式
  if (subDaliStatus == DALI_INSEND)
  {  
  }
  
  // 在接收模式
  else if (subDaliStatus == DALI_INRECV)
  {
    // 在接收模式下，定时中断产生，说明dali超时，出错了
  }
}
interrupt void DaliInputISR(void)
{

  if (DALI_RECV_OK == Receive())
  {
    // diable input 
    // diable timer 
    //  notify dali 上层控制
    
  }
  
}

uint8_t Receive(void)
{
  uint8_t halfbits = 0, bits;
  uint8_t pin;
  uint32_t  timePass;
  
  if (timeInited == 0)
  {
    timeInited = 1;
    //在中断里，设置定时器中断时间要50 TE, 大于22TE
    // DaliTimerReset(50TE)
    pin = DALI_PIN_IN();
    return DALI_RECV_OK;
  }
  
  timePass = getTime();
  
  if (timePass < (TIME_TE * 0.9))  // 太小脉宽，错误信号
  {
    return DALI_RECV_ERROR;
  }
  else if (timePass < (TIME_TE * 1.1))  // 半位
  {
    bit << = 1;
    bit |= pin;
    halfbit ++;
    pin = DALI_PIN_IN();
  }
  else if (timePass < (TIME_TE * 2.2))  // 两个半位
  {
    bit <<= 1;
    bit |= pin;
    bit <<= 1;
    bit |= pin;    
    halfbits += 2;
    pin = DALI_PIN_IN();
  }
  else                                  // 太大脉宽，错误信号
  {
    return DALI_RECV_ERROR;
  }
  
  if (halfbits == 2)  
  {
    if ( DALI_RECV_OK == GetOneBit(bits))
    {
      halfbits = 0;
    }
    else
    {
      return DALI_RECV_ERROR；
    }
  }
  else if (halfbits == 3)
  {
    if ( DALI_RECV_OK == GetOneBit(bits >> 1))
    {
      halfbits = 1;
    }
    else
    {
      return DALI_RECV_ERROR；
    }
  }
  
  //在中断里，设置定时器中断时间要50 TE, 大于22TE
  // DaliTimerReset(50TE)
  
  return DALI_RECV_OK;
  
}




