
class RTC_DateTime
{
private:
  byte day_, month_;
  word year_;
  byte hour_, minute_, second_;
  word millisecond_;

public:
  RTC_DateTime()
  {
  }
  
  RTC_DateTime(long long int milliseconds)
  {
    SetMilliseconds(milliseconds);
  }
  
  byte Day()
  {
    return day_;
  }
  
  void Day(const byte day)
  {
    day_ = day;
  }
  
  byte Month()
  {
    return month_;
  }
  
  void Month(const byte month)
  {
    month_ = month;
  }
  
  word Year()
  {
    return year_;
  }
  
  void Year(const word year)
  {
    year_ = year;
  }
  
  byte Hour()
  {
    return hour_;
  }
  
  void Hour(const byte hour)
  {
    hour_ = hour;
  }
  
  byte Minute()
  {
    return minute_;
  }
  
  void Minute(const byte minute)
  {
    minute_ = minute;
  }
  
  byte Second()
  {
    return second_;
  }
  
  void Second(const byte second)
  {
    second_ = second;
  }
  
  word Millisecond()
  {
    return millisecond_;
  }
  
  void Millisecond(const word millisecond)
  {
    millisecond_ = millisecond;
  }
    
  long long int ToMilliseconds()
  {
    long long int milliseconds = ((long long int)year_) * 366LL * 24LL * 3600LL * 1000LL +
      ((long long int)month_ - 1) * 31LL * 24LL * 3600LL * 1000LL + ((long long int)day_ - 1) * 24LL * 3600LL * 1000LL +
      ((long long int)hour_) * 3600LL * 1000LL + ((long long int)minute_) * 60LL * 1000LL + 
      ((long long int)second_) * 1000LL + millisecond_;
      
    return milliseconds;
  }
  
  void SetMilliseconds(long long int timestamp)
  {
    year_ = timestamp / (366LL * 24LL * 3600LL * 1000LL);
    
    long long int intermediate = timestamp % (366LL * 24LL * 3600LL * 1000LL);
    
    month_ = intermediate / (31LL * 24LL * 3600LL * 1000LL);
    
    intermediate %= (31LL * 24LL * 3600LL * 1000LL);
    
    day_ = intermediate / (24LL * 3600LL * 1000LL);
    
    intermediate %= (24LL * 3600LL * 1000LL);
    
    hour_ = intermediate / (3600LL * 1000LL);
    
    intermediate %= (3600LL * 1000LL);
    
    minute_ = intermediate / (60LL * 1000LL);
    
    intermediate %= (60LL * 1000LL);
    
    second_ = intermediate / 1000LL;
    millisecond_ = intermediate % 1000LL;

    month_ += 1;
    day_ += 1;
  }
};
