///////////////////////////////////////////////////////////////////////////

class RTC
{
private:
  bool is_valid_;
  
  long long int bias_;
  
  unsigned long last_millis_;
  unsigned long long millis_offset_;

  inline long long int ll_millis()
  {
    unsigned long ms = millis();
    
    if (last_millis_ != 0 && ms < last_millis_)
    {
      millis_offset_ += 1<<31;
    }
    
    last_millis_ = ms;
    
    return millis_offset_ + ms;
  }

public:
  RTC() :
    is_valid_(false),
    bias_(0),
    last_millis_(0),
    millis_offset_(0)
  {
  }
  
  void sync(RTC_DateTime &dt)
  {
    bias_ = dt.ToMilliseconds() - ll_millis();
    is_valid_ = true;
  }
  
  void sync(qword timestamp)
  {
    bias_ = timestamp - ll_millis();
    is_valid_ = true;
  }
  
  RTC_DateTime get_datetime()
  {
    RTC_DateTime dt(bias_ + ll_millis());
    return dt;
  }
  
  bool is_valid()
  {
    return is_valid_;
  }
};

