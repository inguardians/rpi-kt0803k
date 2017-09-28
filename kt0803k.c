#include <argp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPiI2C.h>
#include "kt0803k.h"

int ktWiringPiFd;

uint8_t getBit(uint8_t bit, uint8_t word) {
    return (word & (1 << bit)) >> bit;
}

uint8_t setBit(uint8_t bit, uint8_t value, uint8_t word) {
    return (word ^ (getBit(bit, word) << bit)) | ((value == 0 ? 0 : 1) << bit);
}

uint8_t getBitField(uint8_t bitStart, uint8_t bitEnd, uint8_t word) {
    uint8_t out = 0;
    int inBit = bitEnd;
    while (inBit >= bitStart) {
        out = (out << 1) | getBit(inBit, word);
        inBit--;
    }
    return out;
}

uint8_t setBitField(uint8_t bitStart, uint8_t bitEnd, uint8_t word, uint8_t value) {
    uint8_t out = word;
    int inBit = 0;
    int outBit = bitStart;
    while (outBit <= bitEnd) {
        out = setBit(outBit, getBit(inBit, value), out);
        inBit++;
        outBit++;
    }
    return out;
}

void ktInit() {
    ktWiringPiFd = wiringPiI2CSetup(KT0803K_ADDRESS);
    if (ktWiringPiFd == -1 || ktWiringPiFd == -1) {
        fprintf(stderr, "Error initializing WiringPi\n");
        exit(1);
    }
}

uint8_t ktReadRegister(uint8_t reg) {
    return (uint8_t) wiringPiI2CReadReg8(ktWiringPiFd, reg);
}

void ktWriteRegister(uint8_t reg, uint8_t word) {
    wiringPiI2CWriteReg8(ktWiringPiFd, reg, (int) word);
}


uint8_t ktReadRegisterField(uint8_t bitStart, uint8_t bitEnd, uint8_t reg) {
    return getBitField(bitStart, bitEnd, ktReadRegister(reg));
}

void ktWriteRegisterField(uint8_t bitStart, uint8_t bitEnd, uint8_t reg, uint8_t value) {
    uint8_t in = ktReadRegister(reg);
    uint8_t ot = setBitField(bitStart, bitEnd, in, value);
    if (in != ot) {
        ktWriteRegister(reg, ot);
    }
}

uint32_t ktGetChannel() {
    uint32_t b0 = ktReadRegisterField(BIT_START_CHSEL_0, BIT_END_CHSEL_0, REG_CHSEL_0);
    uint32_t b8_1 = ktReadRegisterField(BIT_START_CHSEL_8_1, BIT_END_CHSEL_8_1, REG_CHSEL_8_1);
    uint32_t b11_9 = ktReadRegisterField(BIT_START_CHSEL_11_9, BIT_END_CHSEL_11_9, REG_CHSEL_11_9);
    // Multiply by 50 because the kt0803k takes the channel as a 12 bit value, in
    // increments of 50 KHz.
    return (b0 | (b8_1 << 1) | (b11_9 << 9)) * 50;
}

void ktSetChannel(uint32_t channel) {
    // Divide by 50 because the kt0803k takes the channel as a 12 bit value, in
    // increments of 50 KHz.
    uint32_t ktChannel = channel / 50;
    uint32_t b0 = ktChannel & 0x1;
    uint32_t b8_1 = (ktChannel >> 1) & 0xFF;
    uint32_t b11_9 = (ktChannel >> 9) & 0x07;
    ktWriteRegisterField(BIT_START_CHSEL_0, BIT_END_CHSEL_0, REG_CHSEL_0, (uint8_t) b0);
    ktWriteRegisterField(BIT_START_CHSEL_8_1, BIT_END_CHSEL_8_1, REG_CHSEL_8_1, (uint8_t) b8_1);
    ktWriteRegisterField(BIT_START_CHSEL_11_9, BIT_END_CHSEL_11_9, REG_CHSEL_11_9, (uint8_t) b11_9);
}

