#ifndef MCP9808LUI_h
#define MCP9808LUI_h

class MCP9808LUI: public LogicalUnitInfo {

friend class MCP9808Driver;

public:

  MCP9808LUI(int addr) : LogicalUnitInfo(), i2cAddress(addr) {}

  ~MCP9808LUI() {}

private:
  int i2cAddress;
};

#endif
