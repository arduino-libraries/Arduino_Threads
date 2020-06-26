// The preprocessor should enforce that all variables belonging here are declared Shared
// There's no risk to spill other variables around since they are all private memeber of the automatic class

struct i2cScanResults {
    uint8_t address[128];
};

Shared<float> temperature;

Shared<struct i2cScanResults> scanResults;
