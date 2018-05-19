
#define SHORT_ADDR  (1)
#define DIRECT      (1)

/// ת����ַ
#define Dali_Address(Address, Short, Direct) \
      (((Short) == SHORT_ADDR? 0: 0x80) + ((Address) << 1) + ((Direct) == DIRECT? 0 : 0x01))


// ֱ�Ӷ̵�ַ
uint8_t Dali_DirectShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);
  
}

// ֱ�����ַ
uint8_t Dali_DirectGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}
// ��Ӷ̵�ַ
void Dali_InDirectShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}
// ������ַ
void Dali_InDirectGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, !DIRECT);
  
  DaliMaster_SendCmd(Address, Command, 0, 0);  
}

// ���õ���������
void Dali_SetDimLevel(uint8_t Address, uint8_t Level)
{
  Dali_DirectShort(Address, Level);
}

// ����һ������ȣ�����
void Dali_SetDimLevelGrp(uint8_t Address, uint8_t Level)
{
  Dali_DirectGroup(Address, Level);
}

// �̵�ַ����
void Dali_ConfigShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);
  
  // ��Ҫ�ط�һ��
  DaliMaster_SendCmd(Address, Command, 0, 1);  
}
// ���ַ����
void Dali_ConfigGroup(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, !SHORT_ADDR, !DIRECT);
  
  // ��Ҫ�ط�һ��
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

// �̵�ַ����
void Dali_QueryShort(uint8_t Address, uint8_t Command)
{
  Address = Dali_Address(Address, SHORT_ADDR, !DIRECT);

  DaliMaster_SendCmd(Address, Command, 1, 0);  
}
// ���ַ����
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




