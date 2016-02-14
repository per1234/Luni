#ifndef MCP9808LUI_h
#define MCP9808LUI_h

class MCP9808LUI: public LogicalUnitInfo {

public:

  MCP9808LUI(int addr) : LogicalUnitInfo(), i2cAddress(addr) {}

  ~MCP9808LUI() {}

  const int getI2CAddress() {
    return i2cAddress;
  }

private:
  int i2cAddress;
};

#endif
