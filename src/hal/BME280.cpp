#include "hal/BME280.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Logger.hpp"

namespace edge::hal {

BME280::BME280(const std::string& bus, int address)
    : i2c_bus_(bus), i2c_address_(address), file_descriptor_(-1), t_fine_(0) {
}

BME280::~BME280() {
    if (file_descriptor_ >= 0) close(file_descriptor_);
}

bool BME280::ReadCalibrationData() {
    // 1. Read Temp & Press + H1 (Registers 0x88 to 0xA1 -> 26 bytes)
    uint8_t reg1 = 0x88;
    write(file_descriptor_, &reg1, 1);
    uint8_t calib1[26];
    if (read(file_descriptor_, calib1, 26) != 26) return false;

    calib_data_.dig_T1 = (calib1[1] << 8) | calib1[0];
    calib_data_.dig_T2 = (calib1[3] << 8) | calib1[2];
    calib_data_.dig_T3 = (calib1[5] << 8) | calib1[4];

    calib_data_.dig_P1 = (calib1[7] << 8) | calib1[6];
    calib_data_.dig_P2 = (calib1[9] << 8) | calib1[8];
    calib_data_.dig_P3 = (calib1[11] << 8) | calib1[10];
    calib_data_.dig_P4 = (calib1[13] << 8) | calib1[12];
    calib_data_.dig_P5 = (calib1[15] << 8) | calib1[14];
    calib_data_.dig_P6 = (calib1[17] << 8) | calib1[16];
    calib_data_.dig_P7 = (calib1[19] << 8) | calib1[18];
    calib_data_.dig_P8 = (calib1[21] << 8) | calib1[20];
    calib_data_.dig_P9 = (calib1[23] << 8) | calib1[22];
    calib_data_.dig_H1 = calib1[25];

    // 2. Read remaining Humidity (Registers 0xE1 to 0xE7 -> 7 bytes)
    uint8_t reg2 = 0xE1;
    write(file_descriptor_, &reg2, 1);
    uint8_t calib2[7];
    if (read(file_descriptor_, calib2, 7) != 7) return false;

    calib_data_.dig_H2 = (calib2[1] << 8) | calib2[0];
    calib_data_.dig_H3 = calib2[2];
    calib_data_.dig_H4 = (calib2[3] << 4) | (calib2[4] & 0x0F);
    calib_data_.dig_H5 = (calib2[5] << 4) | (calib2[4] >> 4);
    calib_data_.dig_H6 = calib2[6];

    return true;
}

bool BME280::Init() {
    file_descriptor_ = open(i2c_bus_.c_str(), O_RDWR);
    if (file_descriptor_ < 0) return false;
    if (ioctl(file_descriptor_, I2C_SLAVE, i2c_address_) < 0) return false;

    // 1. Read factory calibration parameters
    if (!ReadCalibrationData()) {
        LOG_ERROR("[BME280] Failed to read calibration matrix.");
        return false;
    }

    // 2. Set oversampling and mode (Ctrl_Meas 0xF4: Temp x1, Press x1, Normal mode)
    if (!WriteRegister(0xF4, 0x27)) return false;

    LOG_INFO("[BME280] Real hardware initialized with factory calibration.");
    return true;
}

bool BME280::WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    return write(file_descriptor_, buffer, 2) == 2;
}

SensorData BME280::ReadData() {
    SensorData data = {0.0f, 0.0f, 0.0f};
    uint8_t reg = 0xF7;
    write(file_descriptor_, &reg, 1);

    uint8_t buffer[8];
    if (read(file_descriptor_, buffer, 8) != 8) return data;

    // Parse raw ADC values
    int32_t adc_P = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
    int32_t adc_T = (buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4);
    int32_t adc_H = (buffer[6] << 8) | buffer[7];

    // --- TEMPERATURE MATH ---
    int32_t var1, var2;
    var1 =
        ((((adc_T >> 3) - ((int32_t)calib_data_.dig_T1 << 1))) * ((int32_t)calib_data_.dig_T2)) >>
        11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib_data_.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)calib_data_.dig_T1))) >>
             12) *
            ((int32_t)calib_data_.dig_T3)) >>
           14;
    t_fine_ = var1 + var2;
    data.temperature = ((t_fine_ * 5 + 128) >> 8) / 100.0f;

    // --- PRESSURE MATH (64-bit precision) ---
    int64_t p_var1, p_var2, p;
    p_var1 = ((int64_t)t_fine_) - 128000;
    p_var2 = p_var1 * p_var1 * (int64_t)calib_data_.dig_P6;
    p_var2 = p_var2 + ((p_var1 * (int64_t)calib_data_.dig_P5) << 17);
    p_var2 = p_var2 + (((int64_t)calib_data_.dig_P4) << 35);
    p_var1 = ((p_var1 * p_var1 * (int64_t)calib_data_.dig_P3) >> 8) +
             ((p_var1 * (int64_t)calib_data_.dig_P2) << 12);
    p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)calib_data_.dig_P1) >> 33;

    if (p_var1 == 0) {
        data.pressure = 0;
    }  // Avoid division by zero
    else {
        p = 1048576 - adc_P;
        p = (((p << 31) - p_var2) * 3125) / p_var1;
        p_var1 = (((int64_t)calib_data_.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        p_var2 = (((int64_t)calib_data_.dig_P8) * p) >> 19;
        p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)calib_data_.dig_P7) << 4);
        data.pressure = (float)p / 25600.0f;  // Convert to hPa
    }

    // --- HUMIDITY MATH ---
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine_ - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)calib_data_.dig_H4) << 20) -
                    (((int32_t)calib_data_.dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >>
                  15) *
                 (((((((v_x1_u32r * ((int32_t)calib_data_.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)calib_data_.dig_H3)) >> 11) + ((int32_t)32768))) >>
                     10) +
                    ((int32_t)2097152)) *
                       ((int32_t)calib_data_.dig_H2) +
                   8192) >>
                  14));
    v_x1_u32r =
        (v_x1_u32r -
         (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)calib_data_.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    data.humidity = (float)(v_x1_u32r >> 12) / 1024.0f;

    return data;
}

}  // namespace edge::hal