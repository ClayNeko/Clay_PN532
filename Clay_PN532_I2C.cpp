#include "Clay_PN532_I2C.h"
#include "Clay_PN532_Interface.h"

#include <cstring>

esp_err_t 
Clay_PN532_I2C::init() {
	i2c_config_t i2ccfg = {
		.mode 			  = I2C_MODE_MASTER,
		.sda_io_num 	  = I2C_IO_SDA,
		.scl_io_num 	  = I2C_IO_SCL,
		.sda_pullup_en 	  = GPIO_PULLUP_ENABLE,
		.scl_pullup_en 	  = GPIO_PULLUP_ENABLE, 
		.master           = {
			.clk_speed = 100000,
		},
    	.clk_flags 		  = 0,
	};
/* 配置i2c参数，接收错误并返回 */
    i2c_param_config(I2C_NUM_0, &i2ccfg);
/* 安装驱动程序，并检查是否成功（err == ESP_OK） */
    err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if(err != ESP_OK){
    	return err;
    }
    return ESP_OK;
}

esp_err_t 
Clay_PN532_I2C::write(const uint8_t* data, size_t datasize) {
	printf("write.data:");
	for (size_t i = 0; i < datasize; i++) printf("%d ", data[i]);
	printf("\n");
	i2c_cmd_handle_t cmd_haldle = i2c_cmd_link_create();
//添加各种子数据帧
	i2c_master_start(cmd_haldle);					//起始信号
   	i2c_master_write_byte(cmd_haldle, PN532_SLAVE_ADDR, false);	//从机地址及读写位

	
   	i2c_master_write_byte(cmd_haldle, PN532_PREAMBLE, false); //0x00
   	i2c_master_write_byte(cmd_haldle, PN532_STARTCODE1, false); //0x00
   	i2c_master_write_byte(cmd_haldle, PN532_STARTCODE2, false); //0xFF

   	i2c_master_write_byte(cmd_haldle, datasize, false);
   	i2c_master_write_byte(cmd_haldle, ~datasize+1, false);
	
   	i2c_master_write_byte(cmd_haldle, PN532_HOSTTOPN532, false); //0xD4
	uint8_t sum = PN532_HOSTTOPN532;

	for (size_t i = 0; i < datasize; i++)
	{
   		i2c_master_write_byte(cmd_haldle, data[i], false);
		sum += data[i];
	}

	uint8_t checksum = ~sum + 1;
	i2c_master_write_byte(cmd_haldle, checksum, false);
	i2c_master_write_byte(cmd_haldle, PN532_POSTAMBLE, true); //0x00

   	i2c_master_stop(cmd_haldle);					//终止信号
//向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd_haldle, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd_haldle);
	return err;
}

uint8_t 
Clay_PN532_I2C::read() {
	return 0;
}

int 
Clay_PN532_I2C::wake() {
	const uint8_t PN532_WAKE[] = {0x00, 0x00};
    err = write(PN532_WAKE, sizeof(PN532_WAKE) / sizeof(PN532_WAKE[0]));
	printf("write.err:%d => %s\n", err, esp_err_to_name(err));
	return ESP_OK;
}