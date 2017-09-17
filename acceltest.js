"use strong";

const i2c = require('i2c-bus');
const MPU6050 = require('i2c-mpu6050');

const address = 0x68;
const i2c1 = i2c.open(1, function (err) {
	if (err){console.error(err)};

	const sensor = MPU6050(i2c1, address);

	(function read() {
		sensor.read(function (err, data) {
			if (err){ console.error(err)};
      console.log(data);
      
			setTimeout(read,500);
		});
	}());
});
