## Install

compile the device tree file

        dtc -O dtb -o BB-BONE-AUDI-MOD-02-00A0.dtbo -b 0 -@ BB-BONE-AUDI-MOD-02-00A0.dts
    
copy it over the original cape file

        cp BB-BONE-AUDI-MOD-02-00A0.dtbo /lib/firmware/BB-BONE-AUDI-02-00A0.dtbo
   
reboot
    
## Use

After boot, initalize the cape

        echo BB-BONE-AUDI-02 > /sys/devices/bone*/slots

## Testing

playback a wav file

        aplay file.wav
    
record a file

        arecord -c 2 -r 48000 -d 10 -vv test.wav

where `-c` is the channel count (stereo), `-r` is the sample rate, `-d` is the duration in seconds, and `-vv` is super-verbose mode.
