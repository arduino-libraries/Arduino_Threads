// The preprocessor should enforce that all variables belonging here are declared Shared
// There's no risk to spill other variables around since they are all private members of the automatic class

struct i2cScanResults {
    uint8_t address[128];
};

Shared<float> temperature;

Shared<struct i2cScanResults> scanResults;
Shared<uint8_t> pf1550_id;
Shared<uint32_t> randomNumber;
