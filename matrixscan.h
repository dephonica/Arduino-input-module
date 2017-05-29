
class MatrixScan
{
private:
  const int inputsCount_, outputsCount_;
  const byte *inputVector_, *outputVector_;

  RTC &rtc_;
  MsgQueue &queue_;
  
  CLongTimer scanTimer_;
  
  char hotInputs_[MaxOutputsCount][MaxInputsCount];
  bool stateInputs_[MaxOutputsCount][MaxInputsCount];
  
  byte activeRow_;
  
  void nextScan()
  {
    // Read inputs
    for (byte n = 0; n < inputsCount_; n++)
    {
      hotInputs_[activeRow_][n] += digitalRead(inputVector_[n]) == HIGH ? 1 : -1;
      
      if (hotInputs_[activeRow_][n] < 0) 
      {
        hotInputs_[activeRow_][n] = 0;
        
        if (stateInputs_[activeRow_][n] == true)
        {
          stateInputs_[activeRow_][n] = false;

          byte key[1] = {(activeRow_ * inputsCount_) + n};
          queue_.push(rtc_.get_datetime().ToMilliseconds(), MSG_CMD_KEYDOWN, key, 1);
        }
      }
      
      if (hotInputs_[activeRow_][n] >= IntegrationPeriods) 
      {
        hotInputs_[activeRow_][n] = IntegrationPeriods;
        
        if (stateInputs_[activeRow_][n] == false)
        {
          stateInputs_[activeRow_][n] = true;
          
          byte key[1] = {(activeRow_ * inputsCount_) + n};
          queue_.push(rtc_.get_datetime().ToMilliseconds(), MSG_CMD_KEYUP, key, 1);
        }
      }
    }
    
    // Switch output to the next
    digitalWrite(outputVector_[activeRow_], HIGH);
    
    activeRow_++;
    if (activeRow_ >= outputsCount_) activeRow_ = 0;
    
    digitalWrite(outputVector_[activeRow_], LOW);    
  }

  void setupPio()
  {
    for (byte n = 0; n < inputsCount_; n++)
      pinMode(inputVector_[n], INPUT);

    for (byte n = 0; n < outputsCount_; n++)
      pinMode(outputVector_[n], OUTPUT);
  }
  
public:
  MatrixScan(RTC &rtc, MsgQueue &queue, const byte *inputVector, const byte inputsCount, 
    const byte *outputVector, const byte outputsCount) : 
      rtc_(rtc),
      queue_(queue),
      scanTimer_(RowScanPeriodMs),
      inputVector_(inputVector),
      inputsCount_(inputsCount),
      outputVector_(outputVector),
      outputsCount_(outputsCount)
  {
    setupPio();
    
    activeRow_ = 0;
    digitalWrite(outputVector_[activeRow_], LOW);
    
    scanTimer_.start();
  }
  
  inline void tick()
  {
    scanTimer_.tick();
    
    if (scanTimer_.elapsed())
    {
      nextScan();
    }
  }
};
