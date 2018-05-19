
#include <stdint.h>

typedef uint32_t FORWARD;
typedef uint16_t BACKWARD;

typedef struct __forwardframe
{
  FORWARD     Data;
  uint8_t     ToSend;
  uint8_t     Sent;
}tsForwardFrame;

typedef struct __backwardframe
{
  uint16_t  Data;
  uint8_t   ToReceive;
  uint8_t   Received;
  uint8_t   Valid;
  uint8_t   Halfbits;
  uint8_t   Bits;
}tsBackwardFrame;

typedef enum
{
  DALI_INSEND,
  DALI_INRECV,
};

typedef enum tEvent;
typedef int StateMachine;

static StateMachine DaliState;

#define Dali_DispEvent(e)     DispatchEvent(DaliState, (e))

#define STATE_TO(State)   

static void DaliSend_Idle(tEvent *e);
static void DaliSend_Start(tEvent *e);
static void DaliSend_SendFirstHalf(tEvent *e);
static void DaliSend_SendSecondHalf(tEvent *e);
static void DaliSend_SendEnd(tEvent *e);

static void DaliRecv_Idle(tEvent *e);
static void DaliRecv_Start(tEvent *e);
static void DaliRecv_RecvFirstHalf(tEvent *e);
static void DaliRecv_RecvSecondHalf(tEvent *e);


#define TIME_TE        416  // 416.67 us
#define DALI_PIN_OUT_HIGH()
#define DALI_PIN_OUT_LOW()
#define DALI_PIN_OUT_TOGGLE()
#define DALI_PIN_IN()

#define MostBit       ( 1 < (sizeof(FORWARD) * 8 - 1))
void DaliSend_StartTime(uint32_t  Time);
void DaliSend_StopTime(void);

typedef enum
{
  DALI_RECV_ERROR,
  DALI_RECV_OK,
};

typedef struct __dali_status
{
  uint8_t     Re;
}tsDaliStatus;

tsForwardFrame      thisForwardFrame;
tsBackwardFrame     thisBackWardFrame;
uint8_t       subDaliStatus;

//
void Dali_Init(void)
{
  Dali_InitTimer();
  STATE_INIT(Dali_Idle);
}
// 启动发送
void Dali_Send(FORWARD Frame)
{
  thisForwardFrame.Data = Frame;  
  
  tEvent e;
  
  e.Id = E_Dali_Send;
  
  Dali_DispEvent(e);
}
// 启动接收
void Dali_Recv(void)
{
  tEvent e;
  
  e.Id = E_Dali_Recv;
  
  Dali_DispEvent(e);    
  
}

void Dali_InitTimer()
{
  
}

void Dali_StartTimer(uint32_t Time)
{
}

void Dali_StopTimer()
{
}

/// dali 定时器中断
void DaliTimerISR(void)
{
  tEvent e;
  
  e.Id = E_Dali_TimeOut;
  
  Dali_DispEvent(e);
}

void Dali_Idle(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    break;
    
  case E_Dali_Send:
    STATE_TO(DaliSend_Start);
    break;

  case E_Dali_Recv:
    STATE_TO(DaliRecv_WaitStart);
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
    Dali_StartTimer(TIME_TE);
    break;
    
  case E_Dali_TimeOut:
    STATE_TO(DaliSend_SendSecondHalf);
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }
}
void DaliSend_SendSecondHalf(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    DALI_PIN_OUT_TOGGLE();
    
     Dali_StartTimer(TIME_TE);
    break;
    
  case E_Dali_TimeOut:
    ForwardFrame.Sent ++;
    ForwardFrame.Data <<= 1;
    if (ForwardFrame.ToSend !=  ForwardFrame.Sent)  // 发送下一位
    {
      STATE_TO(DaliSend_SendFirstHalf);
    }
    else        /// 发送结束
    {
      STATE_TO(DaliSend_SendEnd);
    }
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }
}

void DaliSend_SendEnd(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:   // 发送停止位
     DALI_PIN_OUT_HIGH();
    
     Dali_StartTimer(TIME_TE * 4);
    break;
    
  case E_Dali_TimeOut:   // 停止位发送完成
    STATE_TO(DALI_IDLE);
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }
}


void DaliRecv_WaitStart(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    // forward 到 backward之间， 22TE
    Dali_StartTimer(TIME_TE * 22);  
    // input ISR
    Enable_InputISR();
    break;
    
  case E_Dali_Received:                   // 等到了数据，转到等待结束状态
    STATE_TO(DaliRecv_WaitFinish);
    break;

  case E_Dali_TimeOut:           // 超时没有等到 数据
    // 禁止input ISR
    Disable_InputISR();
    thisBackwardFrame.Valid = 0;
    STATE_TO(Dali_Idle);
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }
}

