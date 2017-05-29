
// General settings /////////////////////////////////////////////////////////

const byte InputVector[] = {A0,A1,A2,A3,13};
const byte OutputVector[] = {2,3,4,5,6,7,8,9,10,11,12};

const byte InputsCount = sizeof(InputVector)/sizeof(InputVector[0]);
const byte OutputsCount = sizeof(OutputVector)/sizeof(OutputVector[0]);

// MaxrixScan settings //////////////////////////////////////////////////////

const byte MaxOutputsCount = 12;
const byte MaxInputsCount = 5;

const byte RowScanPeriodMs = 2;
const byte IntegrationPeriods = 3;

// MsgQueue settings ////////////////////////////////////////////////////////

const byte MaxMessagesInQueue = 4;

/////////////////////////////////////////////////////////////////////////////

typedef unsigned long dword;
typedef long long int qword;

/////////////////////////////////////////////////////////////////////////////

#include "longtime.h"
#include "datetime.h"
#include "rtc.h"
#include "msg_queue.h"
#include "matrixscan.h"
#include "host_xchange.h"

///////////////////////////////////////////////////////////////////////////

// Host-synced real time clock
RTC rtc;

// Message queue
MsgQueue messages_in_(rtc), messages_out_(rtc);

// Matrix inputs
MatrixScan mtx(rtc, messages_out_, InputVector, InputsCount, OutputVector, OutputsCount);

// Host exchange
HostXchange xchange(rtc, messages_in_, messages_out_);

///////////////////////////////////////////////////////////////////////////

void on_income_message(MESSAGE &msg)
{
  if (msg.command == MSG_CMD_SYNCTIME) 
  {
    messages_out_.push(rtc.get_datetime().ToMilliseconds(), MSG_CMD_SYNCDONE);
  }
}

///////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  // Outcome processing
  mtx.tick();
  
  // Income processing
  xchange.tick();
  
  if (messages_in_.count() > 0)
  {
    MESSAGE msg;
    messages_in_.pop(&msg);
    on_income_message(msg);
  }
}

