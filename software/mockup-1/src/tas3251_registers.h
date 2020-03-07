#include <stdint.h>

/** i2c registers
 */

/** book0 page0 */
#define TAS3251_REG_PAGE 0x00
#define TAS3251_REG_RESET 0x01
#define TAS3251_REG_STANDBY 0x02
#define TAS3251_REG_MUTE 0x03
#define TAS3251_REG_PLL 0x04
#define TAS3251_REG_OSCILLATOR 0x05
#define TAS3251_REG_DEVICE_COMMUNICATION 0x06
#define TAS3251_REG_SDOUT 0x07
#define TAS3251_REG_GPIO 0x08
#define TAS3251_REG_BCLK 0x09
#define TAS3251_REG_DSP_GPIO_INPUT 0x0A
#define TAS3251_REG_CLOCK_CONFIGURATION 0x0C
#define TAS3251_REG_PLL_CLOCK_CONFIGURATION 0x0D
#define TAS3251_REG_PLLP 0x14
#define TAS3251_REG_PLL_J 0x15
#define TAS3251_REG_PLL_D_MSB 0x16
#define TAS3251_REG_PLL_D_LSB 0x17
#define TAS3251_REG_PLL_R 0x18
#define TAS3251_REG_DSP_CLOCK_DIVIDER 0x1B
#define TAS3251_REG_DAC_CLOCK_DIVIDER 0x1C
#define TAS3251_REG_NCP_CLOCK_DIVIDER 0x1D
#define TAS3251_REG_OSR_CLOCK_DIVIDER 0x1E
#define TAS3251_REG_MASTER_MODE_BCK_DIVIDER 0x20
#define TAS3251_REG_MASTER_MODE_LRCK_DIVIDER 0x21
#define TAS3251_REG_PIN_INTERRUPT 0x23
#define TAS3251_REG_CLOCK_DETECTION_CONFIG 0x25
#define TAS3251_REG_I2S_CONFIG 0x28
#define TAS3251_REG_I2S_SHIFT 0x29
#define TAS3251_REG_DAC_DATA_PATH 0x2A
#define TAS3251_REG_DSP_PROGRAM_SELECTION 0x2B
#define TAS3251_REG_CLOCK_MISSING_DETECTION 0x2C
#define TAS3251_REG_INTERRUPT_MASK 0x2D
#define TAS3251_REG_AUTO_MUTE 0x3B
#define TAS3251_REG_DIGITAL_VOLUME_CONTROL 0x3C
#define TAS3251_REG_LEFT_DIGITAL_VOLUME 0x3D
#define TAS3251_REG_RIGHT_DIGITAL_VOLUME 0x3E
#define TAS3251_REG_DIGITAL_VOLUME_RAMP_CONFIG 0x3F
#define TAS3251_REG_DIGITAL_VOLUME_EMERGENCY_RAMP_CONFIG 0x40
#define TAS3251_REG_AUTO_MUTE_CONTROL 0x41
#define TAS3251_REG_GPIO0_OUTPUT 0x53
#define TAS3251_REG_GPIO2_OUTPUT 0x55
#define TAS3251_REG_GPIO_OUTPUT_CONTROL 0x56
#define TAS3251_REG_GPIO_OUTPUT_INVERSION 0x57
#define TAS3251_REG_DETECTED_CLOCK_CONFIG 0x5B
#define TAS3251_REG_DETECTED_BCK_RATIO 0x5D
#define TAS3251_REG_CLOCK_DETECTOR 0x5E
#define TAS3251_REG_GPIO_INPUT_STATE 0x77
#define TAS3251_REG_AUTO_MUTE_FLAG 0x78
#define TAS3251_REG_DAC_MODE 0x79
#define TAS3251_REG_BOOK 0x7f

/** book0 page1 */

/** structures
 */

/** book0 page0 */

typedef struct {
    uint8_t reset_registers : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t reset_modules : 1;
    uint8_t reserved_7_5 : 3;
} tas3251_reg_reset_t;

typedef struct {
    uint8_t powerdown_request : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t standby_request : 1;
    uint8_t reserved_6_5 : 2;
    uint8_t dsp_reset : 1;
} tas3251_reg_standby_t;
#define TAS3251_STANDBY_ENABLE 0x00
#define TAS3251_STANDBY_POWER_DOWN 0x01
#define TAS3251_STANDBY_STANDBY 0x10
#define TAS3251_STANDBY_DSPR 0x80

typedef struct {
    uint8_t mute_right_channel : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t mute_left_channel : 1;
    uint8_t reserved_7_5 : 3;
} tas3251_reg_mute_t;
#define TAS3251_MUTE_RIGHT 0x01
#define TAS3251_MUTE_LEFT 0x10
#define TAS3251_MUTE_UNMUTE_ALL 0x0