void Dali_InitBackFrame(void)
{
  thisBackwardFrame.Data = 0;
  thisBackwardFrame.Valid = 0;
  thisBackwardFrame.Received = 0;
}
void DaliRecv_WaitFinish(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    Dali_StartTimer(TIME_TE * 22);  // backward帧最多22个TE
    LastTime = 0;
    Dali_InitBackFrame();
    break;
    
  case E_Dali_TimeOut:           // 超时，完成接收数据
    // 检查是否还有剩余位，如果，肯定是 1
    if (thisBackwardFrame.Halfbits == 1)
    {
      thisBackwardFrame.Bits <<= 1;
      thisBackwardFrame.Bits |= 1;
      if ( DALI_RECV_OK == GetOneBit(thisBackwardFrame.Bits))
      {
        thisBackwardFrame.Halfbits = 0;
      }
    }
    // 通知上层
    DaliMaster_GetData(&thisBackwardFrame);
    // 禁止input ISR
    Disable_InputISR();
    // 转到空闲状态
    STATE_TO(Dali_Idle);
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }
}

// dali作为slave，或者多主机模式
void Dali_Monitor(tEvent *e)
{
  switch(e->Id)
  {
  case E_ENTRY:
    Dali_StartTimer(TIME_TE * 22);  // backward帧最多22个TE
    break;
    
  case E_Dali_TimeOut:           // 超时，完成接收数据
    STATE_TO(Dali_Idle);
    break;
    
  case E_Dali_Received:
    Dali_StartTimer(TIME_TE * 22);
    break;
    
  case E_EXIT:
    Dali_StopTimer();
    break;
  }  
}

// 
void Enable_InputISR()
{
}
//
void Disable_InputISR(void)
{
  
}

interrupt void DaliInputISR(void)
{
  tEvent e;
  
  e.Id = E_Dali_Received;
  
  Dali_DispEvent(e);  
  
  if (DALI_RECV_OK == Receive())
  {    
  }

  
}

uint8_t GetOneBit(uint8_t Data)
{
  if ((Data & 0x03) == 0x01)   ///  上升沿， 为 1
  {
    thisBackwardFrame.Data << = 1;
    thisBackwardFrame.Data |= 1;
  }
  else ((Data & 0x03) == 0x02)   /// 下降沿， 为 0
  {
    thisBackwardFrame.Data << = 1;
  }
  else
  {
    return DALI_RECV_ERROR;
  }      
  
  return DALI_RECV_OK;
}




uint8_t Receive(void)
{
  static uint8_t 
  static uint8_t pin;
  uint32_t  now, timeInterval;
  
  // 第一位，特殊处理
  if (LastTime == 0)
  {
    LastTime = 1;    
    
    thisBackwardFrame.Halfbits = 0;
    
    thisBackwardFrame.Bits = 0;

    pin = DALI_PIN_IN();
    
    return DALI_RECV_OK;
  }
  
  now = getTime();
  timeInterval = now - LastTime;
  LastTime = now;
  
  if (timeInterval < (uint32_t)(TIME_TE * 0.9))  // 太小脉宽，错误信号
  {
    return DALI_RECV_ERROR;
  }
  else if (timeInterval < (uint32_t)(TIME_TE * 1.1))  // 半位
  {
    thisBackwardFrame.Bits << = 1;
    thisBackwardFrame.Bits |= pin;                    // 确认上一个半位
    thisBackwardFrame.Halfbits ++;
    pin = DALI_PIN_IN();
  }
  else if (timeInterval < (uint32_t)(TIME_TE * 2.2))  // 两个半位
  {
    thisBackwardFrame.Bits <<= 1;
    thisBackwardFrame.Bits |= pin;
    thisBackwardFrame.Bits <<= 1;
    thisBackwardFrame.Bits |= pin;    
    thisBackwardFrame.Halfbits += 2;
    pin = DALI_PIN_IN();              // 记下当前位状态
  }
  else                                  // 太大脉宽，错误信号
  {
    return DALI_RECV_ERROR;
  }
  
  if (thisBackwardFrame.Halfbits == 2)  
  {
    if ( DALI_RECV_OK == GetOneBit(thisBackwardFrame.Bits))
    {
      thisBackwardFrame.Halfbits = 0;
    }
    else
    {
      return DALI_RECV_ERROR；
    }
  }
  else if (halfbits == 3)
  {
    if ( DALI_RECV_OK == GetOneBit(thisBackwardFrame.Bits >> 1))
    {
      thisBackwardFrame.Halfbits = 1;
    }
    else
    {
      return DALI_RECV_ERROR；
    }
  }
  
  return DALI_RECV_OK;
  
}




