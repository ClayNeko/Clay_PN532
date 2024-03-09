#include "Clay_PN532_I2C.h"

#include <cstring>

esp_err_t Clay_PN532_I2C::init() {
	i2c_config_t i2ccfg = {
		.mode 			  = I2C_MODE_MASTER,
		.sda_io_num 	  = I2C_IO_SDA,
		.scl_io_num 	  = I2C_IO_SCL,
		.sda_pullup_en 	  = GPIO_PULLUP_ENABLE,
		.scl_pullup_en 	  = GPIO_PULLUP_ENABLE, 
		.master           = {
			.clk_speed = 400000,
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
	cmd_haldle = i2c_cmd_link_create();
    return ESP_OK;
//删除i2c_cmd_handle_t对象，释放资源
    //i2c_cmd_link_delete(cmd);
}

esp_err_t Clay_PN532_I2C::write(const uint8_t* data, size_t datasize) {
	printf("write.data:");
	for (size_t i = 0; i < 11; i++)
	{
		printf("%d ", data[i]);
	}
	printf("\nsizeof data:%d\n", datasize);
//添加各种子数据帧
	i2c_master_start(cmd_haldle);					//起始信号
   	i2c_master_write_byte(cmd_haldle, (PN532_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true);	//从机地址及读写位
   	i2c_master_write(cmd_haldle, data, datasize, true);	//数据位(数组)
   	i2c_master_stop(cmd_haldle);					//终止信号
//向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd_haldle, 1000 / portTICK_PERIOD_MS);
	return err;
}

uint8_t* Clay_PN532_I2C::read() {
	uint8_t* buf = new uint8_t[256]{0};
//添加各种子数据帧
	i2c_master_start(cmd_haldle);					//起始信号
   	i2c_master_write_byte(cmd_haldle, (PN532_SLAVE_ADDR << 1) | I2C_MASTER_READ, true);	//从机地址及读写位
	i2c_master_read(cmd_haldle, buf, 1024, I2C_MASTER_LAST_NACK);
   	i2c_master_stop(cmd_haldle);					//终止信号
//向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd_haldle, 1000 / portTICK_PERIOD_MS);
	return buf;
}

int Clay_PN532_I2C::wake() {
	const uint8_t PN532_WAKE[] = {0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x01, 0x00, 0xE1, 0x00};
	printf("wake.PN532_WAKE:");
	for (size_t i = 0; i < 11; i++)
	{
		printf("%d ", PN532_WAKE[i]);
	}
	printf("\n");
    err = write(PN532_WAKE, sizeof(PN532_WAKE) / sizeof(PN532_WAKE[0]));
	printf("write:%d | %s\n", err, esp_err_to_name(err));
	uint8_t* res = read();
	for (size_t i = 0; i < 256; i++)
	{
		printf("%d ", res[i]);
	}
	printf("\n");
	return ESP_OK;
}