# dmloop

## environment

have to create a zero-linear dm-device before take it;

```shell
dmsetup create zero --table "0 100000000 zero" 
#0 and 100000000 point the number of sector.
```

need to cover the real device by a dm-device-layer, and mkfs on the dm-device-layer,
while get the true file-mapped on the real device.

```shell
DM_DEVICE_LAYER_NAME="n1p11"
REAL_DEVICE_NAME="/dev/nvme0n1p11"

dmsetup create $DM_DEVICE_LAYER_NAME --table "0 `blockdev --getsz /dev/$REAL_DEVICE_NAME` linear /dev/$REAL_DEVICE_NAME 0"
mkfs.ext4 /dev/mapper/$DM_DEVICE_LAYER_NAME
mkdir /mnt/test/
mount /dev/mapper/$DM_DEVICE_LAYER_NAME /mnt/test
```

##  execute

```shell
mkdir build
cd build
#default configure is used for debug, don't execute the command printed in terminal
	cmake  .. 
#config debug is OFF, and ensure execute the command
	cmake -D_DEBUG=OFF -D_EXEC=ON ..
make
sudo ./dmloop file_path source_device target_device_name sector_size
# file is saved in /mnt/test/...., file_path likes /mnt/test/....
# source_device is /dev/REAL_DEVICE_NAME
# target_device_name is the dm-device this command will create
# sector_size is sector size of this block device
```