uint8_t ktGetRfGain() {
    uint8_t b1_0 = ktReadRegisterField(BIT_START_RFGAIN_1_0, BIT_END_RFGAIN_1_0, REG_RFGAIN_1_0);
    uint8_t b2 = ktReadRegisterField(BIT_START_RFGAIN_2, BIT_END_RFGAIN_2, REG_RFGAIN_2);
    uint8_t b3 = ktReadRegisterField(BIT_START_RFGAIN_3, BIT_END_RFGAIN_3, REG_RFGAIN_3);
    return b1_0 | (b2 << 2) | (b3 << 3);
}

void ktSetRfGain(uint8_t gain) {
    uint8_t b1_0 = gain & 0x3;
    uint8_t b2 = (gain >> 2) & 0x1;
    uint8_t b3 = (gain >> 3) & 0x1;
    ktWriteRegisterField(BIT_START_RFGAIN_1_0, BIT_END_RFGAIN_1_0, REG_RFGAIN_1_0, b1_0);
    ktWriteRegisterField(BIT_START_RFGAIN_2, BIT_END_RFGAIN_2, REG_RFGAIN_2, b2);
    ktWriteRegisterField(BIT_START_RFGAIN_3, BIT_END_RFGAIN_3, REG_RFGAIN_3, b3);
}

uint8_t ktGetMute() {
    return ktReadRegisterField(BIT_START_MUTE, BIT_END_MUTE, REG_MUTE);
}

void ktSetMute(uint8_t mute) {
    ktWriteRegisterField(BIT_START_MUTE, BIT_END_MUTE, REG_MUTE, mute);
}

uint8_t ktGetMono() {
    return ktReadRegisterField(BIT_START_MONO, BIT_END_MONO, REG_MONO);
}

void ktSetMono(uint8_t mono) {
    ktWriteRegisterField(BIT_START_MONO, BIT_END_MONO, REG_MONO, mono);
}


int8_t ktGetPGA() {
    uint8_t hi = ktReadRegisterField(BIT_START_PGA_2_0, BIT_END_PGA_2_0, REG_PGA_2_0);
    uint8_t lo = ktReadRegisterField(BIT_START_PGA_LSB_1_0, BIT_END_PGA_LSB_1_0, REG_PGA_LSB_1_0);
    uint8_t x = (hi << 2) | lo;

    // pga is a signed 5 bit number, but if it's positive, we need to subtract
    // 3 and max with 0. Unlike most signed numbers, a set sign bit indicates a
    // positive value, rather than a negative one.
    if (getBit(4, x)) {
        int8_t out = ((int8_t) (x & 0xF)) - 3;
        if (out < 0) {
            out = 0;
        }
        return out;
    } else {
        return  ((int8_t) (x & 0xF)) * -1;
    }
}

void ktSetPGA(int8_t pga) {
    uint8_t x;

    // pga is a signed 5 bit number, but if it's positive, we need to add
    // 3. Unlike most signed numbers, a set sign bit indicates a positive
    // value, rather than a negative one.
    if (pga <= 0) {
        x = (uint8_t) ((pga * -1) & 0xF);
    } else {
        x = (uint8_t) (((pga + 3) & 0xF) | 0x10);
    }
    uint8_t hi = (x >> 2) & 0x7;
    uint8_t lo = x & 0x3;
    ktWriteRegisterField(BIT_START_PGA_2_0, BIT_END_PGA_2_0, REG_PGA_2_0, hi);
    ktWriteRegisterField(BIT_START_PGA_LSB_1_0, BIT_END_PGA_LSB_1_0, REG_PGA_LSB_1_0, lo);
}

uint8_t ktGetPilotToneAmplitudeAdjustment() {
    return ktReadRegisterField(BIT_START_PLTADJ, BIT_END_PLTADJ, REG_PLTADJ);
}

void ktSetPilotToneAmplitudeAdjustment(uint8_t pltadj) {
    ktWriteRegisterField(BIT_START_PLTADJ, BIT_END_PLTADJ, REG_PLTADJ, pltadj);
}

