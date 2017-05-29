const byte MSG_CMD_KEYUP      = 1;
const byte MSG_CMD_KEYDOWN    = 2;
const byte MSG_CMD_SYNCTIME   = 3;
const byte MSG_CMD_SYNCDONE   = 4;

const byte MSG_CMD_SWITCHON   = 5;
const byte MSG_CMD_SWITCHOFF  = 6;

const byte MSG_ERR_CRCFAIL    = 7;

typedef struct __MESSAGE
{
  qword timestamp;
  word command;
  
  union
  {
    byte pbyte[8];
    word pword[4];
    dword pdword[2];
    qword pqword;
  };
} MESSAGE;

class MsgQueue
{
private:
  RTC &rtc_;

  byte message_low_, message_high_;
  MESSAGE queue_[MaxMessagesInQueue];

  MESSAGE *alloc_message()
  {
    if (rtc_.is_valid() == false) return NULL;

    MESSAGE *msg = &queue_[message_high_];
    
    message_high_++;
    if (message_high_ >= MaxMessagesInQueue)
      message_high_ = 0;
      
    if (message_high_ == message_low_)
      message_low_++;
    if (message_low_ >= MaxMessagesInQueue)
      message_low_ = 0;
      
    return msg;
  }

public:
  MsgQueue(RTC &rtc) :
    rtc_(rtc),
    message_low_(0), message_high_(0)
  {
  }
  
  byte count()
  {
    if (message_high_ >= message_low_)
      return message_high_ - message_low_;
      
    return MaxMessagesInQueue - message_low_ + message_high_;
  }
  
  void push(unsigned long long timestamp, word command)
  {
    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    msg->timestamp = timestamp;
    msg->command = command;
  }
  
  void push(unsigned long long timestamp, word command, byte *params, byte param_count)
  {
    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    msg->timestamp = timestamp;
    msg->command = command;
    
    for (byte n = 0; n < param_count && n < 8; n++)
      msg->pbyte[n] = params[n];
  }
  
  void push(unsigned long long timestamp, word command, word *params, byte param_count)
  {
    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    msg->timestamp = timestamp;
    msg->command = command;
    
    for (byte n = 0; n < param_count && n < 4; n++)
      msg->pword[n] = params[n];
  }

  void push(unsigned long long timestamp, word command, dword *params, byte param_count)
  {
    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    msg->timestamp = timestamp;
    msg->command = command;
    
    for (byte n = 0; n < param_count && n < 2; n++)
      msg->pdword[n] = params[n];
  }

  void push(unsigned long long timestamp, word command, qword param)
  {
    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    msg->timestamp = timestamp;
    msg->command = command;
    msg->pqword = param;
  }

  void push(MESSAGE &pmsg)
  {
    if (pmsg.command == MSG_CMD_SYNCTIME) 
      rtc_.sync(pmsg.timestamp);

    MESSAGE *msg = alloc_message();
    if (msg == NULL) return;
    
    memcpy(msg, &pmsg, sizeof(MESSAGE));
  }
  
  void pop(MESSAGE *pmsg)
  {
    if (count() > 0)
    {
      memcpy(pmsg, &queue_[message_low_], sizeof(MESSAGE));
      
      message_low_++;
      if (message_low_ >= MaxMessagesInQueue)
        message_low_ = 0;
    }
  }
};

