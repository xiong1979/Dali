
#define SHORT_ADDR  (1)
#define DIRECT      (1)

/// 转换地址
#define Dali_Address(Address, Short, Direct) \
      (((Short) == SHORT_ADDR? 0: 0x80) + ((Address) << 1) + ((Direct) == DIRECT? 0 : 0x01))


// 直接短地址
uint8_t Dali_DirectShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);
  
}

// 直接组地址
uint8_t Dali_DirectGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}
// 间接短地址
void Dali_InDirectShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}
// 间接组地址
void Dali_InDirectGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, !DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}

// 设置单个灯亮度
void Dali_SetDimLevel(uint8_t Address, uint8_t Level)
{
  Dali_DirectShort(Address, Level);
}

// 设置一组灯亮度，包括
void Dali_SetDimLevelGrp(uint8_t Address, uint8_t Level)
{
  Dali_DirectGroup(Address, Level);
}

// 短地址配置
void Dali_ConfigShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);
  
  // 需要重发一次
  DaliMaster_SendCmd(Address, Command, 0, 1);  
}
// 组地址配置
void Dali_ConfigGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, !DIRECT);
  
  // 需要重发一次
  DaliMaster_SendCmd(Address, Command, 0, 1);  
}

#define CFG_RESET                                   0x20
#define CFG_STORE_ACTUAL_LEVEL_IN_THE_DTR           0x21
#define CFG_STORE_THE_DTR_AS_MAX_LEVEL              0x2A

#define CFG_ADD_TO_GROUP                            0x60
#define CFG_REMOVE_FROM_GROUP                       0x70

#define CFG_STORE_DTR_AS_SHORT_ADDRESS              0x80

void Dali_SetShortAddress(uint8_ Address, uint8_t NewAddress)
{
  Dali_ConfigShort(Address, NewAddress);
}

// 短地址配置
void Dali_QueryShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);

  DaliMaster_SendCmd(Address, Command, 1, 0);  
}
// 组地址配置
void Dali_QueryGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, !DIRECT);

  DaliMaster_SendCmd(Address, Command, 1, 0);  
}

#define QRY_ACTUAL_LEVEL                        0xA0
#define QRY_MAX_LEVEL                           0xA1
#define QRY_MIN_LEVEL                           0xA2

void Dali_QueryActualLevel(uint8_t Address)
{
  Dali_QueryShort(Address, QRY_ACTUAL_LEVEL);
}