uint8_t ktGetLimitLevel() {
    return ktReadRegisterField(BIT_START_LMTLVL_1_0, BIT_END_LMTLVL_1_0, REG_LMTLVL_1_0);
}

void ktSetLimitLevel(uint8_t limit) {
    ktWriteRegisterField(BIT_START_LMTLVL_1_0, BIT_END_LMTLVL_1_0, REG_LMTLVL_1_0, limit);
}

uint8_t ktGetFrequencyDeviation() {
    return ktReadRegisterField(BIT_START_FDEV_1_0, BIT_END_FDEV_1_0, REG_FDEV_1_0);
}

void ktSetFrequencyDeviation(uint8_t deviation) {
    ktWriteRegisterField(BIT_START_FDEV_1_0, BIT_END_FDEV_1_0, REG_FDEV_1_0, deviation);
}

uint8_t ktGetPaBias() {
    return ktReadRegisterField(BIT_START_PA_BIAS, BIT_END_PA_BIAS, REG_PA_BIAS);
}

void ktSetPaBias(uint8_t bias) {
    ktWriteRegisterField(BIT_START_PA_BIAS, BIT_END_PA_BIAS, REG_PA_BIAS, bias);
}

error_t ktParseBool(char *opt, char *arg, int *out) {
    if (strncasecmp("true", arg, 4) == 0) {
        *out = 1;
        return 0;
    } else if (strncasecmp("false", arg, 5) == 0) {
        *out = 0;
        return 0;
    } else {
        fprintf(stderr, "Error parsing boolean for option '%s'. Expected 'true' or 'false', got '%s'\n", opt, arg);
        return EINVAL;
    }
}

error_t ktParseInt(char *opt, char *arg, long int min, long int max, long int *out) {
    char *parseEnd = NULL;
    long int x = strtol(arg, &parseEnd, 10);
    if (*arg != '\0' && parseEnd != NULL && *parseEnd == '\0' && x >= min && x <= max) {
        *out = x;
        return 0;
    } else {
        fprintf(stderr, "Error parsing integer for option '%s'. Expected valid integer from %ld to %ld, got '%s'\n", opt, min, max, arg);
        return EINVAL;
    }
}

error_t ktHandleArgQuery() {
    printf("Mute                 %s\n", ktGetMute() == 0 ? "false" : "true");
    printf("Mono                 %s\n", ktGetMono() == 0 ? "false" : "true");
    printf("RF Gain              %d\n", ktGetRfGain());
    printf("Software Gain        %d\n", ktGetPGA());
    printf("PA Bias              %s\n", ktGetPaBias() == 0 ? "false" : "true");
    printf("Limit Level          %d\n", ktGetLimitLevel());
    printf("FDev                 %d\n", ktGetFrequencyDeviation());
    printf("Channel              %d\n", ktGetChannel());
    return 0;
}

error_t ktHandleArgMute(char *arg) {
    int mute;
    error_t err = ktParseBool("mute", arg, &mute);
    if (err != 0) {
        return err;
    }
    ktSetMute((uint8_t) mute);
    return 0;
}

error_t ktHandleArgMono(char *arg) {
    int mono;
    error_t err = ktParseBool("mono", arg, &mono);
    if (err != 0) {
        return err;
    }
    ktSetMono((uint8_t) mono);
    return 0;
}

error_t ktHandleArgRfGain(char *arg) {
    long int rfgain;
    error_t err = ktParseInt("rfgain", arg, 0, 15, &rfgain);
    if (err != 0) {
        return err;
    }
    ktSetRfGain((uint8_t) rfgain);
    return 0;
}

error_t ktHandleArgSoftwareGain(char *arg) {
    long int softwareGain;
    error_t err = ktParseInt("softwaregain", arg, -15, 12, &softwareGain);
    if (err != 0) {
        return err;
    }
    ktSetPGA((int8_t) softwareGain);
    return 0;
}

error_t ktHandleArgPaBias(char *arg) {
    int pabias;
    error_t err = ktParseBool("pabias", arg, &pabias);
    if (err != 0) {
        return err;
    }
    ktSetPaBias((uint8_t) pabias);
    return 0;
}

