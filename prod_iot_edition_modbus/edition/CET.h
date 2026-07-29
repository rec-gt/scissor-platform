#include <stdint.h>
#include "./SubGlobals.h"

#ifndef CET_H
#define CET_H

/*=== @Derek. @Owen, @後人 請閲讀此説明，瞭解如何修改modbus config ===*/
// 1. 你要先瞭解基礎modbus，至少要知道什麽是Holding Register
// 2. 定義正確的modbus參數，注意是“正確的”，請參閲產品資料，取得slave id, baud rate, 8N1/8E1 這些資料，填錯資料讀不到的話別怪我
// 3. 定義你要讀取的範圍，
// 3.1. 首先要知道自己查詢的數據類型是什麽，是Holding Register? coils? discrete input? 還是其他？
// 3.2. 然後要知道自己查詢的地址是多少，是address 0-5？還是46-47？一定要清楚
//     ***注意，絕大部分情況，address都是從0開始的，雜牌的modbus，例如RGT的iFCU,
//     ***對，你沒看錯，我説的就是RGT的iFCU，它的modbus address (Input Register)，是從30000開始
// 4. 修改operate()裏面的代碼，來實現需求




/*=== ！！！@Derek, 請在這裏定義modbus參數 ===*/
constexpr uint16_t SLAVE_ID = 12;                  // target device 的 slave id
constexpr uint16_t MB_BAUD_RATE = 9600;            // serial 的 baud rate, 根據產品修改參數
constexpr uint16_t MB_SERIAL_CONFIG = SERIAL_8E1;  // e.g., SERIAL_8E1, SERIAL_8N1, 根據產品切換使用
// 寫錯以上參數的，用錯Parity的，這是你的問題，用不了別怪我
constexpr uint8_t MB_DATA_SIZE = 100;  // 不要改了，就這樣

/*=== ！！！@Derek, 請在這裏定義你要查找的範圍，不要超過100個config ===*/
uint8_t mbReadConfig[MB_DATA_SIZE][3] = {
  // format: { modbus data type, modbus data address, number of consecutive data (default 2) }
  // 通常read得最多的是INPUT_REGISTERS、HOLDING_REGISTERS，其他COILS、DISCRETE_INPUTS少機會用到，按需求自己開發
  // 有連續的data的話，請盡量在寫下有多少個連續數據，能寫多少就多少，這是性能考量
  // { HOLDING_REGISTERS, 0, 6 },   // 從address 0開始，連續讀取6個數據，也就是讀取: 0, 1, 2, 3, 4, 5 位置的數據
  // { HOLDING_REGISTERS, 46, 2 },  // 從address 46開始，連續讀取2個數據，也就是讀取: 46, 47 位置的數據
  // { INPUT_REGISTERS, i, n },     // 根據需求及產品Modbus Table説明而定，請自行研究
  // { COILS, i, n },               // 根據需求及產品Modbus Table説明而定，請自行研究
  // { DISCRETE_INPUTS, i, n },     // 根據需求及產品Modbus Table説明而定，請自行研究

  { HOLDING_REGISTERS, 0, 6 },   // 根據CET Modbus Table, HR addr 從 0 讀取, 連續讀取6個數據
  { HOLDING_REGISTERS, 46, 2 },  // 根據CET Modbus Table, HR addr 從 46 讀取, 連續讀取2個數據
};

uint8_t mbNumOfConfig = 0;
uint8_t mbNumOfConfigSwitch = 0;
uint8_t mbNumOfData = 0;
uint8_t mbDataPtr = 0;
uint32_t mbHrData[MB_DATA_SIZE] = {};
uint32_t newPayload[16] = {};  // 這裏寫死16，不解釋，問就是找Keith大哥處理

class CET {
private:
  float IEEEfloat(uint32_t val) {
    union {
      uint32_t i;
      float f;
    } u;
    u.i = val;
    return u.f;
  }

  // 不要動query()的代碼
  void query() {
    if (mbNumOfConfigSwitch >= mbNumOfConfig) {
      mbDataPtr = 0;
      mbNumOfConfigSwitch = 0;
      return;  // end of an epoch
    }

    uint8_t mbDataType = mbReadConfig[mbNumOfConfigSwitch][0];
    uint8_t mbDataStartIdx = mbReadConfig[mbNumOfConfigSwitch][1];
    uint8_t mbDataRange = mbReadConfig[mbNumOfConfigSwitch][2];

    mbRtuClient.requestFrom(SLAVE_ID, mbDataType, mbDataStartIdx, mbDataRange);

    for (uint8_t j = 0; j < mbDataRange; j++) {
      mbHrData[mbDataPtr] = mbRtuClient.read();
      mbDataPtr++;
    }

    mbNumOfConfigSwitch++;
  }

