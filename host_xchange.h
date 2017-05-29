const char hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


class HostXchange
{
private:
  RTC &rtc_;
  MsgQueue &queue_in_, &queue_out_;

  // income
  bool nibble_;
  byte income_byte_, income_ptr_, income_crc_;
  
  // outcome
  bool sending_;
  byte outcome_byte_, outcome_ptr_, outcome_crc_;
  
  union DataMap
  {
    byte data_[20];
    MESSAGE msg_;
  };
  
  DataMap Income, Outcome;

  void resetReceive()
  {
    nibble_ = false;
    income_crc_ = 0;
    income_ptr_ = 0;    
  }

  void handleMessage()
  {
    if (income_ptr_ == 0) return;
    
    if (income_crc_ != 0)
    {
      byte crc[1] = {income_ptr_};
      queue_out_.push(rtc_.get_datetime().ToMilliseconds(), MSG_ERR_CRCFAIL, (byte*)&crc, 1);
      resetReceive();
      return;
    }
    
    queue_in_.push(Income.msg_);
    resetReceive();
  }

  inline void handleIncome()
  {
    if (Serial.available())
    {
      byte cLook = Serial.read();
      
      if (cLook == 0x0a || cLook == 0x0d) 
        handleMessage();
      else
      {
        if (cLook >= '0' && cLook <= '9')
          cLook -= '0';
        else if (cLook >= 'a' && cLook <= 'f')
          cLook -= 'a' - 10;
        else if (cLook >= 'A' && cLook <= 'F')
          cLook -= 'A' - 10;

        if (nibble_ == false)
        {
          income_byte_ = cLook << 4;
          nibble_ = true;
        } else
        {
          income_byte_ |= cLook;
          nibble_ = false;
        
          Income.data_[income_ptr_++] = income_byte_;
          income_crc_ -= income_byte_;
        }
      }
    }
  }
  
  inline void handleOutcome()
  {
    if (sending_ == true)
    {
      if (outcome_ptr_ >= sizeof(MESSAGE))
      {
        byte data = outcome_crc_;
        Serial.write(hex_table[data >> 4]);
        Serial.write(hex_table[data & 0x0f]);
        Serial.write(0x0d);

        sending_ = false;
      } else
      {
        byte data = Outcome.data_[outcome_ptr_++];
        Serial.write(hex_table[data >> 4]);
        Serial.write(hex_table[data & 0x0f]);
        
        outcome_crc_ -= data;
      }
      
      return;
    }
    
    if (queue_out_.count() > 0)
    {
      sending_ = true;
      outcome_ptr_ = 0;
      outcome_crc_ = 0;
      
      queue_out_.pop(&Outcome.msg_);
    }
  }

public:
  HostXchange(RTC &rtc, MsgQueue &queue_in, MsgQueue &queue_out) :
    rtc_(rtc),
    queue_in_(queue_in),
    queue_out_(queue_out)
  {
    income_ptr_ = 0;
    income_crc_ = 0;
    nibble_ = false;
    sending_ = false;
  }
  
  void tick()
  {
    handleIncome();
    handleOutcome();
  }
};