typedef struct {
    uint8_t pll_enable : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t pll_lock_flag : 1;
    uint8_t reserved_7_5 : 2;
} tas3251_reg_pll_t;

typedef struct {
    uint8_t reserved_7_0 : 8; /* 0x01 */
} tas3251_reg_oscillator_t;

typedef struct {
    uint8_t spi_miso_function_sel : 1;
    uint8_t spi_register_read_frame_delay : 1;
    uint8_t page_auto_increment_disable : 1;
    uint8_t reserved_7_4 : 4;
} tas3251_reg_device_communication_t;

typedef struct {
    uint8_t sdout_select : 1;
    uint8_t reserved_3_1 : 1;
    uint8_t de_emphasis_enable : 1;
    uint8_t reserved_7_5 : 3;
} tas3251_reg_sdout_t;

typedef struct {
    uint8_t reserved_2_0 : 3;
    uint8_t gpio0_output_enable : 1;
    uint8_t mute_control_enable : 1;
    uint8_t gpio2_output_enable : 1;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_gpio_t;

typedef struct {
    uint8_t lrclk_output_enable : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t bck_output_enable : 1;
    uint8_t bck_polarity : 1;
    uint8_t reserved_7_6 : 3;
} tas3251_reg_bclk_t;

typedef struct {
    uint8_t dsp_gpio_input : 8;
} tas3251_reg_dsp_gpio_input_t;

typedef struct {
    uint8_t master_mode_lrck_devider_reset : 1;
    uint8_t master_mode_bck_devider_reset : 1;
    uint8_t reserved_7_2 : 6;
} tas3251_reg_clock_configuration_t;

typedef struct {
    uint8_t reserved_3_0 : 4;
    uint8_t pll_reference : 1;
    uint8_t reserved_7_5 : 3;
} tas3251_reg_pll_clock_configuration_t;

typedef struct {
    uint8_t pllp : 1; /* 0x01 */
    uint8_t reserved_7_4 : 4;
} tas3251_reg_pllp_t;

typedef struct {
    uint8_t pll_j : 6; /* 0x08 */
    uint8_t reserved_7_6 : 2;
} tas3251_reg_pll_j_t;

typedef struct {
    uint8_t pddv : 6;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_pll_d_msb_t;

typedef struct {
    uint8_t pddv : 8;
} tas3251_reg_pll_d_lsb_t;

typedef struct {
    uint8_t prdv : 4;
    uint8_t reserved_7_4 : 4;
} tas3251_reg_pll_r_t;

typedef struct {
    uint8_t ddsp : 7;
    uint8_t reserved_7 : 1;
} tas3251_reg_dsp_clock_divider_t;

typedef struct {
    uint8_t ddac : 7; /* 0x0f */
    uint8_t reserved_7 : 1;
} tas3251_reg_dac_clock_divider_t;

typedef struct {
    uint8_t dncp : 7; /* 0x03 */
    uint8_t reserved_7 : 1;
} tas3251_reg_ncp_clock_divider_t;

typedef struct {
    uint8_t dosr : 7; /* 0x0f */
    uint8_t reserved_7 : 1;
} tas3251_reg_osr_clock_divider_t;

typedef struct {
    uint8_t dbck : 7;
    uint8_t reserved_7 : 1;
} tas3251_reg_master_mode_bck_divider_t;

typedef struct {
    uint8_t dlrck : 7;
    uint8_t reserved_7 : 1;
} tas3251_reg_master_mode_lrck_divider_t;

typedef struct {
    uint8_t pin_interrupt_sticky_flag : 1;
    uint8_t reserved_7_1 : 7;
} tas3251_reg_pin_interrupt_t;

typedef struct {
    uint8_t ignore_pll_lock_detection : 1;
    uint8_t disable_clock_divider_autoset : 1;
    uint8_t ignore_lrck_bck_missing_detection : 1;
    uint8_t ignore_clock_halt_detection : 1;
    uint8_t ignore_sck_halt_detection : 1;
    uint8_t ignore_bck_halt_detection : 1;
    uint8_t ignore_fs_detection : 1;
    uint8_t _32_ksps_mode : 1;
} tas3251_reg_clock_detection_config_t;

typedef struct {
    uint8_t i2s_word_length : 2; /* 0x02 */
    uint8_t reserved_3_2 : 2;
    uint8_t i2s_data_format : 2;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_i2s_config_t;

typedef struct {
    uint8_t i2s_shift : 8;
} tas3251_reg_i2s_shift_t;

typedef struct {
    uint8_t right_dac_data_path : 2; /* 0x01 */
    uint8_t reserved_3_2 : 2;
    uint8_t left_dac_data_path : 2; /* 0x01 */
    uint8_t reserved_7_6 : 2;
} tas3251_reg_dac_data_path_t;

typedef struct {
    uint8_t dsp_program_selection : 5; /* 0x01 */
    uint8_t reserved_7_5 : 3;
} tas3251_reg_dsp_program_selection_t;

typedef struct {
    uint8_t clock_missing_detection_period : 3;
    uint8_t reserved_7_3 : 5; /* 0x01 */
} tas3251_reg_clock_missing_detection_t;

typedef struct {
    uint8_t interrupt_mask : 8;
} tas3251_reg_interrupt_mask_t;

typedef struct {
    uint8_t auto_mute_time_for_right_channel : 3;
    uint8_t reserved_3 : 1;
    uint8_t auto_mute_time_for_left_channel : 3;
    uint8_t reserved_7 : 1;
} tas3251_reg_auto_mute_t;

typedef struct {
    uint8_t digital_volume_control : 2;
    uint8_t reserved_7_2 : 6;
} tas3251_reg_digital_volume_control_t;

typedef struct {
    uint8_t left_digital_volume : 8; /* 0x30 */
} tas3251_reg_left_digital_volume_t;

typedef struct {
    uint8_t right_digital_volume : 8; /* 0x30 */
} tas3251_reg_right_digital_volume_t;

typedef struct {
    uint8_t digital_volume_normal_ramp_up_step : 2; /* 0x03 */
    uint8_t digital_volume_normal_ramp_up_frequency : 2;
    uint8_t digital_volume_normal_ramp_down_step : 2; /* 0x03 */
    uint8_t digital_volume_normal_ramp_down_frequency : 2;
} tas3251_reg_digital_volume_ramp_config_t;

typedef struct {
    uint8_t reserved_3_0 : 4;
    uint8_t digital_volume_emergency_ramp_down_step : 2; /* 0x03 */
    uint8_t digital_volume_emergency_ramp_down_frequency : 2;
} tas3251_reg_digital_volume_emergency_ramp_config_t;

typedef struct {
    uint8_t auto_mute_right_channel : 1;
    uint8_t auto_mute_left_channel : 1;
    uint8_t auto_mute_control : 1;
    uint8_t reserved_7_3 : 5;
} tas3251_reg_auto_mute_control_t;

typedef struct {
    uint8_t gpio0_output : 4;
    uint8_t reserved_7_4 : 4;
} tas3251_reg_gpio0_output_t;

typedef struct {
    uint8_t gpio2_output : 4;
    uint8_t reserved_7_4 : 4;
} tas3251_reg_gpio2_output_t;

typedef struct {
    uint8_t reserved_2_0 : 3;
    uint8_t gpio0_output_control : 1;
    uint8_t mute_output_control : 1;
    uint8_t gpio2_output_control : 1;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_gpio_output_control_t;

typedef struct {
    uint8_t reserved_2_0 : 3;
    uint8_t gpio0_output_inversion : 1;
    uint8_t mute_output_inversion : 1;
    uint8_t gpio2_output_inversion : 1;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_gpio_output_inversion_t;

typedef struct {
    uint8_t detected_sck_ratio : 4;
    uint8_t detected_fs_ratio : 3;
    uint8_t reserved_7 : 1;
} tas3251_reg_detected_clock_config_t;

typedef struct {
    uint8_t detected_bck_ratio : 8;
} tas3251_reg_detected_bck_ratio_t;

typedef struct {
    uint8_t sample_rate_flag : 1;
    uint8_t bck_valid_flag : 1;
    uint8_t sclk_valid_flag : 1;
    uint8_t clock_auto_set_flag : 1;
    uint8_t lrck_and_brck_flag : 1;
    uint8_t pll_lock_flag : 1;
    uint8_t detect_sck : 1;
    uint8_t reserved_7 : 1;
} tas3251_reg_clock_detector_t;

typedef struct {
    uint8_t reserved_2_0 : 3;
    uint8_t gpio0_pin_flag : 1;
    uint8_t reserved_4 : 1;
    uint8_t gpio2_pin_flag : 1;
    uint8_t reserved_7_6 : 2;
} tas3251_reg_gpio_input_state_t;

typedef struct {
    uint8_t auto_mute_flag_for_right_channel : 1;
    uint8_t reserved_3_1 : 3;
    uint8_t auto_mute_flag_for_left_channel : 1;
    uint8_t reserved_7_5 : 3;
} tas3251_reg_auto_mute_flag_t;

typedef struct {
    uint8_t dac_mode : 1;
    uint8_t reserved_7_1 : 7;
} tas3251_reg_dac_mode_t;

/** book0 page1 */