  /*=== ！！！@Derek, here is where bit & arithmetic operates happen ===*/
  void operate() {
    // 請在此處處理數據，並把數據填充到16個預設好的位置

    // ***此處僅以CET產品作説明, 其他產品的操作五花八門, 請參考其説明書指示操作***
    // 順帶一提，雜牌的產品Modbus會有意想不到的驚喜問題，有問題的話，一定是其他牌子的問題，我話説完，不接受任何反駁
    // 例如: 我查詢著溫度，吃著火鍋還唱著歌，突然就變成65535了，所以，沒有bug的modbus，才是好modbus

    // 根據CET説明書, Voltage由兩個Hex組成，而且其格式遵循IEEE float，因此操作如下：

    // 1. 取得組成Voltage的兩個Hex
    uint16_t b1 = mbHrData[0];
    uint16_t b2 = mbHrData[1];

    // 2. 合并b1及b2數據
    uint32_t voltage_raw = ((uint32_t)b1 << 16) | b2;

    // 3. 轉換成IEEE Float
    uint32_t voltage = IEEEfloat(voltage_raw);

    // 4. 把數據填入AI/AO
    // 注意，因爲iot平臺限制，Project的數據數量最終只會收斂成16個數據，即12個AI + 4個AO，除非Keith大哥大發慈悲把AI/AO數量拓展更大
    // 因此這裏我只預留16個位置讓你填入
    // 你只需要決定把哪個數據塞入哪個位置, 例如把voltage塞入第一個位置:
    newPayload[0] = voltage;
    // newPayload[1] = current;
    // newPayload[2] = kwh;
    // newPayload[3] = oiiai;
    // newPayload[4] = "Owen is gay";
    // newPayload[5] = 3.14159265358979323846;
    // newPayload[6] = "大感謝";
    // newPayload[7] = "3億啦";
    // newPayload[8] = 0;
    // newPayload[9] = 0;
    // newPayload[10] = 0;
    // newPayload[11] = 0;
    // newPayload[12] = 0;
    // newPayload[13] = 0;
    // newPayload[14] = 0;
    // newPayload[15] = totalPowerUsage;

    // 5. 你在上方塞入的數據會被自動縫合到MQTT的JSON裏面，依次序占用AI1-AI12, AO1-AO4，此處自動操作，你無需理會
  }

  // 不要動build()的代碼
  void build() {
    /*=== AI Payload ===*/
    AIPayload = F("[");
    for (size_t i = 0; i < 12; i++) {
      AIPayload += newPayload[i];
      if (i < 11) {
        AIPayload += F(",");
      }
    }
    AIPayload += F("]");

    /*=== AO Payload ===*/
    AOPayload = F("[");
    for (size_t i = 12; i < 16; i++) {
      AOPayload += newPayload[i];
      if (i < 15) {
        AOPayload += F(",");
      }
    }
    AOPayload += F("]");

    iot.buildMsg(DIPayload, DOPayload, AIPayload, AOPayload);
  }

  // void debug() {
  //   // 隨便寫，隨你喜歡debug什麽東西
  //   Serial.print(F("\r\n>>> Print Data\r\n"));

  //   for (uint8_t i = 0; i < mbNumOfData; i++) {
  //     Serial.print(i);
  //     Serial.print(F(": "));
  //     Serial.println(mbHrData[i]);
  //   }
  // }
public:
  CET(void) {}

  void init() {
    if (!mbRtuClient.begin(MB_BAUD_RATE, MB_SERIAL_CONFIG)) {
      Serial.println(F("Failed to start Modbus RTU Client!"));
      while (1) {};
    }

    for (uint8_t i = 0; i < 100; i++) {
      uint8_t numCnt = mbReadConfig[i][2];
      if (numCnt > 0) {
        mbNumOfConfig++;
        mbNumOfData += numCnt;
      }
    }
  }

  void loop() {
    if (mbTimer.autoTimeout(500)) {  // query and fill 1 batch of data every 500ms
      this->query();
      this->operate();
    }
    this->build();
    // this->debug();
  }
};

#endif
