#ifndef kt0803k_h
#define kt0803k_h

// 0x7C and 0x7D are write/read, but WiringPi implicitly assumes that the least
// significant bit is used to distinguish between reading and writing, so it
// takes the 7 most significant bits as the address instead. 0x7C >> 1 = 0x3E.
#define KT0803K_ADDRESS 0x3E

// == BEGIN REGISTERS ==
#define REG_CHSEL_8_1 0x00
#define BIT_START_CHSEL_8_1 0x00
#define BIT_END_CHSEL_8_1 0x07

#define REG_RFGAIN_1_0 0x01
#define BIT_START_RFGAIN_1_0 0x06
#define BIT_END_RFGAIN_1_0 0x07

#define REG_PGA_2_0 0x01
#define BIT_START_PGA_2_0 0x03
#define BIT_END_PGA_2_0 0x05

#define REG_CHSEL_11_9 0x01
#define BIT_START_CHSEL_11_9 0x00
#define BIT_END_CHSEL_11_9 0x02

#define REG_CHSEL_0 0x02
#define BIT_START_CHSEL_0 0x07
#define BIT_END_CHSEL_0 0x07

#define REG_RFGAIN_3 0x02
#define BIT_START_RFGAIN_3 0x06
#define BIT_END_RFGAIN_3 0x06

#define REG_MUTE 0x02
#define BIT_START_MUTE 0x03
#define BIT_END_MUTE 0x03

#define REG_PLTADJ 0x02
#define BIT_START_PLTADJ 0x02
#define BIT_END_PLTADJ 0x02

#define REG_PHTCNST 0x02
#define BIT_START_PHTCNST 0x00
#define BIT_END_PHTCNST 0x00

#define REG_MONO 0x04
#define BIT_START_MONO 0x06
#define BIT_END_MONO 0x06

#define REG_PGA_LSB_1_0 0x04
#define BIT_START_PGA_LSB_1_0 0x04
#define BIT_END_PGA_LSB_1_0 0x05

#define REG_FDEV_1_0 0x04
#define BIT_START_FDEV_1_0 0x02
#define BIT_END_FDEV_1_0 0x03

#define REG_BASS_1_0 0x04
#define BIT_START_BASS_1_0 0x00
#define BIT_END_BASS_1_0 0x01

#define REG_PDPA 0x0B
#define BIT_START_PDPA 0x05
#define BIT_END_PDPA 0x05

#define REG_PA_BIAS 0x0E
#define BIT_START_PA_BIAS 0x01
#define BIT_END_PA_BIAS 0x01

#define REG_PW_OK 0x0F
#define BIT_START_PW_OK 0x04
#define BIT_END_PW_OK 0x04

#define REG_SLNCID 0x0F
#define BIT_START_SLNCID 0x02
#define BIT_END_SLNCID 0x02

#define REG_LMTLVL_1_0 0x10
#define BIT_START_LMTLVL_1_0 0x03
#define BIT_END_LMTLVL_1_0 0x04

#define REG_PGAMOD 0x10
#define BIT_START_PGAMOD 0x10
#define BIT_END_PGAMOD 0x10

#define REG_SLNCDIS 0x12
#define BIT_START_SLNCDIS 0x07
#define BIT_END_SLNCDIS 0x07

#define REG_SLNCTHL_2_0 0x12
#define BIT_START_SLNCTHL_2_0 0x04
#define BIT_END_SLNCTHL_2_0 0x06

#define REG_SLNCTHH_2_0 0x12
#define BIT_START_SLNCTHH_2_0 0x01
#define BIT_END_SLNCTHH_2_0 0x03

#define REG_SWMOD 0x12
#define BIT_START_SW_MOD 0x00
#define BIT_END_SW_MOD 0x00

#define REG_RFGAIN_2 0x13
#define BIT_START_RFGAIN_2 0x07
#define BIT_END_RFGAIN_2 0x07

#define REG_PA_CTRL 0x13
#define BIT_START_PA_CTLR 0x02
#define BIT_END_PA_CTLR 0x02

#define REG_SLNCTIME_2_0 0x14
#define BIT_START_SLNCTIME_2_0 0x05
#define BIT_END_SLNCTIME_2_0 0x07

#define REG_SLNCCNTHIGH_2_0 0x14
#define BIT_START_SLNCCNTHIGH_2_0 0x02
#define BIT_END_SLNCCNTHIGH_2_0 0x04

#define REG_SLNCCNTLOW_2_0 0x16
#define BIT_START_SLNCCNTLOW_2_0 0x00
#define BIT_END_SLNCCNTLOW_2_0 0x02
// == END REGISTERS ==

uint8_t getBit(uint8_t bit, uint8_t word);

uint8_t setBit(uint8_t bit, uint8_t value, uint8_t word);

uint8_t getBitField(uint8_t bitStart, uint8_t bitEnd, uint8_t word);

uint8_t setBitField(uint8_t bitStart, uint8_t bitEnd, uint8_t value, uint8_t word);

void ktInit();

uint8_t ktReadRegister(uint8_t reg);

void ktWriteRegister(uint8_t reg, uint8_t word);

uint8_t ktReadRegisterField(uint8_t bitStart, uint8_t bitEnd, uint8_t reg);

void ktWriteRegisterField(uint8_t bitStart, uint8_t bitEnd, uint8_t reg, uint8_t value);

uint32_t ktGetChannel();

void ktSetChannel(uint32_t channel);

uint8_t ktGetRfGain();

void ktSetRfGain(uint8_t gain);

uint8_t ktGetMute();

void ktSetMute(uint8_t mute);

uint8_t ktGetMono();

void ktSetMono(uint8_t mono);

int8_t ktGetPGA();

void ktSetPGA(int8_t pga);

uint8_t ktGetPilotToneAmplitudeAdjustment();

void ktSetPilotToneAmplitudeAdjustment(uint8_t pltadj);

uint8_t ktGetLimitLevel();

void ktSetLimitLevel(uint8_t limit);

uint8_t ktGetFrequencyDeviation();

void ktSetFrequencyDeviation(uint8_t deviation);

uint8_t ktGetPaBias();

void ktSetPaBias(uint8_t bias);

/** 
 * Parse a boolean from a NULL-terminated string, storing the result in *out.
 * Returns EINVAL and prints an error message to stderr in the event of a parse
 * error, leaving *out unchaged.
 */
error_t ktParseBool(char *opt, char *arg, int *out);

/** 
 * Parse an integer from a NULL-terminated string, storing the result in *out.
 * Returns EINVAL and prints an error message to stderr in the event of a parse
 * error, leaving *out unchaged.
 */
error_t ktParseInt(char *opt, char *arg, long int min, long int max, long int *out);

error_t ktHandleArgQuery();

error_t ktHandleArgMute(char *arg);

error_t ktHandleArgMono(char *arg);

error_t ktHandleArgRfGain(char *arg);

error_t ktHandleArgSoftwareGain(char *arg);

error_t ktHandleArgPaBias(char *arg);

error_t ktHandleArgLimitLevel(char *arg);

error_t ktHandleArgFrequencyDeviation(char *arg);

error_t ktHandleArgChannel(char *arg);

error_t ktHandleArg(int key, char *arg, struct argp_state *state);

#endif
