
// Turns on the L3G's gyro and places it in normal mode.
template <typename BusType>
void L3G<BusType>::EnableAll()
{
  // 0x0F = 0b00001111
  // Normal power mode, all axes enabled
   char Reg1 = this->readReg(L3G_CTRL_REG1);
   this->writeReg(L3G_CTRL_REG1, Reg1 | 0x0F);
}

template <typename BusType>
void
L3G<BusType>::hard_reset()
{
   if (device == L3GD20H_DEVICE)
   {
       this->writeReg(L3G_LOW_ODR, 0x04);
   }
}

template <typename BusType>
void
L3G_base<BusType>::soft_reset()
{
    if (device == L3GD20_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG1, 0x03);
    }
    if (device == L3GD20H_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG1, 0x07);
        this->writeReg(L3G_LOW_ODR, 0x00);
    }
    if (device == L3GD20_DEVICE || device == L3GD20H_DEVICE)
    {
        this->writeReg(L3G_CTRL_REG2, 0x00);
        this->writeReg(L3G_CTRL_REG3, 0x00);
        this->writeReg(L3G_CTRL_REG4, 0x00);
        this->writeReg(L3G_CTRL_REG5, 0x00);
        this->writeReg(L3G_REFERENCE, 0x00);
        this->writeReg(L3G_FIFO_CTRL_REG, 0x00);
        this->writeReg(L3G_INT1_CFG, 0x00);
        this->writeReg(L3G_INT1_THS_XH, 0x00);
        this->writeReg(L3G_INT1_THS_XL, 0x00);
        this->writeReg(L3G_INT1_THS_YH, 0x00);
        this->writeReg(L3G_INT1_THS_YL, 0x00);
        this->writeReg(L3G_INT1_THS_ZH, 0x00);
        this->writeReg(L3G_INT1_THS_ZL, 0x00);
        this->writeReg(L3G_INT1_DURATION, 0x00);
    }
}

template <typename BusType>
void L3G<BusType>::SetScale(int dps)
{
   char FS = 0;
   switch (dps)
   {
       case 250  : FS = 0; break;
       case 500  : FS = 1; break;
       case 2000 : FS = 3; break;
       default : FS = 0;
   }
   char Reg4 = this->readReg(L3G_CTRL_REG4);
   this->writeReg(L3G_CTRL_REG4, (Reg4 & 0xCF) | (FS << 4));
}

template <typename BusType>
void L3G<BusType>::SetBlockDataUpdate(bool Enable)
{
   char Reg4 = this->readReg(L3G_CTRL_REG4);
   this->writeReg(L3G_CTRL_REG4, (Reg4 & 0x7F) | (char(Enable) << 7));
}

template <typename BusType>
void L3G<BusType>::SetRateBandwidth(unsigned Setting)
{
   char Reg1 = this->readReg(L3G_CTRL_REG1);
   // no need to shift the Setting to the left, the defined constants already use the correct bits
   this->writeReg(L3G_CTRL_REG1, (Reg1 & 0x0F) | (char(Setting & 0xF0)));
   if (device == L3GD20H_DEVICE)
   {
       char Reg2 = this->readReg(L3G_LOW_ODR);
       this->writeReg(L3G_LOW_ODR, (Reg2 & 0xFE) | ((Setting >> 8) & 0x01));
    }
}

template <typename BusType>
void L3G<BusType>::SetInt2DataReady(bool Enable)
{
   char Reg3 = this->readReg(L3G_CTRL_REG3);
   this->writeReg(L3G_CTRL_REG3, (Reg3 & 0xF7) | (char(Enable) << 3));
}

template <typename BusType>
void L3G<BusType>::SetInt2FTH(bool Enable)
{
   char Reg3 = this->readReg(L3G_CTRL_REG3);
   this->writeReg(L3G_CTRL_REG3, (Reg3 & 0xF7) | (char(Enable) << 2));
}

template <typename BusType>
void L3G<BusType>::SetFIFOMode(FIFO_Modes Mode)
{
   char Reg = this->readReg(L3G_FIFO_CTRL_REG);
   this->writeReg(L3G_FIFO_CTRL_REG, (Reg & 0x1F) | ((char(Mode) & 0x03) << 5));
}

template <typename BusType>
void L3G<BusType>::SetFIFOThreshold(int Threshold)
{
   char Reg = this->readReg(L3G_FIFO_CTRL_REG);
   this->writeReg(L3G_FIFO_CTRL_REG, (Reg & 0x1F) | (char(Threshold & 0x1F)));
}

template <typename BusType>
void L3G<BusType>::EnableFIFO(bool Enable)
{
   char Reg5 = this->readReg(L3G_CTRL_REG5);
   this->writeReg(L3G_CTRL_REG5, (Reg5 & 0xBF) | (char(Enable) << 6));
}

template <typename BusType>
L3G<BusType>::FIFOStatus L3G<BusType>::GetFIFOStatus()
{
   return FIFOStatus(char(this->readReg(L3G_FIFO_SRC_REG)));
}

template <typename BusType>
int
L3G<BusType>::TempRaw()
{
   return this->readReg(L3G_OUT_TEMP);
}

template <typename BusType>
int
L3G<BusType>::Temp()
{
   return 50 - this->TempRaw();
}

template <typename BusType>
short int
L3G<BusType>::ReadX()
{
   return this->read16(L3G_OUT_X_L);
}

template <typename BusType>
short int
L3G<BusType>::ReadY()
{
   return this->read16(L3G_OUT_Y_L);
}

template <typename BusType>
short int
L3G<BusType>::ReadZ()
{
   return this->read16(L3G_OUT_Z_L);
}