error_t ktHandleArgLimitLevel(char *arg) {
    long int limitLevel;
    error_t err = ktParseInt("limitlevel", arg, 0, 3, &limitLevel);
    if (err != 0) {
        return err;
    }
    ktSetLimitLevel((uint8_t) limitLevel);
    return 0;
}

error_t ktHandleArgFrequencyDeviation(char *arg) {
    long int fdev;
    error_t err = ktParseInt("fdev", arg, 0, 3, &fdev);
    if (err != 0) {
        return err;
    }
    ktSetFrequencyDeviation((uint8_t) fdev);
    return 0;
}

error_t ktHandleArgChannel(char *arg) {
    long int channel;
    // 204750 is the maximum unsigned value that can be represented in 12 bits.
    error_t err = ktParseInt("channel", arg, 0, 204750, &channel);
    if (err != 0) {
        return err;
    }
    ktSetChannel((uint32_t) channel);
    return 0;
}

error_t ktHandleArg(int key, char *arg, struct argp_state *state) {
    switch (key) {
        case 'q':
            return ktHandleArgQuery();
        case 'x':
            return ktHandleArgMute(arg);
        case 'm':
            return ktHandleArgMono(arg);
        case 'g':
            return ktHandleArgRfGain(arg);
        case 0x01:
            return ktHandleArgSoftwareGain(arg);
        case 0x02:
            return ktHandleArgPaBias(arg);
        case 'l':
            return ktHandleArgLimitLevel(arg);
        case 'f':
            return ktHandleArgFrequencyDeviation(arg);
        case 'c':
            return ktHandleArgChannel(arg);
        default:
            return ARGP_ERR_UNKNOWN;
    }
}

int main(int argc, char **argv) {
    ktInit();
    const struct argp_option argOptions[] = {
        { .name = "query", .key = 'q', .doc = "Query FM radio state." },
        { .name = "mute", .key = 'x', .arg = "true|false", .doc = "Set mute flag value. Unset by default." },
        { .name = "mono", .key = 'm', .arg = "true|false", .doc = "Set mono flag value. Unset by default." },
        { .name = "rfgain", .key = 'g', .arg = "0-15", .doc = "Set radio RF gain. In other words, set the signal strength. Defaults to max RF gain." },
        { .name = "softwaregain", .key = 0x01, .arg = "-15-12", .doc = "Set radio internal software gain, in decibels. Defaults to 0. This gain is applied to the incoming audio signal before transmission. Note that this can cause clipping issues if the input signal is already at maximum loudness, so I generally recommend just leaving this at 0." },
        { .name = "pabias", .key = 0x02, .arg = "true|false", .doc = "Set PA Bias flag. When this flag is set, signal strength is increased. This flag is set by default." },
        { .name = "limitlevel", .key = 'l', .arg = "0-3", .doc = "Set internal audio limiter threshold. 0-3 are valid values corresponding to thresholds 0.6875, 0.75, 0.875, and 0.9625, with 0.75 as the default." },
        { .name = "fdev", .key = 'f', .arg = "0-3", .doc = "Set frequency deviation. 0-3 are valid values corresponding to deviations of 75KHz, 112.5KHz, 150KHz, and 187.5Khz, with 112.5KHz as the default." },
        { .name = "channel", .key = 'c', .arg = "0-204750", .doc = "Set channel frequency in KHz. Frequency will be rounded down to the nearest 50KHz. Valid frequency range is unknown, but anything within the standard FM radio broadcast spectrum should work." },
        { 0 }
    };
    const struct argp argParser = {
        .parser = ktHandleArg,
        .options = argOptions,
        .doc = "kt0803k - Configure the KT0803 and KT0803K FM radios over i2c on the Raspberry Pi."
    };
    error_t parseError = argp_parse(&argParser, argc, argv, 0, NULL, NULL);
    if (parseError != 0) {
        if (parseError != EINVAL) {
            fprintf(stderr, "Error: %s\n", strerror(parseError));
        }
        exit(1);
    }
    return 0;
}
