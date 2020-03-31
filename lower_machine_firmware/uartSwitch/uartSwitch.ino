//注意，输入端检测到低电平时代表触发，输出端触发时输出高电平，因此控制用的开关应跨接在输入引脚和地之间，选用的继电器模块应该是高电平触发的
//all input pin triggers while LOW!!!! and output HIGH!!!!

//#define SELF_RECOVERY  //uncomment this if you are using a self recovery switch 如果你使用的是自回弹的开关，把这行代码取消注释

#define PIN_COUNT 4     //define the quantity of pins that used for IO 定义开关数量，数字1-8
#define BAUD 9600 //baud rate 串口波特率，一般不用改
/*
 * now you need to define the pin that used for input and out put
 * make sure the quantity of input pins and output pins are same, and also qual to 'pinCount' definded above!
 * this firmware support up to 8-way switch (8 pins for input and 8 pins for output)
 * because arduino nano only have 19 pins for io, and uart port need 2 pins to work. Also, states of 8 pins can be easily presented using 8bits(char or uint8_t) and esay for transmitting.
 */
//定义用到的io口，inputPinList定义用于输入的引脚，outputPinList定义用于输出的引脚，引脚之间是一一对应关系，即数组中相同位置的输入引脚控制相同位置的输出引脚，数量必须与之前定义的开关数量相同
//以下为arduino uno/nano/pro mini/mini的预定义引脚

//uint8_t inputPinList[] = {2, 3, 4, 5, 6, 7, 8, 9};    //maximum pin using in arduino uno/nano/pro mini/mini
//uint8_t outputPinList[] = {10,11,12,13,14,15,16,17};  //maximum pin using in arduino uno/nano/pro mini/mini

//以下为esp8266 nodemcu的预定义引脚
const uint8_t inputPinList[] = {0, 4, 5,10}; //maximum pin using in esp8266-12e nodemcu
const uint8_t outputPinList[] = {15, 13, 12, 14};//maximum pin using in esp8266-12e nodemcu

//========以下代码无需修改=======//
//========code below no need to change======//

//commands defination
#define QUERY_COMMAND 0x18
#define SET_COMMAND 0x29
#define ACK_COMMAND 0x33

uint8_t switch_state = 0;// state of all switches
uint8_t pre_input_state = 0;// state of previous input
uint8_t mask = 0;// bit mask for state correction

void initialize();
void readState();
void writeState();
void handleSerial();


void setup() {
  initialize();
}

void loop() {
  readState();
  handleSerial();
  writeState();
}

void initialize()
{
  Serial.begin(BAUD);
  for(int i = 0;i < PIN_COUNT;i++)
  {
    pinMode(inputPinList[i],INPUT_PULLUP);
    pinMode(outputPinList[i], OUTPUT);
    pre_input_state += 1;// initialize the primary state, in order to prevent dead lock
    mask += 1;// initialize bit mask by 'PIN_COUNT' . for example, if PIN_COUNT = 5,then mask will equals to 00011111(binary)
    if(i != PIN_COUNT - 1)
    {
      pre_input_state = pre_input_state << 1;
      mask = mask << 1; 
    }
  }
}

void readState()
{
  //go through the inputPinList for every pin's value(0 or 1)
  uint8_t cur_input_state = 0;
  for (int i = 0; i < PIN_COUNT; i++)
  {
    cur_input_state += digitalRead(inputPinList[i]);
    if(i != PIN_COUNT-1)
    {
      cur_input_state = cur_input_state << 1; 
    }
  }

  //generate new switches' state 
  #ifdef SELF_RECOVERY
  {
    if(pre_input_state != cur_input_state)
    {
      switch_state = switch_state ^ (~cur_input_state);
    }
  }
  #else
  {
    switch_state = switch_state ^ (pre_input_state ^ cur_input_state);
  }
  #endif

  
  switch_state = switch_state & mask;//add mask and get the correct result
  pre_input_state = cur_input_state;
  delay(15);//Jitter elimination
}

void writeState()
{
  uint8_t state_to_set = switch_state;
  for (int i = PIN_COUNT - 1; i >= 0; i--)
  {
    digitalWrite(outputPinList[i], state_to_set % 2);
    state_to_set = state_to_set >> 1;
  }
}

void handleSerial()
{
  uint8_t cmd;
  cmd = Serial.read();
  if(cmd == QUERY_COMMAND)//query for state
  {
    Serial.write(ACK_COMMAND);
    Serial.write(switch_state);
  }
  else if(cmd == SET_COMMAND)// set new state
  {
    delay(10);//wait for message
    if(Serial.available())
    {
      switch_state = Serial.read(); 
      Serial.write(ACK_COMMAND);
    }
  }
}
